// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/LyraGlobalAbilitySystem.h"
#include "Abilities/LyraAbilitySystemComponent.h"

#include "ActiveGameplayEffectHandle.h"
#include "GameplayAbilitySpecHandle.h"
#include "GameplayEffect.h"
#include "Abilities/GameplayAbility.h"

void FGlobalAppliedAbilities::AddAbilityToASC(TSubclassOf<UGameplayAbility> Ability, ULyraAbilitySystemComponent* ASC)
{
	if (FGameplayAbilitySpecHandle* SpecHandle = Handles.Find(ASC))
	{
		RemoveAbilityFromASC(ASC);
	}
	UGameplayAbility* AbilityCDO = Ability->GetDefaultObject<UGameplayAbility>();
	FGameplayAbilitySpec AbilitySpec(AbilityCDO);
	const FGameplayAbilitySpecHandle AbilitySpecHandle = ASC->GiveAbility(AbilitySpec);
	Handles.Add(ASC, AbilitySpecHandle);
}

void FGlobalAppliedAbilities::RemoveAbilityFromASC(ULyraAbilitySystemComponent* ASC)
{
	if (FGameplayAbilitySpecHandle* SpecHandle = Handles.Find(ASC))
	{
		ASC->ClearAbility(*SpecHandle);
		Handles.Remove(ASC);
	}
}

void FGlobalAppliedAbilities::RemoveAbilityFromAll()
{
	for (auto& KVP : Handles)
	{
		if (KVP.Key != nullptr)
		{
			KVP.Key->ClearAbility(KVP.Value);
		}
	}
	Handles.Empty();
}

void FGlobalAppliedEffects::AddEffectToASC(TSubclassOf<UGameplayEffect> Effect, ULyraAbilitySystemComponent* ASC)
{
	if (FActiveGameplayEffectHandle* EffectHandle = Handles.Find(ASC))
	{
		RemoveEffectFromASC(ASC);
	}
	const UGameplayEffect* GameplayEffectCDO = Effect->GetDefaultObject<UGameplayEffect>();
	const FActiveGameplayEffectHandle GameplayEffectHandle = ASC->ApplyGameplayEffectToSelf(GameplayEffectCDO, /*Level=*/ 1, ASC->MakeEffectContext());
	Handles.Add(ASC, GameplayEffectHandle);
}

void FGlobalAppliedEffects::RemoveEffectFromASC(ULyraAbilitySystemComponent* ASC)
{
	if (FActiveGameplayEffectHandle* EffectHandle = Handles.Find(ASC))
	{
		ASC->RemoveActiveGameplayEffect(*EffectHandle);
		Handles.Remove(ASC);
	}
}

void FGlobalAppliedEffects::RemoveEffectFromAll()
{
	for (auto& KVP : Handles)
	{
		if (KVP.Key != nullptr)
		{
			KVP.Key->RemoveActiveGameplayEffect(KVP.Value);
		}
	}
	Handles.Empty();
}


ULyraGlobalAbilitySystem::ULyraGlobalAbilitySystem()
{

}

void ULyraGlobalAbilitySystem::RegisterASC(ULyraAbilitySystemComponent* ASC)
{
	RegisteredASCs.AddUnique(ASC);
	for (auto& Entry : AppliedAbilities)
	{
		Entry.Value.AddAbilityToASC(Entry.Key, ASC);
	}
	for (auto& Entry : AppliedEffects)
	{
		Entry.Value.AddEffectToASC(Entry.Key, ASC);
	}
}

void ULyraGlobalAbilitySystem::UnregisterASC(ULyraAbilitySystemComponent* ASC)
{
	RegisteredASCs.Remove(ASC);
	for (auto& Entry : AppliedAbilities)
	{
		Entry.Value.RemoveAbilityFromASC(ASC);
	}
	for (auto& Entry : AppliedEffects)
	{
		Entry.Value.RemoveEffectFromASC(ASC);
	}
}

void ULyraGlobalAbilitySystem::ApplyAbilityToAll(TSubclassOf<UGameplayAbility> Ability)
{
	if (Ability.Get() == nullptr || AppliedAbilities.Contains(Ability))
	{
		return;
	}
	FGlobalAppliedAbilities& Entry = AppliedAbilities.Add(Ability);
	for (ULyraAbilitySystemComponent* ASC : RegisteredASCs)
	{
		Entry.AddAbilityToASC(Ability, ASC);
	}
}

void ULyraGlobalAbilitySystem::ApplyEffectToAll(TSubclassOf<UGameplayEffect> Effect)
{
	if (Effect.Get() == nullptr || AppliedEffects.Contains(Effect))
	{
		return;
	}
	FGlobalAppliedEffects& Entry = AppliedEffects.Add(Effect);
	for (ULyraAbilitySystemComponent* ASC : RegisteredASCs)
	{
		Entry.AddEffectToASC(Effect, ASC);
	}
}

void ULyraGlobalAbilitySystem::RemoveAbilityFromAll(TSubclassOf<UGameplayAbility> Ability)
{
	if (Ability.Get() == nullptr || !AppliedAbilities.Contains(Ability))
	{
		return;
	}
	FGlobalAppliedAbilities& Entry = AppliedAbilities[Ability];
	Entry.RemoveAbilityFromAll();
	AppliedAbilities.Remove(Ability);
}

void ULyraGlobalAbilitySystem::RemoveEffectFromAll(TSubclassOf<UGameplayEffect> Effect)
{
	if (Effect.Get() == nullptr || !AppliedEffects.Contains(Effect))
	{
		return;
	}
	FGlobalAppliedEffects& Entry = AppliedEffects[Effect];
	Entry.RemoveEffectFromAll();
	AppliedEffects.Remove(Effect);
}






