// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/LyraGameplayAbility.h"
#include "Abilities/LyraAbilityCost.h"
#include "Abilities/LyraAbilitySystemComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"

ULyraGameplayAbility::ULyraGameplayAbility(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	ActivationPolicy = ELyraAbilityActivationPolicy::OnInputTriggered;
}

void ULyraGameplayAbility::TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const
{
	const bool bIsPredicting = (Spec.ActivationInfo.ActivationMode == EGameplayAbilityActivationMode::Predicting);
	// Try to activate if activation policy is on spawn.
	if (!ActorInfo || Spec.IsActive() || bIsPredicting || (ActivationPolicy != ELyraAbilityActivationPolicy::OnSpawn))
	{
		return;
	}
	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	const AActor* AvatarActor = ActorInfo->AvatarActor.Get();
	// If avatar actor is torn off or about to die, don't try to activate until we get the new one.
	if (!ASC || !AvatarActor || AvatarActor->GetTearOff() || (AvatarActor->GetLifeSpan() > 0.0f))
	{
		return;
	}
	const bool bIsLocalExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalPredicted) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::LocalOnly);
	const bool bIsServerExecution = (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerOnly) || (NetExecutionPolicy == EGameplayAbilityNetExecutionPolicy::ServerInitiated);

	const bool bClientShouldActivate = ActorInfo->IsLocallyControlled() && bIsLocalExecution;
	const bool bServerShouldActivate = ActorInfo->IsNetAuthority() && bIsServerExecution;

	if (bClientShouldActivate || bServerShouldActivate)
	{
		ASC->TryActivateAbility(Spec.Handle);
	}
}

void ULyraGameplayAbility::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);
	K2_OnAbilityAdded();
	TryActivateAbilityOnSpawn(ActorInfo, Spec);
}

void ULyraGameplayAbility::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	K2_OnAbilityRemoved();
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void ULyraGameplayAbility::OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	K2_OnAvatarSet();
	Super::OnAvatarSet(ActorInfo, Spec);
}

bool ULyraGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return false;
	}
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	return true;
}

void ULyraGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

bool ULyraGameplayAbility::CanBeCanceled() const
{
	return Super::CanBeCanceled();
}

void ULyraGameplayAbility::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

void ULyraGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool ULyraGameplayAbility::CommitAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, OUT FGameplayTagContainer* OptionalRelevantTags)
{
	return Super::CommitAbility(Handle, ActorInfo, ActivationInfo, OptionalRelevantTags);
}

bool ULyraGameplayAbility::CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CheckCooldown(Handle, ActorInfo, OptionalRelevantTags);
}

void ULyraGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCooldown(Handle, ActorInfo, ActivationInfo);
}

bool ULyraGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags) || !ActorInfo)
	{
		return false;
	}
	// Verify we can afford any additional costs
	for (TObjectPtr<ULyraAbilityCost> AdditionalCost : AdditionalCosts)
	{
		if (AdditionalCost != nullptr && !AdditionalCost->CheckCost(this, Handle, ActorInfo, /*inout*/ OptionalRelevantTags))
		{
			return false;
		}
	}
	return false;
}

void ULyraGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
	// Used to determine if the ability actually hit a target (as some costs are only spent on successful attempts)
	auto DetermineIfAbilityHitTarget = [&]()
	{
		if (!ActorInfo->IsNetAuthority())
		{
			return false;
		}
		ULyraAbilitySystemComponent* ASC = Cast<ULyraAbilitySystemComponent>(ActorInfo->AbilitySystemComponent.Get());
		if (!ASC)
		{
			return false;
		}
		FGameplayAbilityTargetDataHandle TargetData;
		ASC->GetAbilityTargetData(Handle, ActivationInfo, TargetData);
		for (int32 TargetDataIdx = 0; TargetDataIdx < TargetData.Data.Num(); ++TargetDataIdx)
		{
			if (UAbilitySystemBlueprintLibrary::TargetDataHasHitResult(TargetData, TargetDataIdx))
			{
				return true;
			}
		}
		return false;
	};
	// Pay any additional costs
	bool bAbilityHitTarget = false;
	bool bHasDeterminedIfAbilityHitTarget = false;
	for (TObjectPtr<ULyraAbilityCost> AdditionalCost : AdditionalCosts)
	{
		if (!AdditionalCost)
		{
			continue;
		}
		if (AdditionalCost->ShouldOnlyApplyCostOnHit())
		{
			if (!bHasDeterminedIfAbilityHitTarget)
			{
				bAbilityHitTarget = DetermineIfAbilityHitTarget();
				bHasDeterminedIfAbilityHitTarget = true;
			}
			if (!bAbilityHitTarget)
			{
				continue;
			}
		}
		AdditionalCost->ApplyCost(this, Handle, ActorInfo, ActivationInfo);
	}
}
