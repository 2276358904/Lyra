// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/LyraGameState.h"

ALyraGameState::ALyraGameState(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	ExperienceManagerComponent = CreateDefaultSubobject<ULyraExperienceManagerComponent>(TEXT("ExperienceManager"));

	AbilitySystemComponent = CreateDefaultSubobject<ULyraAbilitySystemComponent>(TEXT("AbilitySystem"));
}

UAbilitySystemComponent* ALyraGameState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
