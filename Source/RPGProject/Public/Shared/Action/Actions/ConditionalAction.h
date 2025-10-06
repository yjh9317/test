// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Action/BaseAction.h"
#include "ConditionalAction.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class RPGPROJECT_API UConditionalAction : public UBaseAction
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	bool CheckCondition();
    
	UFUNCTION(BlueprintImplementableEvent)
	void OnConditionMet();
    
	UFUNCTION(BlueprintImplementableEvent)
	void OnConditionFailed();
};
