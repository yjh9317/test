// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Action/BaseAction.h"
#include "ChargeAction.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class RPGPROJECT_API UChargeAction : public UBaseAction
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge")
	float MaxChargeTime = 2.0f;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge")
	UCurveFloat* ChargeCurve; // 차지 곡선
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge")
	bool bAutoReleaseAtMax = true;
    
	// 블루프린트 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Charge")
	void OnChargeStart();
    
	UFUNCTION(BlueprintImplementableEvent, Category = "Charge")
	void OnChargeUpdate(float ChargePercent);
    
	UFUNCTION(BlueprintImplementableEvent, Category = "Charge")
	void OnChargeRelease(float FinalChargePercent);
    
	UFUNCTION(BlueprintCallable, Category = "Charge")
	void ReleaseCharge() {};
    
	UFUNCTION(BlueprintPure, Category = "Charge")
	float GetChargePercent() const { return CurrentChargeTime; }
    
protected:
	float CurrentChargeTime = 0.0f;
	bool bIsCharging = false;
};
