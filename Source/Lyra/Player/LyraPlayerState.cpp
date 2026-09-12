// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LyraPlayerState.h"
#include "Abilities/LyraAbilitySet.h"
#include "Character/LyraPawnData.h"

DEFINE_LOG_CATEGORY(LogLyraPlayerState);

void ALyraPlayerState::SetPawnData(const ULyraPawnData* InPawnData)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	if (PawnData)
	{
		UE_LOG(LogLyraPlayerState, Error, TEXT("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s]."), *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}
	PawnData = InPawnData;
	for (const ULyraAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, nullptr);
		}
	}
}

UAbilitySystemComponent* ALyraPlayerState::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}
