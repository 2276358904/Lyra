// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "LyraDeveloperSettings.generated.h"

class ULyraExperienceDefinition;

/**
 * Developer settings / editor cheats
 */
UCLASS(Config = EditorPerProjectUserSettings)
class LYRA_API ULyraDeveloperSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:
	// The experience override to use for Play in Editor (if not set, the default for the world settings of the open map will be used)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, config, Category = Lyra, meta = (AllowedTypes = "LyraExperienceDefinition"))
	FPrimaryAssetId ExperienceOverride;
	
public:
	ULyraDeveloperSettings(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UDeveloperSettings interface
	virtual FName GetCategoryName() const override;
	//~End of UDeveloperSettings interface
};
