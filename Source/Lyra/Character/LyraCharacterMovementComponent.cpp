// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LyraCharacterMovementComponent.h"

ULyraCharacterMovementComponent::ULyraCharacterMovementComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{

}

FRotator ULyraCharacterMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	return Super::GetDeltaRotation(DeltaTime);
}

float ULyraCharacterMovementComponent::GetMaxSpeed() const
{
	return Super::GetMaxSpeed();
}
