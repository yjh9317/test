// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Action/BaseAction.h"
#include "SequenceAction.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API USequenceAction : public UBaseAction
{
	GENERATED_BODY()
public:
	// 블루프린트에서 각 단계 구현
	UFUNCTION(BlueprintImplementableEvent, Category = "Sequence")
	void OnStepExecute(int32 StepIndex);
    
	UFUNCTION(BlueprintImplementableEvent, Category = "Sequence")
	int32 GetTotalSteps();
    
	UFUNCTION(BlueprintImplementableEvent, Category = "Sequence")
	float GetStepDelay(int32 StepIndex);
    
	UFUNCTION(BlueprintCallable, Category = "Sequence")
	void NextStep() {};
    
protected:
	int32 CurrentStep = 0;
	FTimerHandle StepTimer;
};
