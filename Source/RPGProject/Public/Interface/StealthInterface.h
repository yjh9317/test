// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "StealthInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UStealthInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGPROJECT_API IStealthInterface
{
	GENERATED_BODY()

public:
	// 은신 상태 관리
	virtual bool CanEnterStealth() const = 0;
	virtual void EnterStealth() = 0;
	virtual void ExitStealth(bool bForced = false) = 0;
	virtual bool IsInStealth() const = 0;
    
	// 감지 시스템
	virtual float GetDetectionRadius() const = 0;
	virtual float GetNoiseLevel() const = 0;
	virtual void SetNoiseLevel(float NewLevel) = 0;
    
	// 은신 공격
	virtual bool CanPerformStealthAttack(AActor* Target) const = 0;
	virtual void OnStealthBreak(AActor* DetectedBy) = 0;
};
