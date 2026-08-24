// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "LyraGlobalAbilitySystem.generated.h"

class ULyraAbilitySystemComponent;
class UGameplayAbility;
class UGameplayEffect;

struct FGameplayAbilitySpecHandle;
struct FActiveGameplayEffectHandle;

USTRUCT()
struct FGlobalAppliedAbilities
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TMap<TObjectPtr<ULyraAbilitySystemComponent>, FGameplayAbilitySpecHandle> Handles;

public:
	void AddAbilityToASC(TSubclassOf<UGameplayAbility> Ability, ULyraAbilitySystemComponent* ASC);

	void RemoveAbilityFromASC(ULyraAbilitySystemComponent* ASC);

	void RemoveAbilityFromAll();
};

USTRUCT()
struct FGlobalAppliedEffects
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TMap<TObjectPtr<ULyraAbilitySystemComponent>, FActiveGameplayEffectHandle> Handles;

public:
	void AddEffectToASC(TSubclassOf<UGameplayEffect> Effect, ULyraAbilitySystemComponent* ASC);

	void RemoveEffectFromASC(ULyraAbilitySystemComponent* ASC);

	void RemoveEffectFromAll();
};
/**
 * 
 */
UCLASS()
class LYRA_API ULyraGlobalAbilitySystem : public UWorldSubsystem
{
	GENERATED_BODY()
	
public:
	ULyraGlobalAbilitySystem();

	/** Register an ASC with global system and apply any active global effects/abilities. */
	void RegisterASC(ULyraAbilitySystemComponent* ASC);

	/** Removes an ASC from the global system, along with any active global effects/abilities. */
	void UnregisterASC(ULyraAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Lyra|AbilitySystem")
	void ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Lyra|AbilitySystem")
	void ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Lyra|AbilitySystem")
	void RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "Lyra|AbilitySystem")
	void RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect);

private:
	UPROPERTY()
	TArray<TObjectPtr<ULyraAbilitySystemComponent>> RegisteredASCs;

	UPROPERTY()
	TMap<TSubclassOf<UGameplayAbility>, FGlobalAppliedAbilities> AppliedAbilities;

	UPROPERTY()
	TMap<TSubclassOf<UGameplayEffect>, FGlobalAppliedEffects> AppliedEffects;

};
