// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LyraPawnExtensionComponent.h"
#include "Abilities/LyraAbilitySystemComponent.h"

const FName ULyraPawnExtensionComponent::NAME_ActorFeatureName("PawnExtension");

ULyraPawnExtensionComponent::ULyraPawnExtensionComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{

}

void ULyraPawnExtensionComponent::SetPawnData(const ULyraPawnData* InPawnData)
{
	APawn* Pawn = GetPawnChecked<APawn>();
	if (Pawn->GetLocalRole() != ROLE_Authority)
	{
		return;
	}
	if (PawnData)
	{
		return;
	}
	PawnData = InPawnData;
	CheckDefaultInitialization();
}

UAbilitySystemComponent* ULyraPawnExtensionComponent::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
