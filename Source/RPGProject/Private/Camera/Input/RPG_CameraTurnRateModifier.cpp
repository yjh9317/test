// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/Input/RPG_CameraTurnRateModifier.h"

#include "EnhancedPlayerInput.h"
#include "Camera/RPG_PlayerCameraManager.h"
#include "GameFramework/PlayerController.h"

FInputActionValue URPG_CameraTurnRateModifier::ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	EInputActionValueType ValueType = CurrentValue.GetValueType();
	if (ValueType == EInputActionValueType::Boolean)
	{
		return CurrentValue;
	}

	if (!PlayerCameraManager)
	{
		if (!PlayerInput->GetOuterAPlayerController() || !PlayerInput->GetOuterAPlayerController()->PlayerCameraManager)
		{
#if ENABLE_DRAW_DEBUG
			// Debugging
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("RPG_CameraSlowDownInputModifier: Could not find Player Camera Manager to use Camera Slow Down constraint.")));
			}
#endif
			return CurrentValue;
		}

		ARPG_PlayerCameraManager* PCManager = Cast<ARPG_PlayerCameraManager>(PlayerInput->GetOuterAPlayerController()->PlayerCameraManager);
		if (!PCManager)
		{
#if ENABLE_DRAW_DEBUG
			// Debugging
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("RPG_CameraSlowDownInputModifier: Player's Camera Manager does not inherit from RPG_PlayerCameraManager. Angle Constraint cannot be used.")));
			}
#endif
			return CurrentValue;
		}
		// Here we know that it's not null
		PlayerCameraManager = PCManager;
	}

	return CurrentValue.Get<FVector>() * PlayerCameraManager->GetCameraTurnRate();
}
