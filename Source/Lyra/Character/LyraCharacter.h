// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "LyraCharacter.generated.h"

class ULyraHealthComponent;
class ULyraPawnExtensionComponent;

UCLASS()
class ALyraCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ALyraCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void ToggleCrouch();

	//~IAbilitySystemInterface interface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface interface

	//~ACharacter interface
	virtual void PossessedBy(AController* NewController) override;

	virtual void UnPossessed() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	//~End of ACharacter interface

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lyra|Character")
	TObjectPtr<ULyraHealthComponent> HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Lyra|Character")
	TObjectPtr<ULyraPawnExtensionComponent> PawnExtComponent;

protected:
	// Begins the death sequence for the character (disables collision, disables movement, etc...)
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	// Ends the death sequence for the character (detaches controller, destroys pawn, etc...)
	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

	virtual void OnAbilitySystemInitialized();

	virtual void OnAbilitySystemUninitialized();

private:
	void DisableMovementAndCollision();

	void UninitAndDestroy();
};

