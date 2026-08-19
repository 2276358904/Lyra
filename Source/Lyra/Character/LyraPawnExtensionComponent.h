// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
#include "LyraPawnExtensionComponent.generated.h"

/**
 * 
 */
UCLASS()
class LYRA_API ULyraPawnExtensionComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	/** The name of this overall feature, this one depends on the other named component features */
	static const FName NAME_ActorFeatureName;

public:
	ULyraPawnExtensionComponent(const FObjectInitializer& ObjectInitializer);


};
