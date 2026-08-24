// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/LyraAbilitySystemComponent.h"
#include "Abilities/LyraGlobalAbilitySystem.h"
#include "Abilities/LyraGameplayAbility.h"
#include "Animation/LyraAnimInstance.h"

ULyraAbilitySystemComponent::ULyraAbilitySystemComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{

}

void ULyraAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
		{
			InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
			InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
		}
	}
}

void ULyraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
		{
			InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
			InputHeldSpecHandles.Remove(AbilitySpec.Handle);
		}
	}
}

void ULyraAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{

}

void ULyraAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}

void ULyraAbilitySystemComponent::GetAbilityTargetData(const FGameplayAbilitySpecHandle AbilityHandle, FGameplayAbilityActivationInfo ActivationInfo, FGameplayAbilityTargetDataHandle& OutTargetDataHandle)
{
	TSharedPtr<FAbilityReplicatedDataCache> ReplicatedData = AbilityTargetDataMap.Find(FGameplayAbilitySpecHandleAndPredictionKey(AbilityHandle, ActivationInfo.GetActivationPredictionKey()));
	if (ReplicatedData.IsValid())
	{
		OutTargetDataHandle = ReplicatedData->TargetData;
	}
}

void ULyraAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (ULyraGlobalAbilitySystem* GlobalAbilitySystem = UWorld::GetSubsystem<ULyraGlobalAbilitySystem>(GetWorld()))
	{
		GlobalAbilitySystem->UnregisterASC(this);
	}
	Super::EndPlay(EndPlayReason);
}

void ULyraAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	FGameplayAbilityActorInfo* ActorInfo = AbilityActorInfo.Get();
	const bool bHasNewPawnAvatar = Cast<APawn>(InAvatarActor) && (InAvatarActor != ActorInfo->AvatarActor);
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
	if (!bHasNewPawnAvatar)
	{
		return;
	}
	// Register with the global system once we actually have a pawn avatar. We wait until this time since some globally-applied effects may require an avatar.
	if (ULyraGlobalAbilitySystem* GlobalAbilitySystem = UWorld::GetSubsystem<ULyraGlobalAbilitySystem>(GetWorld()))
	{
		GlobalAbilitySystem->RegisterASC(this);
	}
	if (ULyraAnimInstance* AnimInstance = Cast<ULyraAnimInstance>(ActorInfo->GetAnimInstance()))
	{
		AnimInstance->InitializeWithAbilitySystem(this);
	}
	for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
	{
		const ULyraGameplayAbility* AbilityCDO = CastChecked<ULyraGameplayAbility>(AbilitySpec.Ability);
		AbilityCDO->TryActivateAbilityOnSpawn(AbilityActorInfo.Get(), AbilitySpec);
	}
}
