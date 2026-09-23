// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/LyraCameraModeStack.h"
#include "Camera/LyraCameraMode.h"

ULyraCameraModeStack::ULyraCameraModeStack(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	bIsActive = true;
}

void ULyraCameraModeStack::ActivateStack()
{
	if (bIsActive)
	{
		return;
	}
	bIsActive = true;
	// Notify camera modes that they are being activated.
	for (ULyraCameraMode* CameraMode : CameraModeStack)
	{
		CameraMode->OnActivation();
	}
}

void ULyraCameraModeStack::DeactivateStack()
{
	if (!bIsActive)
	{
		return;
	}
	bIsActive = false;
	// Notify camera modes that they are being activated.
	for (ULyraCameraMode* CameraMode : CameraModeStack)
	{
		CameraMode->OnDeactivation();
	}
}

bool ULyraCameraModeStack::EvaluateStack(float DeltaTime, FLyraCameraModeView& OutCameraModeView)
{
	if (!bIsActive)
	{
		return false;
	}
	UpdateStack(DeltaTime);
	BlendStack(OutCameraModeView);
	return true;
}

void ULyraCameraModeStack::PushCameraMode(TSubclassOf<ULyraCameraMode> CameraModeClass)
{
	if (!CameraModeClass)
	{
		return;
	}
	ULyraCameraMode* CameraMode = GetCameraModeInstance(CameraModeClass);
	int32 StackSize = CameraModeStack.Num();
	if ((StackSize > 0) && (CameraModeStack[0] == CameraMode))
	{
		// Already top of stack.
		return;
	}
	// See if it's already in the stack and remove it.
	// Figure out how much it was contributing to the stack.
	int32 ExistingStackIndex = INDEX_NONE;
	float ExistingStackContribution = 1.0f;

	CameraMode->SetBlendWeight(0.0f);
	// Add new entry to top of stack.
	CameraModeStack.Insert(CameraMode, 0);
	// Make sure stack bottom is always weighted 100%.
	CameraModeStack.Last()->SetBlendWeight(1.0f);
}

ULyraCameraMode* ULyraCameraModeStack::GetCameraModeInstance(TSubclassOf<ULyraCameraMode> CameraModeClass)
{
	// First see if we already created one.
	for (ULyraCameraMode* CameraMode : CameraModeInstances)
	{
		if ((CameraMode != nullptr) && (CameraMode->GetClass() == CameraModeClass))
		{
			return CameraMode;
		}
	}
	// Not found, so we need to create it.
	ULyraCameraMode* NewCameraMode = NewObject<ULyraCameraMode>(GetOuter(), CameraModeClass, NAME_None, RF_NoFlags);
	CameraModeInstances.Add(NewCameraMode);
	return NewCameraMode;
}

void ULyraCameraModeStack::UpdateStack(float DeltaTime)
{
	if (CameraModeStack.Num() < 0)
	{
		return;
	}
	int RemoveIndex = INDEX_NONE;
	int RemoveCount = 0;
	for (int32 StackIndex = 0; StackIndex < CameraModeStack.Num(); ++StackIndex)
	{
		ULyraCameraMode* CameraMode = CameraModeStack[StackIndex];
		CameraMode->UpdateCameraMode(DeltaTime);
		// Everything below this mode is now irrelevant and can be removed.
		if (CameraMode->GetBlendWeight() >= 1.0f)
		{
			RemoveIndex = StackIndex + 1;
			RemoveCount = CameraModeStack.Num() - RemoveIndex;
			break;
		}
	}
	if (RemoveCount <= 0)
	{
		return;
	}
	// Let the camera modes know they being removed from the stack.
	for (int32 StackIndex = RemoveIndex; StackIndex < CameraModeStack.Num(); ++StackIndex)
	{
		ULyraCameraMode* CameraMode = CameraModeStack[StackIndex];
		CameraMode->OnDeactivation();
	}
	CameraModeStack.RemoveAt(RemoveIndex, RemoveCount);
}

void ULyraCameraModeStack::BlendStack(FLyraCameraModeView& OutCameraModeView) const
{
	if (CameraModeStack.Num() <= 0)
	{
		return;
	}
	// Start at the bottom and blend up the stack
	ULyraCameraMode* CameraMode = CameraModeStack[CameraModeStack.Num() - 1];
	OutCameraModeView = CameraMode->GetCameraModeView();
	for (int32 StackIndex = CameraModeStack.Num() - 2; StackIndex >= 0; ++StackIndex)
	{
		CameraMode = CameraModeStack[StackIndex];
		OutCameraModeView.Blend(CameraMode->GetCameraModeView(), CameraMode->GetBlendWeight());
	}
}
