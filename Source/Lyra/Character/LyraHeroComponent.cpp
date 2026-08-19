// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LyraHeroComponent.h"
#include "LyraGameplayTags.h"
#include "Character/LyraPawnExtensionComponent.h"
#include "Player/LyraPlayerState.h"
#include "Player/LyraPlayerController.h"

#include "Components/GameFrameworkComponentManager.h"

const FName ULyraHeroComponent::NAME_ActorFeatureName("Hero");

ULyraHeroComponent::ULyraHeroComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{

}

void ULyraHeroComponent::OnRegister()
{
	Super::OnRegister();
	// Register with the init state system early, this will only work if this is a game world
	RegisterInitStateFeature();
}

void ULyraHeroComponent::BeginPlay()
{
	Super::BeginPlay();
	// Listen for when the pawn extension component changes init state
	BindOnActorInitStateChanged(ULyraPawnExtensionComponent::NAME_ActorFeatureName, FGameplayTag(), false);
	// Notifies that we are done spawning, then try the rest of initialization
	CheckDefaultInitialization();
}

void ULyraHeroComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnregisterInitStateFeature();
	Super::EndPlay(EndPlayReason);
}

FName ULyraHeroComponent::GetFeatureName() const
{
	return NAME_ActorFeatureName;
}

bool ULyraHeroComponent::CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const
{
	APawn* Pawn = GetPawn<APawn>();
	if (!CurrentState.IsValid() && DesiredState == LyraGameplayTags::Gameplay_InitState_Spawned)
	{
		// As long as we have a real pawn, let us transition
		if (Pawn)
		{
			return true;
		}
	}
	else if (CurrentState == LyraGameplayTags::Gameplay_InitState_Spawned && DesiredState == LyraGameplayTags::Gameplay_InitState_DataAvailable)
	{
		// The player state is required.
		if (!GetPlayerState<ALyraPlayerState>())
		{
			return false;
		}
		// If we're authority or autonomous, we need to wait for a controller with registered ownership of the player state.
		if (Pawn->GetLocalRole() != ROLE_SimulatedProxy)
		{
			AController* Controller = GetController<AController>();
			const bool bHasControllerPairedWithPS = (Controller != nullptr) && (Controller->PlayerState != nullptr) && (Controller->PlayerState->GetOwner() == Controller);
			if (!bHasControllerPairedWithPS)
			{
				return false;
			}
			const bool bIsLocallyControlled = Pawn->IsLocallyControlled();
			const bool bIsBot = Pawn->IsBotControlled();
			if (bIsLocallyControlled && !bIsBot)
			{
				ALyraPlayerController* LyraPC = GetController<ALyraPlayerController>();
				// The input component and local player is required when locally controlled.
				if (!Pawn->InputComponent || !LyraPC || !LyraPC->GetLocalPlayer())
				{
					return false;
				}
			}
			return true;
		}
	}
	else if (CurrentState == LyraGameplayTags::Gameplay_InitState_DataAvailable && DesiredState == LyraGameplayTags::Gameplay_InitState_DataInitialized)
	{
		// Wait for player state and extension component
		ALyraPlayerState* LyraPS = GetPlayerState<ALyraPlayerState>();
		return LyraPS && Manager->HasFeatureReachedInitState(Pawn, ULyraPawnExtensionComponent::NAME_ActorFeatureName, LyraGameplayTags::Gameplay_InitState_DataInitialized);
	}
	else if (CurrentState == LyraGameplayTags::Gameplay_InitState_DataInitialized && DesiredState == LyraGameplayTags::Gameplay_InitState_GameplayReady)
	{
		return true;
	}
	return false;
}

void ULyraHeroComponent::HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState)
{
}

void ULyraHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{
}

void ULyraHeroComponent::CheckDefaultInitialization()
{
}
