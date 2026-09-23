// Fill out your copyright notice in the Description page of Project Settings.


#include "Development/LyraDeveloperSettings.h"

ULyraDeveloperSettings::ULyraDeveloperSettings(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{

}

FName ULyraDeveloperSettings::GetCategoryName() const
{
	return FApp::GetProjectName();
}
