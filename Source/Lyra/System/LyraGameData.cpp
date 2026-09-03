// Fill out your copyright notice in the Description page of Project Settings.


#include "System/LyraGameData.h"
#include "System/LyraAssetManager.h"

const ULyraGameData& ULyraGameData::Get()
{
	return ULyraAssetManager::Get().GetGameData();
}

ULyraGameData::ULyraGameData(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{

}
