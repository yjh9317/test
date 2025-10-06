// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseAnimInstance.h"
#include "SwordShieldAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API USwordShieldAnimInstance : public UBaseAnimInstance
{
	GENERATED_BODY()
protected:
	// ========== 방어 시스템 ==========
	UPROPERTY(BlueprintReadOnly, Category = "Defense")
	bool bIsBlocking;
    
	UPROPERTY(BlueprintReadOnly, Category = "Defense")
	bool bIsParrying;  // 패링 타이밍
    
	UPROPERTY(BlueprintReadOnly, Category = "Defense")
	float BlockAngle;  // 방어 각도 (-90 ~ 90)
    
	UPROPERTY(BlueprintReadOnly, Category = "Defense")
	bool bBlockHit;  // 방어 중 피격
    
	UPROPERTY(BlueprintReadOnly, Category = "Defense")
	float BlockStability;  // 방어 안정성 (0~1)
    
	// ========== 방패 투척 ==========
	UPROPERTY(BlueprintReadOnly, Category = "Shield")
	bool bIsThrowingShield;
    
	UPROPERTY(BlueprintReadOnly, Category = "Shield")
	bool bShieldReturning;
    
	UPROPERTY(BlueprintReadOnly, Category = "Shield")
	float ShieldThrowCharge;  // 투척 차지
    
	// ========== 공격 시스템 ==========
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bIsThrusting;  // 찌르기
    
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	int32 ThrustCombo;  // 연속 찌르기
    
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bIsExecuting;
    
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bIsPlunging;
    
	// ========== 도발 ==========
	UPROPERTY(BlueprintReadOnly, Category = "Taunt")
	bool bIsTaunting;
    
	// UPROPERTY(BlueprintReadOnly, Category = "Taunt")
	// ETauntType TauntType;
    
	// ========== 방패 위치 ==========
	UPROPERTY(BlueprintReadOnly, Category = "Shield Position")
	FVector ShieldOffset;  // 방패 위치 오프셋
    
	UPROPERTY(BlueprintReadOnly, Category = "Shield Position")
	FRotator ShieldRotation;  // 방어 각도에 따른 회전
};
