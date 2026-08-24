// Copyright Epic Games, Inc. All Rights Reserved.

#include "Character/LyraCharacter.h"
#include "Character/LyraCharacterMovementComponent.h"


ALyraCharacter::ALyraCharacter(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{

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









