// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/Input/RPG_InputAccelerationModifier.h"

FInputActionValue URPG_InputAccelerationModifier::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	EInputActionValueType ValueType = CurrentValue.GetValueType();
	if (ValueType == EInputActionValueType::Boolean)
	{
		return CurrentValue;
	}

	if (CurrentValue.Get<FVector>() == FVector::ZeroVector || !AccelerationCurve)
	{
		Timer = 0.f;
		return CurrentValue;
	}

#if ENABLE_DRAW_DEBUG
	// Debugging
	if (GEngine)
	{
		bool bValidCurve = true;
		FVector2D TimeRange;
		AccelerationCurve->GetTimeRange(TimeRange.X, TimeRange.Y);

		if (TimeRange.X != 0.f || TimeRange.Y != 1.f)
		{
			GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("RPG_InputAccelerationModifier: Given Curve's time range (X Axis) is not normalized i.e., between 0 and 1! Found: %s"), *TimeRange.ToString()));
			bValidCurve = false;
		}

		if (!bValidCurve)
		{
			return CurrentValue;
		}
	}
#endif

	Timer += DeltaTime;
	float const ClampedTime = FMath::Clamp(AccelerationTime > 0.f ? Timer / AccelerationTime : 0.f, 0.f, 1.f);
	return CurrentValue.Get<FVector>() * AccelerationCurve->GetFloatValue(ClampedTime);
}