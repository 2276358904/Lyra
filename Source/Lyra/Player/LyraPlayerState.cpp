// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LyraPlayerState.h"
#include "Abilities/LyraAbilitySet.h"
#include "Abilities/LyraAbilitySystemComponent.h"
#include "Character/LyraPawnData.h"
#include "GameModes/LyraExperienceDefinition.h"
#include "GameModes/LyraExperienceManagerComponent.h"

ALyraPlayerState::ALyraPlayerState(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<ULyraAbilitySystemComponent>(TEXT("AbilitySystem"));
}

void ALyraPlayerState::SetPawnData(const ULyraPawnData* InPawnData)
{
	if (GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	if (PawnData)
	{
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

void ALyraPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	UWorld* World = GetWorld();
	if (World && World->IsGameWorld() && World->GetNetMode() != NM_Client)
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		ULyraExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<ULyraExperienceManagerComponent>();
		ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnLyraExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
}

UAbilitySystemComponent* ALyraPlayerState::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void ALyraPlayerState::OnExperienceLoaded(const ULyraExperienceDefinition* CurrentExperience)
{
	const ULyraPawnData* NewPawnData = CurrentExperience->DefaultPawnData;
	if (NewPawnData)
	{
		SetPawnData(NewPawnData);
	}
}
