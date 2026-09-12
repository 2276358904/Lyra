// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/LyraCharacter.h"
#include "LyraGameplayTags.h"
#include "Abilities/LyraAbilitySystemComponent.h"
#include "Character/LyraCharacterMovementComponent.h"
#include "Character/LyraHealthComponent.h"
#include "Character/LyraPawnExtensionComponent.h"

#include "Components/CapsuleComponent.h"

ALyraCharacter::ALyraCharacter(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	HealthComponent = CreateDefaultSubobject<ULyraHealthComponent>(TEXT("Health"));
	HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

	PawnExtComponent = CreateDefaultSubobject<ULyraPawnExtensionComponent>(TEXT("PawnExtension"));
	PawnExtComponent->OnAbilitySystemInitialized_RegisterAndCall(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemInitialized));
	PawnExtComponent->OnAbilitySystemUninitialized_Register(FSimpleMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnAbilitySystemUninitialized));
}

void ALyraCharacter::ToggleCrouch()
{
	const ULyraCharacterMovementComponent* MovementComponent = CastChecked<ULyraCharacterMovementComponent>(GetCharacterMovement());
	if (bIsCrouched || MovementComponent->bWantsToCrouch)
	{
		UnCrouch();
	}
	else if (MovementComponent->IsMovingOnGround())
	{
		Crouch();
	}
}

UAbilitySystemComponent* ALyraCharacter::GetAbilitySystemComponent() const
{
	return PawnExtComponent->GetAbilitySystemComponent();
}

void ALyraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	PawnExtComponent->HandleControllerChanged();
}

void ALyraCharacter::UnPossessed()
{
	Super::UnPossessed();
	PawnExtComponent->HandleControllerChanged();
}

void ALyraCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PawnExtComponent->SetupPlayerInputComponent();
}

void ALyraCharacter::FellOutOfWorld(const UDamageType& dmgType)
{
	HealthComponent->DamageSelfDestruct(/*bFellOutOfWorld=*/ true);
}

void ALyraCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	ULyraAbilitySystemComponent* AbilitySystemComponent = Cast<ULyraAbilitySystemComponent>(GetAbilitySystemComponent());
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(LyraGameplayTags::Gameplay_State_Crouching, 1);
	}
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void ALyraCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	ULyraAbilitySystemComponent* AbilitySystemComponent = Cast<ULyraAbilitySystemComponent>(GetAbilitySystemComponent());
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(LyraGameplayTags::Gameplay_State_Crouching, 0);
	}
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void ALyraCharacter::OnDeathStarted(AActor* OwningActor)
{
	DisableMovementAndCollision();
}

void ALyraCharacter::OnDeathFinished(AActor* OwningActor)
{
	UninitAndDestroy();
}

void ALyraCharacter::OnAbilitySystemInitialized()
{
	ULyraAbilitySystemComponent* AbilitySystemComponent = Cast<ULyraAbilitySystemComponent>(GetAbilitySystemComponent());
	HealthComponent->InitializeWithAbilitySystem(AbilitySystemComponent);
}

void ALyraCharacter::OnAbilitySystemUninitialized()
{
	HealthComponent->UninitializeFromAbilitySystem();
}

void ALyraCharacter::DisableMovementAndCollision()
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}
	UCapsuleComponent* CurrentCapsuleComponent = GetCapsuleComponent();
	CurrentCapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CurrentCapsuleComponent->SetCollisionResponseToAllChannels(ECR_Ignore);

	ULyraCharacterMovementComponent* CharacterMovementComponent = Cast<ULyraCharacterMovementComponent>(GetCharacterMovement());
	CharacterMovementComponent->StopMovementImmediately();
	CharacterMovementComponent->DisableMovement();
}

void ALyraCharacter::UninitAndDestroy()
{
	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}
	SetActorHiddenInGame(true);
}









