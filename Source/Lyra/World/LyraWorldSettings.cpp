// Fill out your copyright notice in the Description page of Project Settings.


#include "World/LyraWorldSettings.h"

#include "Engine/AssetManager.h"

ALyraWorldSettings::ALyraWorldSettings(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{

}

FPrimaryAssetId ALyraWorldSettings::GetDefaultGameplayExperience() const
{
	FPrimaryAssetId Result;
	if (!DefaultGameplayExperience.IsNull())
	{
		Result = UAssetManager::Get().GetPrimaryAssetIdForPath(DefaultGameplayExperience.ToSoftObjectPath());
	}
	return Result;
}
