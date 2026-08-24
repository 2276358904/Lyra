// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LyraCharacterMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class LYRA_API ULyraCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	ULyraCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);

	//~UMovementComponent interface
	virtual FRotator GetDeltaRotation(float DeltaTime) const override;

	virtual float GetMaxSpeed() const override;
	//~End of UMovementComponent interface
};
