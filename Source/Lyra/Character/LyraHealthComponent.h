// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/GameFrameworkComponent.h"
#include "LyraHealthComponent.generated.h"

class ULyraAbilitySystemComponent;
class ULyraAttributeSet_Health;
class ULyraHealthComponent;

struct FOnAttributeChangeData;
struct FGameplayEffectSpec;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLyraHealth_DeathEvent, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FLyraHealth_AttributeChanged, ULyraHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);

/**
 * Defines current state of death.
 */
UENUM(BlueprintType)
enum class ELyraDeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};

/**
 * An actor component used to handle anything related to health.
 */
UCLASS()
class LYRA_API ULyraHealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()
	
public:
	// Delegate fired when the health value has changed.
	UPROPERTY(BlueprintAssignable)
	FLyraHealth_AttributeChanged OnHealthChanged;

	// Delegate fired when the max health value has changed.
	UPROPERTY(BlueprintAssignable)
	FLyraHealth_AttributeChanged OnMaxHealthChanged;

	// Delegate fired when the death sequence has started.
	UPROPERTY(BlueprintAssignable)
	FLyraHealth_DeathEvent OnDeathStarted;

	// Delegate fired when the death sequence has finished.
	UPROPERTY(BlueprintAssignable)
	FLyraHealth_DeathEvent OnDeathFinished;

public:
	ULyraHealthComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Initialize the component using an ability system component.
	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	void InitializeWithAbilitySystem(ULyraAbilitySystemComponent* InASC);

	// Uninitialize the component, clearing any references to the ability system.
	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	void UninitializeFromAbilitySystem();

	// Returns the current health value.
	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	float GetHealth() const;

	// Returns the current maximum health value.
	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	float GetMaxHealth() const;

	// Returns the current health in the range [0.0, 1.0].
	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	float GetHealthNormalized() const;

	UFUNCTION(BlueprintCallable, Category = "Lyra|Health")
	ELyraDeathState GetDeathState() const { return DeathState; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Lyra|Health", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadOrDying() const { return (DeathState > ELyraDeathState::NotDead); }

	// Begins the death sequence for the owner.
	virtual void StartDeath();

	// Ends the death sequence for the owner.
	virtual void FinishDeath();

	// Applies enough damage to kill the owner.
	virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);

protected:
	virtual void HandleHealthChanged(const FOnAttributeChangeData& ChangeData);

	virtual void HandleMaxHealthChanged(const FOnAttributeChangeData& ChangeData);

	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec& DamageEffectSpec, float DamageMagnitude);

private:
	// Ability system used by this component.
	UPROPERTY()
	TObjectPtr<ULyraAbilitySystemComponent> AbilitySystemComponent;

	// Health set used by this component.
	UPROPERTY()
	TObjectPtr<ULyraAttributeSet_Health> HealthSet;

	// Replicated state used to handle dying.
	UPROPERTY()
	ELyraDeathState DeathState;

private:
	AActor* GetInstigatorFromAttributeChangeData(const FOnAttributeChangeData& ChangeData);

	void ClearGameplayTags();
};
