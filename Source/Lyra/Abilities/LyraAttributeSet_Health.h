// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/LyraAttributeSet.h"
#include "NativeGameplayTags.h"
#include "LyraAttributeSet_Health.generated.h"

struct FGameplayEffectSpec;

/**
 * Class that defines attributes that are necessary for taking damage.
 * Attribute examples include: health, shields, and resistances.
 */
UCLASS()
class LYRA_API ULyraAttributeSet_Health : public ULyraAttributeSet
{
	GENERATED_BODY()
	
public:
	// Delegate to broadcast when the health attribute reaches zero.
	FLyraAttributeEvent OnOutOfHealth;

public:
	ULyraAttributeSet_Health(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	ATTRIBUTE_ACCESSORS(ULyraAttributeSet_Health, Health);
	ATTRIBUTE_ACCESSORS(ULyraAttributeSet_Health, MaxHealth);
	ATTRIBUTE_ACCESSORS(ULyraAttributeSet_Health, Healing);
	ATTRIBUTE_ACCESSORS(ULyraAttributeSet_Health, Damage);

protected:
	//~UAttributeSet interface
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;

	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	//~End of UAttributeSet interface

private:
	// The current health attribute.  The health will be capped by the max health attribute.  Health is hidden from modifiers so only executions can modify it.
	UPROPERTY(BlueprintReadOnly, Category = "Lyra|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Health;

	// The current max health attribute.  Max health is an attribute since gameplay effects can modify it.
	UPROPERTY(BlueprintReadOnly, Category = "Lyra|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxHealth;

	// Used to track when the health reaches 0.
	bool bOutOfHealth;

	// Incoming healing. This is mapped directly to +Health
	UPROPERTY(BlueprintReadOnly, Category = "Lyra|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Healing;

	// Incoming damage. This is mapped directly to -Health
	UPROPERTY(BlueprintReadOnly, Category = "Lyra|Health", Meta = (HideFromModifiers, AllowPrivateAccess = true))
	FGameplayAttributeData Damage;
};
