// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ParryableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UParryableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGPROJECT_API IParryableInterface
{
	GENERATED_BODY()

public:
	// 패링 가능 여부
	virtual bool CanBeParried() const = 0;
	virtual bool IsInParryWindow() const = 0;
	virtual float GetParryWindowDuration() const = 0;
    
	// 패링 실행
	virtual void OnParried(AActor* ParryingActor, const FHitResult& HitInfo) = 0;
	virtual void OnPerfectParry(AActor* ParryingActor) = 0;
    
	// 패링 결과
	virtual float GetStaggerDuration() const = 0;
	virtual void ApplyParryStagger(float Duration) = 0;
	virtual bool ShouldDeflectProjectile() const = 0;
};
