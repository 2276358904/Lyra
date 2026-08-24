// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LyraPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class LYRA_API ALyraPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ALyraPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Lyra|Character")
	void SetIsAutoRunning(const bool bEnabled);

	UFUNCTION(BlueprintCallable, Category = "Lyra|Character")
	bool GetIsAutoRunning() const;

private:
	void OnStartAutoRun();

	void OnEndAutoRun();
};
