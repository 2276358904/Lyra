// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "LyraAssetManager.generated.h"

class ULyraPawnData;
class ULyraGameData;

DECLARE_LOG_CATEGORY_EXTERN(LogLyraAsset, Log, All);

/**
 * 
 */
UCLASS(Config = Game)
class LYRA_API ULyraAssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	// Returns the AssetManager singleton object.
	static ULyraAssetManager& Get();

	// Returns the asset referenced by a TSoftObjectPtr.  This will synchronously load the asset if it's not already loaded.
	template<typename AssetType>
	static AssetType* GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	// Returns the subclass referenced by a TSoftClassPtr.  This will synchronously load the asset if it's not already loaded.
	template<typename AssetType>
	static TSubclassOf<AssetType> GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory = true);

	// Logs all assets currently loaded and tracked by the asset manager.
	static void DumpLoadedAssets();

	ULyraAssetManager();

	const ULyraPawnData* GetDefaultPawnData() const;

	const ULyraGameData& GetGameData();

private:
	// Assets loaded and tracked by the asset manager.
	UPROPERTY()
	TSet<TObjectPtr<const UObject>> LoadedAssets;

	// Used for a scope lock when modifying the list of load assets.
	FCriticalSection LoadedAssetsCritical;

	// Pawn data used when spawning player pawns if there isn't one set on the player state.
	UPROPERTY(Config)
	TSoftObjectPtr<ULyraPawnData> DefaultPawnData;

	// Global game data asset to use.
	UPROPERTY(Config)
	TSoftObjectPtr<ULyraGameData> DefaultGameData;

private:
	static bool ShouldLogAssetLoads();

	static UObject* SynchronousLoadAsset(const FSoftObjectPath& AssetPath);

	// Thread safe way of adding a loaded asset to keep in memory.
	void AddLoadedAsset(const UObject* Asset);
};

template<typename AssetType>
inline AssetType* ULyraAssetManager::GetAsset(const TSoftObjectPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	AssetType* LoadedAsset = nullptr;
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	if (!AssetPath.IsValid())
	{
		return LoadedAsset;
	}
	LoadedAsset = AssetPointer.Get();
	if (!LoadedAsset)
	{
		LoadedAsset = Cast<AssetType>(SynchronousLoadAsset(AssetPath));
	}
	if (LoadedAsset && bKeepInMemory)
	{
		// Added to loaded asset list.
		Get().AddLoadedAsset(Cast<UObject>(LoadedAsset));
	}
	return LoadedAsset;
}

template<typename AssetType>
inline TSubclassOf<AssetType> ULyraAssetManager::GetSubclass(const TSoftClassPtr<AssetType>& AssetPointer, bool bKeepInMemory)
{
	TSubclassOf<AssetType> LoadedSubclass;
	const FSoftObjectPath& AssetPath = AssetPointer.ToSoftObjectPath();
	if (!AssetPath.IsValid())
	{
		return LoadedSubclass;
	}
	LoadedSubclass = AssetPointer.Get();
	if (!LoadedSubclass)
	{
		LoadedSubclass = Cast<UClass>(SynchronousLoadAsset(AssetPath));
	}
	if (LoadedSubclass && bKeepInMemory)
	{
		// Added to loaded asset list.
		Get().AddLoadedAsset(Cast<UObject>(LoadedSubclass));
	}
	return LoadedSubclass;
}
