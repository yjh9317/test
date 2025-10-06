// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Action/BaseAction.h"
#include "PassiveAction.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class RPGPROJECT_API UPassiveAction : public UBaseAction
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void OnPassiveActivate();
    
	UFUNCTION(BlueprintImplementableEvent)
	void OnPassiveDeactivate();
};
