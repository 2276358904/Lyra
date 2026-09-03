// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/LyraAbilitySystemComponent.h"
#include "Abilities/LyraGlobalAbilitySystem.h"
#include "Abilities/LyraGameplayAbility.h"
#include "Animation/LyraAnimInstance.h"

#include "Abilities/LyraAttributeSet.h"
#include "Abilities/LyraGameplayEffect.h"
#include "Abilities/LyraAbilitySystemGlobals.h"

ULyraAbilitySystemComponent::ULyraAbilitySystemComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{

	
}

void ULyraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
		{
			InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
			InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
		}
	}
}

void ULyraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
		{
			InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
			InputHeldSpecHandles.Remove(AbilitySpec.Handle);
		}
	}
}

void ULyraAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	// Process all abilities that activate when the input is held.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle);
		if (!AbilitySpec || !AbilitySpec->Ability || AbilitySpec->IsActive())
		{
			continue;
		}
		const ULyraGameplayAbility* AbilityCDO = CastChecked<ULyraGameplayAbility>(AbilitySpec->Ability);
		if (AbilityCDO->GetActivationPolicy() == ELyraAbilityActivationPolicy::WhileInputActive)
		{
			AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
		}
	}
	// Process all abilities that had their input pressed this frame.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle);
		if (!AbilitySpec || !AbilitySpec->Ability)
		{
			continue;
		}
		AbilitySpec->InputPressed = true;
		if (AbilitySpec->IsActive())
		{
			// Ability is active so pass along the input event.
			AbilitySpecInputPressed(*AbilitySpec);
			continue;
		}
		const ULyraGameplayAbility* AbilityCDO = CastChecked<ULyraGameplayAbility>(AbilitySpec->Ability);
		if (AbilityCDO->GetActivationPolicy() == ELyraAbilityActivationPolicy::OnInputTriggered)
		{
			AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
		}
	}
	// Try to activate all the abilities that are from presses and holds.
	// We do it all at once so that held inputs don't activate the ability
	// and then also send a input event to the ability because of the press.
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}
	// Process all abilities that had their input released this frame.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle);
		if (!AbilitySpec || !AbilitySpec->Ability)
		{
			continue;
		}
		AbilitySpec->InputPressed = false;
		if (AbilitySpec->IsActive())
		{
			// Ability is active so pass along the input event.
			AbilitySpecInputReleased(*AbilitySpec);
		}
	}
	// Clear the cached ability handles.
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void ULyraAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void ULyraAbilitySystemComponent::GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle)
{
	TSharedPtr<FAbilityReplicatedDataCache> ReplicatedData = AbilityTargetDataMap.Find(FGameplayAbilitySpecHandleAndPredictionKey(AbilityHandle, ActivationInfo.GetActivationPredictionKey()));
	if (ReplicatedData.IsValid())
	{
		OutTargetDataHandle = ReplicatedData->TargetData;
	}
}

void ULyraAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ULyraGlobalAbilitySystem* GlobalAbilitySystem = UWorld::GetSubsystem<ULyraGlobalAbilitySystem>(GetWorld()))
	{
		GlobalAbilitySystem->UnregisterASC(this);
	}
	Super::EndPlay(EndPlayReason);
}

void ULyraAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && (InAvatarActor != ActorInfo->AvatarActor);
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
	if (!bHasNewPawnAvatar)
	{
		return;
	}
	// Register with the global system once we actually have a pawn avatar. We wait until this time since some globally-applied effects may require an avatar.
	if (ULyraGlobalAbilitySystem* GlobalAbilitySystem = UWorld::GetSubsystem<ULyraGlobalAbilitySystem>(GetWorld()))
	{
		GlobalAbilitySystem->RegisterASC(this);
	}
	if (ULyraAnimInstance* AnimInstance = Cast<ULyraAnimInstance>(ActorInfo->GetAnimInstance()))
	{
		AnimInstance->InitializeWithAbilitySystem(this);
	}
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		const ULyraGameplayAbility* AbilityCDO = CastChecked<ULyraGameplayAbility>(AbilitySpec.Ability);
		AbilityCDO->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), AbilitySpec);
	}
}
