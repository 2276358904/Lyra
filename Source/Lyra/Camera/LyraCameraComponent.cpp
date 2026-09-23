// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/LyraCameraComponent.h"
#include "Camera/LyraCameraMode.h"
#include "Camera/LyraCameraModeStack.h"

ULyraCameraComponent::ULyraCameraComponent(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	CameraModeStack = nullptr;
}

void ULyraCameraComponent::OnRegister()
{
	Super::OnRegister();
	if (!CameraModeStack)
	{
		CameraModeStack = NewObject<ULyraCameraModeStack>(this);
	}
}

void ULyraCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	Super::GetCameraView(DeltaTime, DesiredView);

	UpdateCameraModeStack();

	FLyraCameraModeView CameraModeView;
	CameraModeStack->EvaluateStack(DeltaTime, CameraModeView);

	DesiredView.Location = CameraModeView.Location;
	DesiredView.Rotation = CameraModeView.Rotation;
	DesiredView.FOV = CameraModeView.FieldOfView;
}

void ULyraCameraComponent::UpdateCameraModeStack()
{
	if (!CameraModeStack->IsStackActivate() || !DetermineCameraModeDelegate.IsBound())
	{
		return;
	}
	TSubclassOf<ULyraCameraMode> CameraModeClass = DetermineCameraModeDelegate.Execute();
	CameraModeStack->PushCameraMode(CameraModeClass);
}
