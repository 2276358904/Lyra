// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "TestAbilitySystem.generated.h"

class UAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class LYRA_API UTestAbilitySystem : public UObject
{
	GENERATED_BODY()
	
public:
	UTestAbilitySystem();

	void TestAbilitySystemComponent_01();
	void TestAbilitySystemComponent_02();
	void TestAbilitySystemComponent_03();
	void TestAbilitySystemComponent_04();
	void TestAbilitySystemComponent_05();
private:
	UAbilitySystemComponent* AbilitySystemComponent;
};
