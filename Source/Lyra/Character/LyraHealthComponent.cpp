// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/LyraHealthComponent.h"
#include "LyraGameplayTags.h"
#include "Abilities/LyraAbilitySystemComponent.h"
#include "Abilities/LyraAttributeSet_Health.h"
#include "System/LyraAssetManager.h"
#include "System/LyraGameData.h"

#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "GameplayEffectExtension.h"

ULyraHealthComponent::ULyraHealthComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	AbilitySystemComponent = nullptr;
	HealthSet = nullptr;
	DeathState = ELyraDeathState::NotDead;
}

void ULyraHealthComponent::InitializeWithAbilitySystem(ULyraAbilitySystemComponent* InASC)
{
	AActor* Owner = GetOwner();
	if (AbilitySystemComponent)
	{
		return;
	}
	AbilitySystemComponent = InASC;
	if (!AbilitySystemComponent)
	{
		return;
	}
	ClearGameplayTags();
	HealthSet = const_cast<ULyraAttributeSet_Health*>(AbilitySystemComponent->GetSet<ULyraAttributeSet_Health>());
	if (!HealthSet)
	{
		return;
	}
	// Register to listen for attribute changes.
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(ULyraAttributeSet_Health::GetHealthAttribute()).AddUObject(this, &ThisClass::HandleHealthChanged);
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(ULyraAttributeSet_Health::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::HandleMaxHealthChanged);
	HealthSet->OnOutOfHealth.AddUObject(this, &ThisClass::HandleOutOfHealth);
	// Reset attributes to default values.  Eventually this will be driven by a spread sheet.
	AbilitySystemComponent->SetNumericAttributeBase(ULyraAttributeSet_Health::GetHealthAttribute(), HealthSet->GetMaxHealth());
	OnHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
	OnMaxHealthChanged.Broadcast(this, HealthSet->GetHealth(), HealthSet->GetHealth(), nullptr);
}

void ULyraHealthComponent::UninitializeFromAbilitySystem()
{
	ClearGameplayTags();
	if (HealthSet)
	{
		HealthSet->OnOutOfHealth.RemoveAll(this);
	}
	HealthSet = nullptr;
	AbilitySystemComponent = nullptr;
}

float ULyraHealthComponent::GetHealth() const
{
	return (HealthSet ? HealthSet->GetHealth() : 0.0f);
}

float ULyraHealthComponent::GetMaxHealth() const
{
	return (HealthSet ? HealthSet->GetMaxHealth() : 0.0f);
}

float ULyraHealthComponent::GetHealthNormalized() const
{
	if (HealthSet)
	{
		const float Health = HealthSet->GetHealth();
		const float MaxHealth = HealthSet->GetMaxHealth();
		return ((MaxHealth > 0.0f) ? (Health / MaxHealth) : 0.0f);
	}
	return 0.0f;
}

void ULyraHealthComponent::StartDeath()
{
	if (DeathState != ELyraDeathState::NotDead)
	{
		return;
	}
	DeathState = ELyraDeathState::DeathStarted;
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(LyraGameplayTags::Gameplay_State_Death_Dying, 1);
	}
	AActor* Owner = GetOwner();
	OnDeathStarted.Broadcast(Owner);
}

void ULyraHealthComponent::FinishDeath()
{
	if (DeathState <= ELyraDeathState::DeathStarted)
	{
		return;
	}
	DeathState = ELyraDeathState::DeathFinished;
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(LyraGameplayTags::Gameplay_State_Death_Dead, 1);
	}
	AActor* Owner = GetOwner();
	OnDeathFinished.Broadcast(Owner);
}

void ULyraHealthComponent::DamageSelfDestruct(bool bFellOutOfWorld)
{
	if ((DeathState != ELyraDeathState::NotDead) || !AbilitySystemComponent)
	{
		return;
	}
	const TSubclassOf<UGameplayEffect> DamageGameplayEffect = ULyraAssetManager::GetSubclass(ULyraGameData::Get().DamageGameplayEffect_SetByCaller);
	if (!DamageGameplayEffect)
	{
		return;
	}
	FGameplayEffectSpecHandle GameplayEffectSpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DamageGameplayEffect, 1.0f, AbilitySystemComponent->MakeEffectContext());
	FGameplayEffectSpec* GameplayEffectSpec = GameplayEffectSpecHandle.Data.Get();
	if (!GameplayEffectSpec)
	{
		return;
	}
	GameplayEffectSpec->AddDynamicAssetTag(LyraGameplayTags::Gameplay_DamageSelfDestruct);
	if (bFellOutOfWorld)
	{
		GameplayEffectSpec->AddDynamicAssetTag(LyraGameplayTags::Gameplay_DamageFellOutOfWorld);
	}
	GameplayEffectSpec->SetSetByCallerMagnitude(LyraGameplayTags::Gameplay_SetByCaller_Damage, GetMaxHealth());
	AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*GameplayEffectSpec);
}

void ULyraHealthComponent::HandleHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	AActor* Instigator = GetInstigatorFromAttributeChangeData(ChangeData);
	OnHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, Instigator);
}

void ULyraHealthComponent::HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData)
{
	AActor* Instigator = GetInstigatorFromAttributeChangeData(ChangeData);
	OnHealthChanged.Broadcast(this, ChangeData.OldValue, ChangeData.NewValue, Instigator);
}

void ULyraHealthComponent::HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude)
{

}

AActor* ULyraHealthComponent::GetInstigatorFromAttributeChangeData(const FOnAttributeChangeData& ChangeData)
{
	if (ChangeData.GEModData != nullptr)
	{
		const FGameplayEffectContextHandle& EffectContext = ChangeData.GEModData->EffectSpec.GetEffectContext();
		return EffectContext.GetOriginalInstigator();
	}
	return nullptr;
}

void ULyraHealthComponent::ClearGameplayTags()
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->SetLooseGameplayTagCount(LyraGameplayTags::Gameplay_State_Death_Dead, 0);
		AbilitySystemComponent->SetLooseGameplayTagCount(LyraGameplayTags::Gameplay_State_Death_Dying, 0);
	}
}
