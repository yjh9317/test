// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseAnimInstance.h"
#include "TwoHandedAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UTwoHandedAnimInstance : public UBaseAnimInstance
{
	GENERATED_BODY()
	protected:
    // ========== 차징 시스템 ==========
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Charge")
    bool bIsCharging;
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Charge")
    float ChargeAmount;  // 0.0 ~ 1.0
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Charge")
    float ChargeHoldTime;
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Charge")
    bool bChargeReleased;
    
    // ========== 전투 상태 ==========
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bIsInCombatStance;
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    int32 ComboCounter;  // 콤보 공격용
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bIsExecuting;  // Execution 중인지
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    bool bIsPlunging;  // 낙하 공격 중
    
    UPROPERTY(BlueprintReadOnly, Category = "Combat")
    float PlungeFallSpeed;  // 낙하 속도
    
    // ========== 버프 ==========
    UPROPERTY(BlueprintReadOnly, Category = "Buff")
    bool bIsBuffing;
    
    // UPROPERTY(BlueprintReadOnly, Category = "Buff")
    //EBuffType CurrentBuffType;
    
    UPROPERTY(BlueprintReadOnly, Category = "Buff")
    float BuffChannelTime;
    
    // ========== 무기 위치/회전 ==========
    UPROPERTY(BlueprintReadOnly, Category = "Weapon")
    float WeaponSwingAngle;  // 휘두르기 각도
    
    UPROPERTY(BlueprintReadOnly, Category = "Weapon")
    float WeaponWeight;  // 무거운 무기 표현용
    
    // ========== IK ==========
    // UPROPERTY(BlueprintReadOnly, Category = "IK")
    // FVector LeftHandIKLocation;  // 양손 그립
    
    // UPROPERTY(BlueprintReadOnly, Category = "IK")
    // FVector RightHandIKLocation;
    
    // ========== Montages (C++에서 관리) ==========
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* ExecutionMontage;
    
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    UAnimMontage* PlungeAttackMontage;
    
    UPROPERTY(EditDefaultsOnly, Category = "Montages")
    TArray<UAnimMontage*> ComboMontages;
};
