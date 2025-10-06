// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputModifiers.h"
#include "RPG_CameraTurnRateModifier.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API URPG_CameraTurnRateModifier : public UInputModifier
{
	GENERATED_BODY()
public:
	virtual FInputActionValue ModifyRaw_Implementation(const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime) override;

private:
	TObjectPtr<class ARPG_PlayerCameraManager> PlayerCameraManager;
};
