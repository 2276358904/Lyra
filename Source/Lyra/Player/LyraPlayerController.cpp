// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LyraPlayerController.h"
#include "Player/LyraPlayerState.h"
#include "Abilities/LyraAbilitySystemComponent.h"

ALyraPlayerController::ALyraPlayerController(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	
}

void ALyraPlayerController::SetIsAutoRunning(const bool bEnabled)
{

}

bool ALyraPlayerController::GetIsAutoRunning() const
{
	return false;
}

void ALyraPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);
	ALyraPlayerState* LyraPlayerState = GetPlayerState<ALyraPlayerState>();
	if (!LyraPlayerState)
	{
		return;
	}
	ULyraAbilitySystemComponent* AbilitySystemComponent = Cast<ULyraAbilitySystemComponent>(LyraPlayerState->GetAbilitySystemComponent());
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->ProcessAbilityInput(DeltaTime, bGamePaused);
	}
}

void ALyraPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();
}

void ALyraPlayerController::OnStartAutoRun()
{

}

void ALyraPlayerController::OnEndAutoRun()
{

}
