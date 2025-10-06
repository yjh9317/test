// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Action/BaseAction.h"
#include "ToggleAction.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UToggleAction : public UBaseAction
{
	GENERATED_BODY()

public:
	// 블루프린트 구현 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Toggle")
	void OnActivate();
    
	UFUNCTION(BlueprintImplementableEvent, Category = "Toggle")
	void OnDeactivate();
    
	UFUNCTION(BlueprintImplementableEvent, Category = "Toggle")
	void OnToggleTick(float DeltaTime);
    
	// 토글 상태 체크
	UFUNCTION(BlueprintPure, Category = "Toggle")
	bool IsToggled() const { return bIsToggled; }
    
	// 수동 토글 (블루프린트에서 호출 가능)
	UFUNCTION(BlueprintCallable, Category = "Toggle")
	void ToggleOff() {};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Toggle")
	bool bStartsActive = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Toggle")
	float DrainPerSecond = 0.0f; // 스태미나 등 소모

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Toggle")
	FGameplayTag DrainStatTag;

protected:
	bool bIsToggled = false;
	FTimerHandle DrainTimer;
    
	virtual void OnToggleOn() {}
	virtual void OnToggleOff() {}
    
	void ProcessDrain() {};

	virtual void OnExecute_Implementation() override
	{
		if (bIsToggled)
		{
			DeactivateToggle();
		}
		else
		{
			ActivateToggle();
		}
	}
    
	void ActivateToggle()
	{
		bIsToggled = true;
		OnActivate();
        
		// 드레인 설정
		if (DrainPerSecond > 0.0f)
		{
			GetWorld()->GetTimerManager().SetTimer(
				DrainTimer,
				this,
				&UToggleAction::ProcessDrain,
				0.1f,
				true
			);
		}
	}
    
	void DeactivateToggle()
	{
		bIsToggled = false;
		OnDeactivate();
		GetWorld()->GetTimerManager().ClearTimer(DrainTimer);
		Complete();
	}
};
