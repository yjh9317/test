// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Action/Actions/InstantAction.h"
#include "RollAction.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API URollAction : public UInstantAction
{
	GENERATED_BODY()
public:
	URollAction(const FObjectInitializer& ObjectInitializer);

	// Motion Warping 설정
	UPROPERTY(EditDefaultsOnly, Category = "Roll")
	float RollDistance = 300.0f;
    
	UPROPERTY(EditDefaultsOnly, Category = "Roll")
	float ObstacleCheckHeight = 100.0f;
    
	UPROPERTY(EditDefaultsOnly, Category = "Roll")
	FName WarpTargetName = "RollTarget";

protected:
	// mutable을 사용하여 const 함수에서도 수정 가능
	mutable FVector CachedWarpLocation;
	mutable FVector CachedInputDirection;

	// 캐시 유효 시간 (0.1초)
	static constexpr float CacheValidityDuration = 0.1f;

	virtual bool CanExecute() const override;
    
protected:
	virtual void OnExecute_Implementation() override;
	virtual void ExecuteInstant() override;
    
	// LineTrace 계산 (const로 mutable 변수 수정)
	bool CalculateWarpLocation() const;
	void InvalidateCache();
	bool IsCacheValid() const;
};
