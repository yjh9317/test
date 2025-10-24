// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNodeBase.h"
#include "UObject/Interface.h"
#include "WeaponLayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWeaponLayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGPROJECT_API IWeaponLayerInterface
{
	GENERATED_BODY()

public:
	// ============ 포즈 처리 함수들 (AnimGraph에서 구현) ============
    
	// 전신 오버라이드 (걷기/달리기 등 기본 동작 대체)
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (AnimGetter))
	void FullBody(FPoseLink InBasePose, FPoseLink& OutPose);
    
	// 상체만 오버라이드 (하체는 기본 locomotion 유지)
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (AnimGetter))
	void UpperBody(FPoseLink InBasePose, FPoseLink& OutPose);
    
	// 팔 추가 애니메이션 (Additive)
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (AnimGetter))
	void ArmsAdditive(FPoseLink InBasePose, FPoseLink& OutPose);
    
	// 손가락 디테일 (활시위 잡기, 트리거 등)
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, meta = (AnimGetter))
	void Hands(FPoseLink InBasePose, FPoseLink& OutPose);
    
	// ============ 데이터 업데이트 함수들 (C++/BP에서 호출) ============
    
	// 무기별 변수 업데이트 (매 프레임)
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void UpdateWeaponAnimationData(float DeltaTime);
    
	// 무기 장착 시 초기화
	//UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	//void OnWeaponEquipped(AWeaponBase* Weapon);
    
	// 무기 해제 시 정리
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)  
	void OnWeaponUnequipped();
};
