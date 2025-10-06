// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Action/BaseAction.h"
#include "InstantAction.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UInstantAction : public UBaseAction
{
	GENERATED_BODY()

public:
	// 블루프린트에서 구현할 핵심 함수
	UFUNCTION(BlueprintImplementableEvent, Category = "Instant Action")
	void OnInstantExecute();
    
	// 옵션: 실행 전/후 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Instant Action")
	void OnPreExecute();
    
	UFUNCTION(BlueprintImplementableEvent, Category = "Instant Action")
	void OnPostExecute();
    
	// 효과 데이터 (블루프린트에서 설정)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	float EffectMagnitude = 1.0f;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Effect")
	float EffectRadius = 0.0f; // 0이면 단일 대상
	
protected:
	virtual void OnExecute_Implementation() override
	{
		// 전처리
		OnPreExecute();
        
		// 메인 실행
		ExecuteInstant();
        
		// 후처리
		OnPostExecute();
        
		// 즉시 완료
		Complete();
	}
    
	// 파생 클래스에서 구현할 효과
	virtual void ExecuteInstant()
	{
		// 블루프린트 이벤트 호출
		OnInstantExecute();
	}
};
