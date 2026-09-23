// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/LyraCameraMode_ThirdPerson.h"

#include "Curves/CurveVector.h"

ULyraCameraMode_ThirdPerson::ULyraCameraMode_ThirdPerson(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer)
{
	TargetOffsetCurve = nullptr;
}

void ULyraCameraMode_ThirdPerson::UpdateView(float DeltaTime)
{
	Super::UpdateView(DeltaTime);

	// Apply third person offset using pitch.
	if (!bUseRuntimeFloatCurves && TargetOffsetCurve)
	{
		const FVector TargetOffset = TargetOffsetCurve->GetVectorValue(View.Rotation.Pitch);
		View.Location += View.Rotation.RotateVector(TargetOffset);
	}
	else
	{
		FVector TargetOffset;
		TargetOffset.X = TargetOffsetX.GetRichCurveConst()->Eval(View.Rotation.Pitch);
		TargetOffset.Y = TargetOffsetY.GetRichCurveConst()->Eval(View.Rotation.Pitch);
		TargetOffset.Z = TargetOffsetZ.GetRichCurveConst()->Eval(View.Rotation.Pitch);
		View.Location += View.Rotation.RotateVector(TargetOffset);
	}
}

