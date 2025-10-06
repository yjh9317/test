// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Action/BaseAction.h"
#include "RangedAttackAction.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API URangedAttackAction : public UBaseAction
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Ranged")
	float DrawTime = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Ranged")
	float MaxDrawTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Ranged")
	bool bCanHold = true;

protected:
	float CurrentDrawTime = 0.0f;
	bool bIsDrawing = false;
};
