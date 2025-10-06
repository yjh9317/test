// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/BlendSpace.h"
#include "Player/Character/MainAnimInstance.h"
#include "BowAnimInstance.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EBowStance : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Battle UMETA(DisplayName = "Battle")
};


UENUM(BlueprintType)
enum class EBowPose : uint8
{
	EBP_Idle      UMETA(DisplayName = "Idle Pose"),         // 기본 대기 포즈
	EBP_Forward   UMETA(DisplayName = "Forward Pose"),      // 정면 포즈
	EBP_Angled    UMETA(DisplayName = "Angled Pose")        // 측면 포즈
};

UCLASS()
class RPGPROJECT_API UBowAnimInstance : public UMainAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Bow")
	float DrawAmount;                 // 0 ~ 1 (당김 정도)
    
	UPROPERTY(BlueprintReadOnly, Category = "Bow")
	bool bIsAiming;                   // 조준 중
    
	UPROPERTY(BlueprintReadOnly, Category = "Bow")
	FRotator AimRotation;            // 조준 각도
    
	// UPROPERTY(BlueprintReadOnly, Category = "Bow")
	// float AimYaw;                    // Aim Offset용
 
	// UPROPERTY(BlueprintReadOnly, Category = "Bow")
	// float AimPitch;

	UPROPERTY(BlueprintReadOnly, Category = "Bow")
	float AimBlend;

	UPROPERTY(BlueprintReadOnly, Category = "Bow")
	EBowPose BowPose;

private:
	// Walk 애니메이션 세트 캐싱
	UPROPERTY(BlueprintReadOnly, Category = "Cached|Walk", meta = (AllowPrivateAccess = true))
	UAnimSequence* CachedWalkStart;
    
	UPROPERTY(BlueprintReadOnly, Category = "Cached|Walk", meta = (AllowPrivateAccess = true))
	UAnimSequence* CachedWalkLoop;
    
	UPROPERTY(BlueprintReadOnly, Category = "Cached|Walk", meta = (AllowPrivateAccess = true))
	UAnimSequence* CachedWalkStop;
    
	UPROPERTY(BlueprintReadOnly, Category = "Cached|Walk", meta = (AllowPrivateAccess = true))
	UBlendSpace* CachedWalkBS;
    
	// 상태 추적 변수
	UPROPERTY(BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = true))
	bool bJustStartedMoving;  // Start 애니메이션 트리거
    
	UPROPERTY(BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = true))
	bool bJustStoppedMoving;  // Stop 애니메이션 트리거
    
	UPROPERTY(BlueprintReadOnly, Category = "State", meta = (AllowPrivateAccess = true))
	float TimeSinceStartedMoving;

public:
	void UpdateCachedAnimations(const FWeaponAnimationRow& AnimSet)
	{
		// Walk 세트 업데이트
		if (AnimSet.Walk.IsValid())
		{
			CachedWalkStart = AnimSet.Walk.Start.Get();
			CachedWalkLoop = AnimSet.Walk.Loop.Get();
			CachedWalkStop = AnimSet.Walk.Stop.Get();
			CachedWalkBS = AnimSet.Walk.LoopBS.Get();
		}
	}

protected:
	virtual void NativeUpdateAnimation(float DeltaSeconds) override
	{
		Super::NativeUpdateAnimation(DeltaSeconds);
        
		// Start/Stop 감지
		bool bIsMovingNow = GroundSpeed > 5.0f;
        
		bJustStartedMoving = bIsMovingNow && !bWasMoving;
		bJustStoppedMoving = !bIsMovingNow && bWasMoving;
        
		if (bJustStartedMoving)
		{
			TimeSinceStartedMoving = 0.0f;
		}
		else if (bIsMovingNow)
		{
			TimeSinceStartedMoving += DeltaSeconds;
		}
        
		bWasMoving = bIsMovingNow;
	}
};
