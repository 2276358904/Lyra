// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "LyraCameraComponent.generated.h"

class ULyraCameraMode;
class ULyraCameraModeStack;

DECLARE_DELEGATE_RetVal(TSubclassOf<ULyraCameraMode>, FLyraCameraModeDelegate);

/**
 * The base camera component class used by this project.
 */
UCLASS()
class LYRA_API ULyraCameraComponent : public UCameraComponent
{
	GENERATED_BODY()
	
public:
	// Delegate used to query for the best camera mode.
	FLyraCameraModeDelegate DetermineCameraModeDelegate;

public:
	ULyraCameraComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UCameraComponent interface
	virtual void OnRegister() override;

	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;
	//~End UCameraComponent interface

private:
	// Stack used to blend the camera modes.
	UPROPERTY()
	TObjectPtr<ULyraCameraModeStack> CameraModeStack;

private:
	void UpdateCameraModeStack();
};
