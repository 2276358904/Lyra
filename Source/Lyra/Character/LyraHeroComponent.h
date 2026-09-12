// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "LyraHeroComponent.generated.h"

struct FLyraMappableConfigPair;
struct FInputActionValue;

/**
 * 
 */
UCLASS()
class LYRA_API ULyraHeroComponent : public UPawnComponent, public IGameFrameworkInitStateInterface
{
	GENERATED_BODY()

public:
	/** The name of this component-implemented feature */
	static const FName NAME_ActorFeatureName;
	
public:
	ULyraHeroComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~ Begin UPawnComponent interface
	virtual void OnRegister() override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End UPawnComponent interface

	//~ Begin IGameFrameworkInitStateInterface interface
	virtual FName GetFeatureName() const override;

	virtual bool CanChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) const override;

	virtual void HandleChangeInitState(UGameFrameworkComponentManager* Manager, FGameplayTag CurrentState, FGameplayTag DesiredState) override;
	
	virtual void OnActorInitStateChanged(const FActorInitStateChangedParams& Params) override;

	virtual void CheckDefaultInitialization() override;
	//~ End IGameFrameworkInitStateInterface interface

protected:
	/**
	 * Input Configs that should be added to this player when initializing the input. These configs
	 * will NOT be registered with the settings because they are added at runtime. If you want the config
	 * pair to be in the settings, then add it via the GameFeatureAction_AddInputConfig
	 */
	UPROPERTY(EditAnywhere)
	TArray<FLyraMappableConfigPair> DefaultInputConfigs;

protected:
	virtual void InitializePlayerInput(UInputComponent* PlayerInputComponent);

private:
	void OnInputMove(const FInputActionValue& InputActionValue);

	void OnInputLook(const FInputActionValue& InputActionValue);

	void OnInputCrouch(const FInputActionValue& InputActionValue);

	void OnInputAutoRun(const FInputActionValue& InputActionValue);

	void OnAbilityInputTagPressed(FGameplayTag InputTag);

	void OnAbilityInputTagReleased(FGameplayTag InputTag);
};
