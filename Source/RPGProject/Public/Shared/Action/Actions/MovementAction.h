// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Action/BaseAction.h"
#include "MovementAction.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UMovementAction : public UBaseAction
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	float MovementDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	bool bDisableInputDuringMovement = true;

	UPROPERTY(EditDefaultsOnly, Category = "Movement")
	bool bGrantsInvincibility = false;

protected:
	FTimerHandle MovementEndTimer;

	virtual void StartMovement() PURE_VIRTUAL(UMovementAction::StartMovement, );
	virtual void EndMovement() {};
};
