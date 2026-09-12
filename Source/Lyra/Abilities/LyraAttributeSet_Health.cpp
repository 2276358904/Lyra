// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/LyraAttributeSet_Health.h"
#include "LyraGameplayTags.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "GameplayEffectExtension.h"


ULyraAttributeSet_Health::ULyraAttributeSet_Health(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer),
	Health(0.0f),
	MaxHealth(100.0f),
	bOutOfHealth(false)
{

}

bool ULyraAttributeSet_Health::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}
	// Handle modifying incoming normal damage
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		if (Data.EvaluatedData.Magnitude <= 0.0f)
		{
			return false;
		}
		const bool bIsDamageFromSelfDestruct = Data.EffectSpec.GetDynamicAssetTags().HasTagExact(LyraGameplayTags::Gameplay_DamageSelfDestruct);
		if (Data.Target.HasMatchingGameplayTag(LyraGameplayTags::Gameplay_DamageImmunity) && !bIsDamageFromSelfDestruct)
		{
			// Do not take away any health.
			Data.EvaluatedData.Magnitude = 0.0f;
			return false;
		}
	}
	return true;
}

void ULyraAttributeSet_Health::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float MinimumHealth = 0.0f;
	if (Data.EvaluatedData.Attribute == GetDamageAttribute())
	{
		// Convert into -Health and then clamp
		SetHealth(FMath::Clamp(GetHealth() - GetDamage(), MinimumHealth, GetMaxHealth()));
		SetDamage(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealingAttribute())
	{
		// Convert into +Health and then clamo
		SetHealth(FMath::Clamp(GetHealth() + GetHealing(), MinimumHealth, GetMaxHealth()));
		SetHealing(0.0f);
	}
	else if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Clamp and fall into out of health handling below
		SetHealth(FMath::Clamp(GetHealth(), MinimumHealth, GetMaxHealth()));
	}
	if ((GetHealth() <= 0.0f) && !bOutOfHealth && OnOutOfHealth.IsBound())
	{
		const FGameplayEffectContextHandle& EffectContext = Data.EffectSpec.GetEffectContext();
		AActor* Instigator = EffectContext.GetOriginalInstigator();
		AActor* Causer = EffectContext.GetEffectCauser();

		OnOutOfHealth.Broadcast(Instigator, Causer, Data.EffectSpec, Data.EvaluatedData.Magnitude);
	}
	// Check health again in case an event above changed it.
	bOutOfHealth = (GetHealth() <= 0.0f);
}
