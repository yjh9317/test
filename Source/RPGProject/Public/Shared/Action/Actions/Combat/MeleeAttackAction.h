// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Interface/ActionNotifyHandler.h"
#include "Shared/Action/BaseAction.h"
#include "MeleeAttackAction.generated.h"


USTRUCT(BlueprintType)
struct FMeleeAttackData
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animation")
    TArray<UAnimMontage*> ComboMontages;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
    bool bCanMoveWhileAttacking = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
    float MovementSpeedWhileAttacking = 0.3f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
    float ComboTimeoutDuration = 2.0f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
    float InputQueueDuration = 0.3f;

    bool IsValid() const { return ComboMontages.Num() > 0; }
};
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UMeleeAttackAction : public UBaseAction, public IActionNotifyHandler
{
	GENERATED_BODY()
	
public:
    UMeleeAttackAction(const FObjectInitializer& ObjectInitializer);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee Attack")
    FMeleeAttackData MeleeData;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Melee Attack")
    float BaseStaminaCost = 10.0f;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Combo State")
	int32 CurrentComboIndex = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Combo State")
	bool bComboWindowOpen = false;

	UPROPERTY(BlueprintReadOnly, Category = "Combo State")
	bool bMovementCancelable = false;

	float LastAttackTime = 0.0f;
	float BlendOutTime = 0.2f;
	
	UPROPERTY()
	TArray<AActor*> AlreadyHitActors;

	UPROPERTY()
	TObjectPtr<UAnimMontage> CurrentAnimMontage;

public:
	// 콤보 윈도우 관리
	void SetComboWindowOpen(bool bOpen) { bComboWindowOpen = bOpen; }
	bool IsComboWindowOpen() const { return bComboWindowOpen; }
	
	void OnComboWindowClosed();
	void CancelToMovement(const FVector& Direction);
    void OnAttackMontageEnded();
	// 콤보 처리
	UFUNCTION(BlueprintCallable, Category = "Combo")
	bool TryProcessComboInput();

	UFUNCTION(BlueprintPure, Category = "Combat")
	int32 GetCurrentComboIndex() const { return CurrentComboIndex; }

	UFUNCTION(BlueprintPure, Category = "Combat")
	int32 GetMaxComboCount() const { return MeleeData.ComboMontages.Num(); }

protected:
	virtual void Initialize(AActor* InOwner) override;
	virtual bool CanExecute() const override;
	virtual void OnComplete() override;
	virtual void OnExecute();
	virtual void OnInterrupt();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformAttack();

	UFUNCTION(BlueprintCallable, Category = "Animation")
	bool PlayAttackMontage();

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void ResetCombo();
 
	bool ValidateExecution() const;
	float GetStaminaCostForCombo() const;

	virtual void HandleActionEvent(EActionEvent EventType, float Value) override;
	virtual bool CanHandleActionEvent(EActionEvent EventType) const override;
	
private:
	UPROPERTY()
	TObjectPtr<UInputBufferComponent> CachedInputBuffer;
};
