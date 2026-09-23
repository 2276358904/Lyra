// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/LyraCameraMode.h"
#include "Camera/LyraCameraComponent.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

FLyraCameraModeView::FLyraCameraModeView():
	Location(ForceInit), 
	Rotation(ForceInit),
	FieldOfView(80.0f)
{

}

void FLyraCameraModeView::Blend(const FLyraCameraModeView& Other, float OtherWeight)
{
	Location = FMath::Lerp(Location, Other.Location, OtherWeight);
	const FRotator DeltaRotation = (Other.Rotation - Rotation).GetNormalized();
	Rotation = Rotation + (OtherWeight * DeltaRotation);
	FieldOfView = FMath::Lerp(FieldOfView, Other.FieldOfView, OtherWeight);
}

ULyraCameraMode::ULyraCameraMode(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	FieldOfView = 80.0f;
	ViewPitchMin = -89.0f;
	ViewPitchMax = 89.0f;
	BlendTime = 0.5f;
	BlendFunction = ELyraCameraModeBlendFunction::Linear;
	BlendExponent = 4.0f;
	BlendAlpha = 1.0f;
	BlendWeight = 1.0f;
	CrouchOffsetBlendMultiplier = 5.0f;
	CrouchOffsetBlendWeight = 1.0f;
	InitialCrouchOffset = FVector::ZeroVector;
	TargetCrouchOffset = FVector::ZeroVector;
	CurrentCrouchOffset = FVector::ZeroVector;
}

void ULyraCameraMode::UpdateCameraMode(float DeltaTime)
{
	UpdateView(DeltaTime);
	UpdateCrouchOffset(DeltaTime);
}

const FLyraCameraModeView& ULyraCameraMode::GetCameraModeView() const
{
	return View;
}

float ULyraCameraMode::GetBlendTime() const
{
	return BlendTime;
}

float ULyraCameraMode::GetBlendWeight() const
{
	return BlendWeight;
}

void ULyraCameraMode::SetBlendWeight(float Weight)
{
	BlendWeight = Weight;
	switch (BlendFunction)
	{
	case ELyraCameraModeBlendFunction::Linear:
		BlendAlpha = BlendWeight;
		break;
	case ELyraCameraModeBlendFunction::EaseIn:
		BlendAlpha = FMath::InterpEaseIn(0.0f, 1.0f, BlendWeight, BlendExponent);
		break;
	case ELyraCameraModeBlendFunction::EaseOut:
		BlendAlpha = FMath::InterpEaseOut(0.0f, 1.0f, BlendWeight, BlendExponent);
		break;
	case ELyraCameraModeBlendFunction::EaseInOut:
		BlendAlpha = FMath::InterpEaseInOut(0.0f, 1.0f, BlendWeight, BlendExponent);
		break;
	default:
		break;
	}
}

void ULyraCameraMode::UpdateView(float DeltaTime)
{
	View.Location = GetViewLocation() + CurrentCrouchOffset;
	View.Rotation = GetViewRotation();
	View.Rotation.Pitch = FMath::ClampAngle(View.Rotation.Pitch, ViewPitchMin, ViewPitchMax);
	View.FieldOfView = FieldOfView;
}

void ULyraCameraMode::UpdateBlending(float DeltaTime)
{
	if (BlendTime > 0)
	{
		BlendAlpha += DeltaTime / BlendTime;
		BlendAlpha = FMath::Clamp(BlendAlpha, 0.0f, 1.0f);
	}
	else
	{
		BlendAlpha = 1.0f;
	}
	switch (BlendFunction)
	{
	case ELyraCameraModeBlendFunction::Linear:
		BlendWeight = BlendAlpha;
		break;
	case ELyraCameraModeBlendFunction::EaseIn:
		BlendWeight = FMath::InterpEaseIn(0.0f, 1.0f, BlendAlpha, BlendExponent);
		break;
	case ELyraCameraModeBlendFunction::EaseOut:
		BlendWeight = FMath::InterpEaseOut(0.0f, 1.0f, BlendAlpha, BlendExponent);
		break;
	case ELyraCameraModeBlendFunction::EaseInOut:
		BlendWeight = FMath::InterpEaseInOut(0.0f, 1.0f, BlendAlpha, BlendExponent);
		break;
	default:
		break;
	}
}

void ULyraCameraMode::UpdateCrouchOffset(float DeltaTime)
{
	ACharacter* Character = Cast<ACharacter>(GetViewActor());
	ACharacter* CharacterCDO = Character->StaticClass()->GetDefaultObject<ACharacter>();
	TargetCrouchOffset = FVector(0.0f, 0.0f, CharacterCDO->CrouchedEyeHeight - CharacterCDO->BaseEyeHeight);
	if (Character->bIsCrouched)
	{
		CrouchOffsetBlendWeight = FMath::Min(CrouchOffsetBlendWeight + DeltaTime * CrouchOffsetBlendMultiplier, 1.0f);
	}
	else
	{
		CrouchOffsetBlendWeight = FMath::Max(CrouchOffsetBlendWeight - DeltaTime * CrouchOffsetBlendMultiplier, 0.0f);
	}
	CurrentCrouchOffset = FMath::InterpEaseInOut(InitialCrouchOffset, TargetCrouchOffset, CrouchOffsetBlendWeight, 1.0f);
}

FVector ULyraCameraMode::GetViewLocation() const
{
	AActor* Actor = GetViewActor();
	if (ACharacter* Character = Cast<ACharacter>(Actor))
	{
		const UCapsuleComponent* CapsuleComponent = Character->GetCapsuleComponent();
		// Height adjustments for characters to account for crouching.
		const ACharacter* CharacterCDO = Character->GetClass()->GetDefaultObject<ACharacter>();
		const UCapsuleComponent* CapsuleComponentCDO = CharacterCDO->GetCapsuleComponent();
		float HeightOffset = CapsuleComponentCDO->GetScaledCapsuleHalfHeight() - CapsuleComponent->GetScaledCapsuleHalfHeight();
		return Character->GetActorLocation() + FVector(0.0f, 0.0f, CharacterCDO->BaseEyeHeight + HeightOffset);
	}
	else if (APawn* Pawn = Cast<APawn>(Actor))
	{
		return Pawn->GetPawnViewLocation();
	}
	else
	{
		return Actor->GetActorLocation();
	}
}

FRotator ULyraCameraMode::GetViewRotation() const
{
	AActor* Actor = GetViewActor();
	if (APawn* Pawn = Cast<APawn>(Actor))
	{
		return Pawn->GetViewRotation();
	}
	else
	{
		return Actor->GetActorRotation();
	}
}

AActor* ULyraCameraMode::GetViewActor() const
{
	ULyraCameraComponent* CameraComponent = Cast<ULyraCameraComponent>(GetOuter());
	return CameraComponent->GetOwner();
}
