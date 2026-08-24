// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/LyraInputComponent.h"
#include "Player/LyraLocalPlayer.h"

#include "EnhancedInputSubsystems.h"

ULyraInputComponent::ULyraInputComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{

}

void ULyraInputComponent::AddInputMappings(const ULyraInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	ULyraLocalPlayer* LocalPlayer = InputSubsystem->GetLocalPlayer<ULyraLocalPlayer>();

}

void ULyraInputComponent::RemoveInputMappings(const ULyraInputConfig* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
}

void ULyraInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
