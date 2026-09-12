// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameplayTagContainer.h"
#include "LyraAbilityCost.generated.h"

class ULyraGameplayAbility;

struct FGameplayAbilitySpecHandle;
struct FGameplayAbilityActorInfo;
struct FGameplayAbilityActivationInfo;

/**
 * Base class for costs that a LyraGameplayAbility has (e.g., ammo or charges)
 */
UCLASS()
class LYRA_API ULyraAbilityCost : public UObject
{
	GENERATED_BODY()
	
public:
	ULyraAbilityCost(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** Checks if we can afford this cost */
	virtual bool CheckCost(const ULyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const;

	/** Applies the ability's cost to the target */
	virtual void ApplyCost(const ULyraGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo);

	/** If true, this cost should only be applied if this ability hits successfully */
	bool ShouldOnlyApplyCostOnHit() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Costs)
	bool bOnlyApplyCostOnHit = false;
};
