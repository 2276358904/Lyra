// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "Components/GameFrameworkInitStateInterface.h"
#include "LyraHeroComponent.generated.h"

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
	ULyraHeroComponent(const FObjectInitializer& ObjectInitializer);

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
};
