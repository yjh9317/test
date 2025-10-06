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

	// === 이벤트 델리게이트 ===
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnActionStarted OnActionStarted;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnActionFinished OnActionFinished;

protected:
	// === 런타임 상태 ===
	UPROPERTY()
	TObjectPtr<AActor> OwnerActor;

	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UActionComponent> CachedActionComponent;

	UPROPERTY()
	TObjectPtr<UInputBufferComponent> CachedInputBuffer;

	bool bIsActive = false;
	float TimeActive = 0.0f;
	float LastExecutionTime = -999.0f;

public:
    // === 핵심 라이프사이클 ===
    virtual void Initialize(AActor* InOwner);

    UFUNCTION(BlueprintCallable, Category = "Action")
    virtual void Execute();

    UFUNCTION(BlueprintCallable, Category = "Action")
    virtual void Tick(float DeltaTime);

    UFUNCTION(BlueprintCallable, Category = "Action")
    virtual void Interrupt();

    UFUNCTION(BlueprintCallable, Category = "Action")
    virtual void Complete();

    // === 실행 조건 체크 ===
    UFUNCTION(BlueprintPure, Category = "Action")
    virtual bool CanExecute() const;

    UFUNCTION(BlueprintPure, Category = "Action")
    virtual bool HasRequiredResources() const;

    // === 상태 쿼리 ===
    UFUNCTION(BlueprintPure, Category = "Action")
    bool IsActive() const { return bIsActive; }

    UFUNCTION(BlueprintPure, Category = "Action")
    float GetTimeActive() const { return TimeActive; }

    UFUNCTION(BlueprintPure, Category = "Action")
    float GetCooldownRemaining() const;

    UFUNCTION(BlueprintPure, Category = "Action")
    float GetExecutionProgress() const { return 0.0f; } // 파생 클래스에서 구현

    // === 유틸리티 함수들 ===
    UFUNCTION(BlueprintPure, Category = "Action")
    FGameplayTag GetActionTag() const { return ActionTag; }

    UFUNCTION(BlueprintPure, Category = "Action")
    AActor* GetOwnerActor() const { return OwnerActor; }

    UFUNCTION(BlueprintPure, Category = "Action")
    ACharacter* GetOwnerCharacter() const { return OwnerCharacter; }

    UFUNCTION(BlueprintPure, Category = "Action")
    APlayerController* GetOwnerController() const;

    // UObject 오버라이드
    virtual UWorld* GetWorld() const override;

protected:
    virtual void ConsumeResources();
    virtual void RefundResources(float Percentage = 1.0f);
	
    virtual void ApplyGrantedTags();
    virtual void RemoveGrantedTags();

    virtual void StartCooldown();

	UFUNCTION(BlueprintNativeEvent, Category = "Action|Lifecycle")
    void OnInitialized();
	virtual void OnInitialized_Implementation() {};
	
    UFUNCTION(BlueprintNativeEvent, Category = "Action|Lifecycle")
    void OnExecute();
    virtual void OnExecute_Implementation() {};

    UFUNCTION(BlueprintNativeEvent, Category = "Action|Lifecycle")
    void OnTick(float DeltaTime);
    virtual void OnTick_Implementation(float DeltaTime) {};

    UFUNCTION(BlueprintNativeEvent, Category = "Action|Lifecycle")
    void OnInterrupt();
    virtual void OnInterrupt_Implementation() {};

    UFUNCTION(BlueprintNativeEvent, Category = "Action|Lifecycle")
    void OnComplete();
    virtual void OnComplete_Implementation() {};

    // === 조건 체크 가상 함수들 ===
    UFUNCTION(BlueprintNativeEvent, Category = "Action|Conditions")
    bool OnCanExecute() const;
    virtual bool OnCanExecute_Implementation() const { return true; }

    UFUNCTION(BlueprintNativeEvent, Category = "Action|Resources")
    bool OnHasRequiredResources() const;
    virtual bool OnHasRequiredResources_Implementation() const { return true; }

};
