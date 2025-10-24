// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FinishableInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFinishableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGPROJECT_API IFinishableInterface
{
	GENERATED_BODY()

public:
	// 처형 가능 여부
	virtual bool CanBeFinished() const = 0;
	virtual bool CanPerformFinisher(AActor* Target) const = 0;
    
	// 처형 실행
	virtual void ExecuteFinisher(AActor* Attacker, FName FinisherType) = 0;
	virtual void ReceiveFinisher(AActor* Executor, FName FinisherType) = 0;
    
	// 처형 애니메이션
	virtual UAnimMontage* GetFinisherMontage(FName FinisherType, bool bIsExecutor) const = 0;
	virtual FTransform GetFinisherPosition(AActor* Target) const = 0;
    
	// 처형 중 상태
	virtual bool IsPerformingFinisher() const = 0;
	virtual void OnFinisherComplete() = 0;
};
