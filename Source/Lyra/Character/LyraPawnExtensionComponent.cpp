// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LyraPawnExtensionComponent.h"
#include "LyraGameplayTags.h"
#include "Abilities/LyraAbilitySystemComponent.h"

#include "Components/GameFrameworkComponentManager.h"

const FName ULyraPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

ULyraPawnExtensionComponent::ULyraPawnExtensionComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{

}

void ULyraPawnExtensionComponent::SetPawnData(const ULyraPawnData* InPawnData)
{
	APawn* Pawn = GetPawnChecked<APawn>();
	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	if (PawnData)
	{
		return;
	}
	PawnData = InPawnData;
	CheckDefaultInitialization();
}

void ULyraPawnExtensionComponent::InitializeAbilitySystem(ULyraAbilitySystemComponent* InASC, AActor* InOwnerActor)
{
	if (AbilitySystemComponent == InASC)
	{
		// The ability system component hasn't changed.
		return;
	}
	if (AbilitySystemComponent)
	{
		// Clean up the old ability system component.
		UninitializeAbilitySystem();
	}
	APawn* Pawn = GetPawnChecked<APawn>();
	AActor* ExistingAvatar = InASC->GetAvatarActor();
	if (ExistingAvatar != nullptr && ExistingAvatar != Pawn)
	{
		// There is already a pawn acting as the ASC's avatar, so we need to kick it out
		// This can happen on clients if they're lagged: their new pawn is spawned + possessed before the dead one is removed
		ULyraPawnExtensionComponent* OtherExtensionComponent = ExistingAvatar->FindComponentByClass<ULyraPawnExtensionComponent>();
		if (OtherExtensionComponent)
		{
			OtherExtensionComponent->UninitializeAbilitySystem();
		}
	}
	AbilitySystemComponent = InASC;
	AbilitySystemComponent->InitAbilityActorInfo(InOwnerActor, Pawn);
	OnAbilitySystemInitialized.Broadcast();
}

void ULyraPawnExtensionComponent::UninitializeAbilitySystem()
{
	if (!AbilitySystemComponent)
	{
		return;
	}
	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (AbilitySystemComponent->GetAvatarActor() == GetOwner())
	{
		FGameplayTagContainer AbilityTypesToIgnore;
		AbilitySystemComponent->CancelAbilities(nullptr, &AbilityTypesToIgnore);
		AbilitySystemComponent->ClearAbilityInput();
		AbilitySystemComponent->RemoveAllGameplayCues();
		if (AbilitySystemComponent->GetOwnerActor() != nullptr)
		{
			AbilitySystemComponent->SetAvatarActor(nullptr);
		}
		else
		{
			// If the ASC doesn't have a valid owner, we need to clear *all* actor info, not just the avatar pairing
			AbilitySystemComponent->ClearActorInfo();
		}
		OnAbilitySystemUninitialized.Broadcast();
	}
	AbilitySystemComponent = nullptr;
}

void ULyraPawnExtensionComponent::HandleControllerChanged()
{
	if (AbilitySystemComponent && (AbilitySystemComponent->GetAvatarActor() == GetPawnChecked<APawn>()))
	{
		if (AbilitySystemComponent->GetOwnerActor() == nullptr)
		{
			UninitializeAbilitySystem();
		}
		else
		{
			AbilitySystemComponent->RefreshAbilityActorInfo();
		}
	}
	CheckDefaultInitialization();
}

void ULyraPawnExtensionComponent::SetupPlayerInputComponent()
{
	CheckDefaultInitialization();
}

void ULyraPawnExtensionComponent::OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemInitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemInitialized.Add(Delegate);
	}
	if (AbilitySystemComponent)
	{
		Delegate.Execute();
	}
}

void ULyraPawnExtensionComponent::OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate Delegate)
{
	if (!OnAbilitySystemUninitialized.IsBoundToObject(Delegate.GetUObject()))
	{
		OnAbilitySystemUninitialized.Add(Delegate);
	}
}

bool ULyraPawnExtensionComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	APawn* Pawn = GetPawn<APawn>();
	if (!CurrentState.IsValid() && DesiredState == LyraGameplayTags::Gameplay_InitState_Spawned)
	{
		// As long as we are on a valid pawn, we count as spawned
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == LyraGameplayTags::Gameplay_InitState_Spawned && DesiredState == LyraGameplayTags::Gameplay_InitState_DataAvailable)
	{
		// Pawn data is required.
		if (!PawnData)
		{
			return false;
		}
		const bool bHasAuthority = Pawn->HasAuthority();
		const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
		if (bHasAuthority || bIsLocallyControlled)
		{
			// Check for being possessed by a controller.
			if (!GetController<AController>())
			{
				return false;
			}
		}
		return true;
	}
	else if (CurrentState == LyraGameplayTags::Gameplay_InitState_DataAvailable && DesiredState == LyraGameplayTags::Gameplay_InitState_DataInitialized)
	{
		// Transition to initialize if all features have their data available
		return Manager->HaveAllFeaturesReachedInitState(Pawn, LyraGameplayTags::Gameplay_InitState_DataAvailable);
	}
	else if (CurrentState == LyraGameplayTags::Gameplay_InitState_DataInitialized && DesiredState == LyraGameplayTags::Gameplay_InitState_GameplayReady)
	{
		return true;
	}
	return false;
}

void ULyraPawnExtensionComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
	if (DesiredState == LyraGameplayTags::Gameplay_InitState_DataInitialized)
	{
		// This is currently all handled by other components listening to this state change
	}
}

void ULyraPawnExtensionComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
	// If another feature is now in DataAvailable, see if we should transition to DataInitialized
	if (Params.FeatureName != NAME_ActorFeatureName && Params.FeatureState == LyraGameplayTags::Gameplay_InitState_DataAvailable)
	{
		CheckDefaultInitialization();
	}
}

void ULyraPawnExtensionComponent::CheckDefaultInitialization()
{
	// Before checking our progress, try progressing any other features we might depend on
	CheckDefaultInitializationForImplementers();
	const TArray<FGameplayTag> StateChain = { 
		LyraGameplayTags::Gameplay_InitState_Spawned,
		LyraGameplayTags::Gameplay_InitState_DataAvailable,
		LyraGameplayTags::Gameplay_InitState_DataInitialized,
		LyraGameplayTags::Gameplay_InitState_GameplayReady
	};
	// This will try to progress from spawned (which is only set in BeginPlay) through the data initialization stages until it gets to gameplay ready
	ContinueInitStateChain(StateChain);
}

UAbilitySystemComponent* ULyraPawnExtensionComponent::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
