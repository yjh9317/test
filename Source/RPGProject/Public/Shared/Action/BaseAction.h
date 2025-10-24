// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActionTypes.h"
#include "GameplayTagContainer.h"
#include "Shared/Action/Data/ActionData.h"
#include "BaseAction.generated.h"

class UInputBufferComponent;
class UStatsComponent;
class UCharacterMovementComponent;
class UCapsuleComponent;
class UStateComponent;
class UActionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActionFinished);
/**
 * 
 */
UCLASS(Blueprintable,BlueprintType)
class RPGPROJECT_API UBaseAction : public UObject
{
	GENERATED_BODY()
	
public:
	UBaseAction(const FObjectInitializer& ObjectInitializer);

	// === 기본 정보 ===
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action")
	FGameplayTag ActionTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action")
	FText ActionName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Action")
	UTexture2D* ActionIcon;

	// === 태그 시스템 (GAS 스타일) ===
	// 이 Action을 실행하기 위해 필요한 태그들
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	FGameplayTagContainer RequiredTags;

	// 이 태그들이 있으면 Action 실행 불가
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	FGameplayTagContainer BlockedTags;

	// Action 실행 중에 부여되는 태그들
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	FGameplayTagContainer GrantedTags;

	// Action이 실행 중일 때 제거되는 태그들
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Tags")
	FGameplayTagContainer RemoveTags;

	// === 리소스 시스템 ===
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost")
	TArray<FActionCost> ResourceCosts;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cost")
	float CooldownDuration = 0.0f;
	
	// === 실행 조건 ===
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Conditions")
	bool bCanExecuteWhileMoving = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Conditions")
	bool bCanExecuteInAir = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Conditions")
	bool bInterruptible = false; // 다른 Action에 의해 중단 가능한지
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Conditions")
	bool bCanInterruptOthers = false;  // 다른 액션을 중단시킬 수 있는지  

	// UPROPERTY(BlueprintAssignable, Category = "Events")
	// FOnActionStarted OnActionStarted;
	//
	// UPROPERTY(BlueprintAssignable, Category = "Events")
	// FOnActionFinished OnActionFinished;
protected:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UActionComponent> CachedActionComponent;

	bool bIsActive = false;
	float TimeActive = 0.0f;
	float LastExecutionTime = -999.0f;

public:
    virtual void Initialize(AActor* InOwner);

	void Execute();
    void Tick(float DeltaTime);
    void Interrupt();
	void Complete();

    UFUNCTION(BlueprintPure, Category = "Action")
    virtual bool CanExecute() const;

    UFUNCTION(BlueprintPure, Category = "Action")
    virtual bool HasRequiredResources() const;

    UFUNCTION(BlueprintPure, Category = "Action")
    bool IsActive() const { return bIsActive; }

    UFUNCTION(BlueprintPure, Category = "Action")
    float GetTimeActive() const { return TimeActive; }

    UFUNCTION(BlueprintPure, Category = "Action")
    float GetCooldownRemaining() const;
	
    UFUNCTION(BlueprintPure, Category = "Action")
    FGameplayTag GetActionTag() const { return ActionTag; }

    UFUNCTION(BlueprintPure, Category = "Action")
    ACharacter* GetOwnerCharacter() const { return OwnerCharacter; }

    UFUNCTION(BlueprintPure, Category = "Action")
    APlayerController* GetOwnerController() const;

protected:
    virtual void ConsumeResources();
    virtual void RefundResources(float Percentage = 1.0f);
    virtual void ApplyGrantedTags();
    virtual void RemoveGrantedTags();


#pragma region InternalFunction
    virtual void OnInitialized(){};
    virtual void OnExecute(){};
    virtual void OnTick(float DeltaTime){};
    virtual void OnInterrupt(){};
    virtual void OnComplete() {};
#pragma endregion
	
	virtual void StartCooldown();
    virtual bool OnHasRequiredResources() const { return true; }
};
