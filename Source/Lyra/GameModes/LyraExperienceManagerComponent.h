// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameStateComponent.h"
#include "LyraExperienceManagerComponent.generated.h"

namespace UE::GameFeatures { struct FResult; }

class ULyraExperienceDefinition;
class ULyraGameFeatureAction;

struct FGameFeatureActivatingContext;
struct FGameFeatureDeactivatingContext;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLyraExperienceLoaded, const ULyraExperienceDefinition* /*Experience*/);

enum class ELyraExperienceLoadState
{
	Unloaded,
	Loading,
	LoadingGameFeatures,
	LoadingChaosTestingDelay,
	ExecutingActions,
	Loaded,
	Deactivating
};

/**
 * 
 */
UCLASS()
class LYRA_API ULyraExperienceManagerComponent : public UGameStateComponent
{
	GENERATED_BODY()
	
public:
	ULyraExperienceManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Tries to set the current experience, either a UI or gameplay one
	void SetCurrentExperience(FPrimaryAssetId ExperienceId);

	// This returns the current experience if it is fully loaded, asserting otherwise
	// (i.e., if you called it too soon)
	const ULyraExperienceDefinition* GetCurrentExperience() const;

	// Ensures the delegate is called once the experience has been loaded
	// If the experience has already loaded, calls the delegate immediately
	void CallOrRegister_OnExperienceLoaded(FOnLyraExperienceLoaded::FDelegate&& Delegate);

	// Returns true if the experience is fully loaded
	bool IsExperienceLoaded() const;

	//~UActorComponent interface
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of UActorComponent interface

private:
	UPROPERTY()
	TObjectPtr<const ULyraExperienceDefinition> CurrentExperience;

	ELyraExperienceLoadState LoadState = ELyraExperienceLoadState::Unloaded;

	int32 NumGameFeaturePluginsLoading = 0;
	TArray<FString> GameFeaturePluginURLs;

	int32 NumObservedPausers = 0;
	int32 NumExpectedPausers = 0;

	/** Delegate called when the experience has finished loading */
	FOnLyraExperienceLoaded OnExperienceLoaded;

private:
	void StartExperienceLoad();

	void EndExperienceLoad();

	void OnExperienceLoadComplete();

	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);

	void OnExperienceFullLoadCompleted();

	void OnExperienceUnload();

	void OnActionDeactivationCompleted();

	void OnAllActionsDeactivated();

	void CollectGameFeaturePluginURLs(const UPrimaryDataAsset* Context, const TArray<FString>& FeaturePluginList);

	void ActivateListOfActions(FGameFeatureActivatingContext Context, const TArray<ULyraGameFeatureAction*>& ActionList);

	void DeactivateListOfActions(FGameFeatureDeactivatingContext Context, const TArray<ULyraGameFeatureAction*>& ActionList);
};
