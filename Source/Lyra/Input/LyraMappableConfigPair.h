// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LyraMappableConfigPair.generated.h"

class UPlayerMappableInputConfig;

/**
 * A container to organize potentially unloaded player mappable configs to their CommonUI input type
 */
USTRUCT()
struct FLyraMappableConfigPair
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UPlayerMappableInputConfig> Config;

	/**
	 * Container of platform traits that must be set in order for this input to be activated.
	 *
	 * If the platform does not have one of the traits specified it can still be registered, but cannot
	 * be activated.
	 */
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer DependentPlatformTraits;

	/**
	 * If the current platform has any of these traits, then this config will not be actived.
	 */
	UPROPERTY(EditAnywhere)
	FGameplayTagContainer ExcludedPlatformTraits;

	/** 
	 * If true, then this input config will be activated when it's associated Game Feature is activated.
	 * This is normally the desirable behavior.
	 */
	UPROPERTY(EditAnywhere)
	bool bShouldActivateAutomatically = true;

public:
	/** Returns true if this config pair can be activated based on the current platform traits and settings. */
	bool CanBeActivated() const;
};
