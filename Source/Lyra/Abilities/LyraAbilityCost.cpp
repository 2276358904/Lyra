// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/LyraAbilityCost.h"

ULyraAbilityCost::ULyraAbilityCost(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{

}

bool ULyraAbilityCost::CheckCost(const ULyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	return false;
}

void ULyraAbilityCost::ApplyCost(const ULyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{

}

bool ULyraAbilityCost::ShouldOnlyApplyCostOnHit() const
{
	return bOnlyApplyCostOnHit;
}
