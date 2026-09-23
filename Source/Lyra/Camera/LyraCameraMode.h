// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "LyraCameraMode.generated.h"

/**
 * Blend function used for transitioning between camera modes.
 */
UENUM(BlueprintType)
enum class ELyraCameraModeBlendFunction : uint8
{
	// Does a simple linear interpolation.
	Linear,

	// Immediately accelerates, but smoothly decelerates into the target.  Ease amount controlled by the exponent.
	EaseIn,

	// Smoothly accelerates, but does not decelerate into the target.  Ease amount controlled by the exponent.
	EaseOut,

	// Smoothly accelerates and decelerates.  Ease amount controlled by the exponent.
	EaseInOut,

	COUNT	UMETA(Hidden)
};

/**
 * View data produced by the camera mode that is used to blend camera modes.
 */
struct FLyraCameraModeView
{
public:
	FVector Location;

	FRotator Rotation;

	float FieldOfView;

public:
	FLyraCameraModeView();

	void Blend(const FLyraCameraModeView& Other, float OtherWeight);
};

/**
 * Base class for all camera modes.
 */
UCLASS(Abstract, Blueprintable)
class LYRA_API ULyraCameraMode : public UObject
{
	GENERATED_BODY()
	
public:
	ULyraCameraMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Updates the active camera mode for this frame, handling mode transitions and blend state.
	void UpdateCameraMode(float DeltaTime);

	// Returns the current camera mode's view parameters (location, rotation, FOV, etc.) after blending.
	const FLyraCameraModeView& GetCameraModeView() const;

	// Returns the configured blend duration (in seconds) for transitioning between camera modes.
	float GetBlendTime() const;

	// Returns the current interpolation weight for the camera mode blend (0.0 = previous mode, 1.0 = target mode).
	float GetBlendWeight() const;

	// Sets the current interpolation weight for the camera mode blend, clamped to [0.0, 1.0].
	void SetBlendWeight(float Weight);

	// Called when this camera mode is activated on the camera mode stack.
	virtual void OnActivation() {};

	// Called when this camera mode is deactivated on the camera mode stack.
	virtual void OnDeactivation() {};

protected:
	// View output produced by the camera mode.
	FLyraCameraModeView View;

	// The horizontal field of view (in degrees).
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "5.0", UIMax = "170", ClampMin = "5.0", ClampMax = "170.0"))
	float FieldOfView;

	// Minimum view pitch (in degrees).
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMin;

	// Maximum view pitch (in degrees).
	UPROPERTY(EditDefaultsOnly, Category = "View", Meta = (UIMin = "-89.9", UIMax = "89.9", ClampMin = "-89.9", ClampMax = "89.9"))
	float ViewPitchMax;

	// How long it takes to blend in this mode.
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendTime;

	// Function used for blending.
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	ELyraCameraModeBlendFunction BlendFunction;

	// Exponent used by blend functions to control the shape of the curve.
	UPROPERTY(EditDefaultsOnly, Category = "Blending")
	float BlendExponent;

	// Linear blend alpha used to determine the blend weight.
	float BlendAlpha;

	// Blend weight calculated using the blend alpha and function.
	float BlendWeight;

	// Alters the speed that a crouch offset is blended in or out.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crouch")
	float CrouchOffsetBlendMultiplier;

	// Current interpolation weight for the crouch offset blend (0.0 = standing, 1.0 = fully crouched).
	float CrouchOffsetBlendWeight;

	// The crouch offset value stored when the character begins crouching, used as the blend start point.
	FVector InitialCrouchOffset;

	// The target crouch offset value to blend towards when the crouch state changes.
	FVector TargetCrouchOffset;

	// The final blended crouch offset applied to the character each frame.
	FVector CurrentCrouchOffset;

protected:
	// Updates the camera view for this frame, delegating to blending and crouch offset logic.
	virtual void UpdateView(float DeltaTime);

	// Updates the interpolation weight used to smoothly blend between different camera modes (e.g., standing, crouched, aiming).
	void UpdateBlending(float DeltaTime);

	// Computes the current crouch offset by interpolating between InitialCrouchOffset and TargetCrouchOffset based on BlendWeight.
	void UpdateCrouchOffset(float DeltaTime);

	// Returns the final camera world position after applying the blended crouch offset to the view actor's location.
	virtual FVector GetViewLocation() const;

	// Returns the camera view rotation, forwarded directly from the view actor's control rotation.
	virtual FRotator GetViewRotation() const;

private:
	// Resolves the actor whose transform serves as the base for the camera view (typically the possessed Pawn/Character).
	AActor* GetViewActor() const;
};
