// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "LyraWorldSettings.generated.h"

class ULyraExperienceDefinition;

/**
 * The default world settings object, used primarily to set the default gameplay experience to use when playing on this map
 */
UCLASS()
class LYRA_API ALyraWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
	
public:
	ALyraWorldSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Returns the default experience to use when a server opens this map if it is not overridden by the user-facing experience
	FPrimaryAssetId GetDefaultGameplayExperience() const;

protected:
	// The default experience to use when a server opens this map if it is not overridden by the user-facing experience
	UPROPERTY(EditDefaultsOnly, Category = "GameMode")
	TSoftClassPtr<ULyraExperienceDefinition> DefaultGameplayExperience;
};
