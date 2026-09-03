// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LyraHeroComponent.h"
#include "LyraGameplayTags.h"
#include "Abilities/LyraAbilitySystemComponent.h"
#include "Character/LyraCharacter.h"
#include "Character/LyraPawnData.h"
#include "Character/LyraPawnExtensionComponent.h"
#include "Player/LyraPlayerState.h"
#include "Player/LyraPlayerController.h"
#include "Player/LyraLocalPlayer.h"
#include "Input/LyraInputConfig.h"
#include "Input/LyraInputComponent.h"
#include "Input/LyraMappableConfigPair.h"

#include "EnhancedInputSubsystems.h"
#include "PlayerMappableInputConfig.h"
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
	TryToChangeInitState(LyraGameplayTags::Gameplay_InitState_Spawned);
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
		if (Pawn->GetLocalRole() == ROLE_SimulatedProxy)
		{
			return false;
		}
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
	if (CurrentState == LyraGameplayTags::Gameplay_InitState_DataAvailable && DesiredState == LyraGameplayTags::Gameplay_InitState_DataInitialized)
	{

	}
}

void ULyraHeroComponent::OnActorInitStateChanged(const FActorInitStateChangedParams& Params)
{

}

void ULyraHeroComponent::CheckDefaultInitialization()
{

}

void ULyraHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}
	const APlayerController* PlayerController = GetController<APlayerController>();
	if (!PlayerController)
	{
		return;
	}
	const ULyraLocalPlayer* LocalPlayer = Cast<ULyraLocalPlayer>(PlayerController->GetLocalPlayer());
	if (!LocalPlayer)
	{
		return;
	}
	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!Subsystem)
	{
		return;
	}
	Subsystem->ClearAllMappings();
	// Register any default input configs with the settings so that they will be applied to the player during AddInputMappings
	for (const FLyraMappableConfigPair& Pair : DefaultInputConfigs)
	{
		if (Pair.bShouldActivateAutomatically && Pair.CanBeActivated())
		{
			FModifyContextOptions Options = {};
			Options.bIgnoreAllPressedKeysUntilRelease = false;
			// Actually add the config to the local player							
			Subsystem->AddPlayerMappableConfig(Pair.Config.LoadSynchronous(), Options);
		}
	}
	const ULyraPawnExtensionComponent* PawnExtensionComponent = Pawn->FindComponentByClass<ULyraPawnExtensionComponent>();
	if (!PawnExtensionComponent)
	{
		return;
	}
	const ULyraPawnData* PawnData = PawnExtensionComponent->GetPawnData<ULyraPawnData>();
	if (!PawnData)
	{
		return;
	}
	const ULyraInputConfig* InputConfig = PawnData->InputConfig;
	if (InputConfig)
	{
		return;
	}
	// The Lyra Input Component has some additional functions to map Gameplay Tags to an Input Action.
	// If you want this functionality but still want to change your input component class, make it a subclass
	// of the ULyraInputComponent or modify this component accordingly.
	ULyraInputComponent* InputComponent = Cast<ULyraInputComponent>(PlayerInputComponent);
	if (InputComponent)
	{
		InputComponent->BindNativeAction(InputConfig, LyraGameplayTags::Gameplay_Input_Move, ETriggerEvent::Triggered, this, &ThisClass::OnInputMove, /*bLogIfNotFound=*/ false);
		InputComponent->BindNativeAction(InputConfig, LyraGameplayTags::Gameplay_Input_Look, ETriggerEvent::Triggered, this, &ThisClass::OnInputLook, /*bLogIfNotFound=*/ false);
		InputComponent->BindNativeAction(InputConfig, LyraGameplayTags::Gameplay_Input_Crouch, ETriggerEvent::Triggered, this, &ThisClass::OnInputCrouch, /*bLogIfNotFound=*/ false);
		InputComponent->BindNativeAction(InputConfig, LyraGameplayTags::Gameplay_Input_AutoRun, ETriggerEvent::Triggered, this, &ThisClass::OnInputAutoRun, /*bLogIfNotFound=*/ false);

		// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
		// be triggered directly by these input actions Triggered events. 
		TArray<uint32> BindHandles;
		InputComponent->BindAbilityActions(InputConfig, this, &ThisClass::OnAbilityInputTagPressed, &ThisClass::OnAbilityInputTagReleased, /*out*/ BindHandles);
	}
}

void ULyraHeroComponent::OnInputMove(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	AController* Controller = Pawn ? Pawn->GetController() : nullptr;
	if (!Controller)
	{
		return;
	}
	// If the player has attempted to move again then cancel auto running
	if (ALyraPlayerController* PlayerController = Cast<ALyraPlayerController>(Controller))
	{
		PlayerController->SetIsAutoRunning(false);
	}
	const FVector2D Value = InputActionValue.Get<FVector2D>();
	const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
	if (Value.X != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
		Pawn->AddMovementInput(MovementDirection, Value.X);
	}
	if (Value.Y != 0.0f)
	{
		const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
		Pawn->AddMovementInput(MovementDirection, Value.Y);
	}
}

void ULyraHeroComponent::OnInputLook(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}
	const FVector2D Value = InputActionValue.Get<FVector2D>();
	if (Value.X != 0.0f)
	{
		Pawn->AddControllerYawInput(Value.X);
	}
	if (Value.Y != 0.0f)
	{
		Pawn->AddControllerPitchInput(Value.Y);
	}
}

void ULyraHeroComponent::OnInputCrouch(const FInputActionValue& InputActionValue)
{
	if (ALyraCharacter* Character = GetPawn<ALyraCharacter>())
	{
		Character->ToggleCrouch();
	}
}

void ULyraHeroComponent::OnInputAutoRun(const FInputActionValue& InputActionValue)
{
	APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}
	ALyraPlayerController* PlayerController = Cast<ALyraPlayerController>(Pawn->GetController());
	if (PlayerController)
	{
		// Toggle auto running
		PlayerController->SetIsAutoRunning(!PlayerController->GetIsAutoRunning());
	}
}

void ULyraHeroComponent::OnAbilityInputTagPressed(FGameplayTag InputTag)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}
	const ULyraPawnExtensionComponent* PawnExtensionComponent = Pawn->FindComponentByClass<ULyraPawnExtensionComponent>();
	if (!PawnExtensionComponent)
	{
		return;
	}
	ULyraAbilitySystemComponent* AbilitySystemComponent = Cast<ULyraAbilitySystemComponent>(PawnExtensionComponent->GetAbilitySystemComponent());
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityInputTagPressed(InputTag);
	}
}

void ULyraHeroComponent::OnAbilityInputTagReleased(FGameplayTag InputTag)
{
	const APawn* Pawn = GetPawn<APawn>();
	if (!Pawn)
	{
		return;
	}
	const ULyraPawnExtensionComponent* PawnExtensionComponent = Pawn->FindComponentByClass<ULyraPawnExtensionComponent>();
	if (!PawnExtensionComponent)
	{
		return;
	}
	ULyraAbilitySystemComponent* AbilitySystemComponent = Cast<ULyraAbilitySystemComponent>(PawnExtensionComponent->GetAbilitySystemComponent());
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->AbilityInputTagReleased(InputTag);
	}
}
