// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/LyraAnimInstance.h"

ULyraAnimInstance::ULyraAnimInstance(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{

}

void ULyraAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	GameplayTagPropertyMap.Initialize(this, ASC);
}
