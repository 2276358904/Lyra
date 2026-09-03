// Fill out your copyright notice in the Description page of Project Settings.


#include "System/LyraAssetManager.h"
#include "Character/LyraPawnData.h"

DEFINE_LOG_CATEGORY(LogLyraAsset);

ULyraAssetManager& ULyraAssetManager::Get()
{
	if (ULyraAssetManager* Singleton = Cast<ULyraAssetManager>(GEngine->AssetManager))
	{
		return *Singleton;
	}
	// Fatal error above prevents this from being called.
	return *NewObject<ULyraAssetManager>();
}

void ULyraAssetManager::DumpLoadedAssets()
{
	UE_LOG(LogLyraAsset, Log, TEXT("========== Start Dumping Loaded Assets =========="));

	for (const UObject* LoadedAsset : Get().LoadedAssets)
	{
		UE_LOG(LogLyraAsset, Log, TEXT("  %s"), *GetNameSafe(LoadedAsset));
	}

	UE_LOG(LogLyraAsset, Log, TEXT("... %d assets in loaded pool"), Get().LoadedAssets.Num());
	UE_LOG(LogLyraAsset, Log, TEXT("========== Finish Dumping Loaded Assets =========="));
}

ULyraAssetManager::ULyraAssetManager()
{
	DefaultPawnData = nullptr;
}

const ULyraPawnData* ULyraAssetManager::GetDefaultPawnData() const
{
	return GetAsset(DefaultPawnData);
}

const ULyraGameData& ULyraAssetManager::GetGameData()
{
	return *GetAsset(DefaultGameData);
}

bool ULyraAssetManager::ShouldLogAssetLoads()
{
	return false;
}

UObject* ULyraAssetManager::SynchronousLoadAsset(const FSoftObjectPath& AssetPath)
{
	if (!AssetPath.IsValid())
	{
		return nullptr;
	}
	TUniquePtr<FScopeLogTime> LogTimePtr;
	if (ShouldLogAssetLoads())
	{
		LogTimePtr = MakeUnique<FScopeLogTime>(*FString::Printf(TEXT("Synchronously loaded asset [%s]"), *AssetPath.ToString()), nullptr, FScopeLogTime::ScopeLog_Seconds);
	}
	if (UAssetManager::IsValid())
	{
		return UAssetManager::GetStreamableManager().LoadSynchronous(AssetPath, false);
	}
	// Use LoadObject if asset manager isn't ready yet.
	return AssetPath.TryLoad();
}

void ULyraAssetManager::AddLoadedAsset(const UObject* Asset)
{
	if (Asset)
	{
		FScopeLock LoadedAssetsLock(&LoadedAssetsCritical);
		LoadedAssets.Add(Asset);
	}
}
