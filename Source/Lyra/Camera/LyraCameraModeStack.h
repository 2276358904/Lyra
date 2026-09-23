// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LyraCameraModeStack.generated.h"

class ULyraCameraMode;

struct FLyraCameraModeView;

/**
 * Stack used for blending camera modes.
 */
UCLASS()
class LYRA_API ULyraCameraModeStack : public UObject
{
	GENERATED_BODY()
	
public:
	ULyraCameraModeStack(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Activates the camera mode stack, enabling it to begin evaluating and blending modes each frame.
	void ActivateStack();

	// Deactivates the camera mode stack, stopping all mode evaluation and blend updates.
	void DeactivateStack();

	// Returns whether the camera mode stack is currently active and processing updates.
	bool IsStackActivate() const { return bIsActive; }

	// Evaluates the active camera mode stack for this frame, outputting the final blended view parameters.
	bool EvaluateStack(float DeltaTime, FLyraCameraModeView& OutCameraModeView);

	// Pushes a new camera mode class onto the stack, creating or reusing an instance as needed.
	void PushCameraMode(TSubclassOf<ULyraCameraMode> CameraModeClass);

protected:
	// Retrieves or creates a cached instance for the given camera mode class from the instance pool.
	ULyraCameraMode* GetCameraModeInstance(TSubclassOf<ULyraCameraMode> CameraModeClass);

private:
	// Tracks whether the camera mode stack is currently active.
	bool bIsActive;

	UPROPERTY()
	TArray<TObjectPtr<ULyraCameraMode>> CameraModeStack;

	UPROPERTY()
	TArray<TObjectPtr<ULyraCameraMode>> CameraModeInstances;

private:
	// Performs per-frame updates on all active modes in the stack, handling activation/deactivation and blend timing.
	void UpdateStack(float DeltaTime);

	// Blends all active camera modes in the stack by priority weight, outputting the final composite view.
	void BlendStack(FLyraCameraModeView& OutCameraModeView) const;
};
