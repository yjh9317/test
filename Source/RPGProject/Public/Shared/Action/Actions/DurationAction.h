// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Action/BaseAction.h"
#include "DurationAction.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class RPGPROJECT_API UDurationAction : public UBaseAction
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Duration")
	float Duration = 5.0f;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Duration")
	float TickInterval = 0.5f;
    
	// 블루프린트 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Duration")
	void OnDurationStart();
    
	UFUNCTION(BlueprintImplementableEvent, Category = "Duration")
	void OnDurationTick();
    
	UFUNCTION(BlueprintImplementableEvent, Category = "Duration")
	void OnDurationEnd();
    
	UFUNCTION(BlueprintPure, Category = "Duration")
	float GetRemainingTime() const { return Duration; }
    
	UFUNCTION(BlueprintPure, Category = "Duration")
	float GetElapsedPercent() const { return 0.f;};
};
