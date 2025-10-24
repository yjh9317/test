// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QTE/QTETypes.h"
#include "Widgets/Layout/Anchors.h"
#include "QTEComponent.generated.h"

class UQTEWidget;
class ABaseCharacter;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQTEStarted, const FQTEInfo&, QTEInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQTECompleted, const FQTEResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQTEFailed, const FQTEResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQTEInput, FKey, InputKey, bool, bWasCorrect);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQTEProgress, float, Progress);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UQTEComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UQTEComponent();

	// === 델리게이트 ===
	UPROPERTY(BlueprintAssignable)
	FOnQTEStarted OnQTEStarted;

	UPROPERTY(BlueprintAssignable)
	FOnQTECompleted OnQTECompleted;

	UPROPERTY(BlueprintAssignable)
	FOnQTEFailed OnQTEFailed;

	UPROPERTY(BlueprintAssignable)
	FOnQTEInput OnQTEInput;

	UPROPERTY(BlueprintAssignable)
	FOnQTEProgress OnQTEProgress;

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(Transient)
	TObjectPtr<ABaseCharacter> OwnerCharacter;

	// === QTE 설정 ===
	UPROPERTY(EditDefaultsOnly, Category = "QTE|Settings")
	FQTESettings QTESettings;

	UPROPERTY(EditDefaultsOnly, Category = "QTE|UI")
	TSubclassOf<UQTEWidget> QTEWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category = "QTE|Data")
	TObjectPtr<UDataTable> QTEDataTable;

	// === 현재 QTE 상태 ===
	UPROPERTY(BlueprintReadOnly, Category = "QTE|State")
	FQTEInfo CurrentQTEInfo;

	UPROPERTY(BlueprintReadOnly, Category = "QTE|State")
	EQTEStatus CurrentStatus = EQTEStatus::Inactive;

	UPROPERTY(BlueprintReadOnly, Category = "QTE|State")
	float CurrentProgress = 0.0f;

	// === UI 위젯 ===
	UPROPERTY(Transient)
	TObjectPtr<UQTEWidget> QTEWidget;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> KeyWidget;

private:
	// === 내부 상태 ===
	float QTEStartTime = 0.0f;
	float LastInputTime = 0.0f;
	int32 CurrentInputIndex = 0;
	int32 CorrectInputCount = 0;
	int32 TotalInputCount = 0;
	float AccumulatedAccuracy = 0.0f;
	int32 CurrentRetryCount = 0;
	bool bIsHolding = false;
	float HoldStartTime = 0.0f;

	// === 타이머 ===
	FTimerHandle QTEDurationTimer;
	FTimerHandle RetryTimer;
	FTimerHandle ProgressUpdateTimer;

	// === 내부 함수 ===
// 	void UpdateQTEProgress();
// 	void CompleteQTE();
// 	void FailQTE();
// 	void TimeoutQTE();
// 	void ResetQTE();
// 	float CalculateInputAccuracy(float InputTime) const;
// 	EQTEStatus DetermineInputResult(float Accuracy) const;
// 	void ProcessSequenceInput(FKey InputKey);
// 	void ProcessRapidInput(FKey InputKey);
// 	void ProcessHoldInput(FKey InputKey, bool bPressed);
// 	void ProcessTimingInput(FKey InputKey);
// 	void CreateQTEWidget();
// 	void DestroyQTEWidget();
// 	void UpdateWidgetDisplay();
//
// public:
//
// #pragma region Core_QTE_Functions
//
// 	UFUNCTION(BlueprintCallable, Category = "QTE")
// 	bool StartQTE(const FQTEInfo& QTEInfo);
//
// 	UFUNCTION(BlueprintCallable, Category = "QTE")
// 	bool StartQTEByTag(const FGameplayTag& QTETag);
//
// 	UFUNCTION(BlueprintCallable, Category = "QTE")
// 	void EndQTE();
//
// 	UFUNCTION(BlueprintCallable, Category = "QTE")
// 	void ProcessInput(FKey InputKey, bool bPressed = true);
//
// 	UFUNCTION(BlueprintCallable, Category = "QTE")
// 	bool RetryQTE();
//
// #pragma endregion
//
// #pragma region State_Queries
//
// 	UFUNCTION(BlueprintPure, Category = "QTE|State")
// 	bool IsQTEActive() const { return CurrentStatus == EQTEStatus::Active; }
//
// 	UFUNCTION(BlueprintPure, Category = "QTE|State")
// 	EQTEStatus GetQTEStatus() const { return CurrentStatus; }
//
// 	UFUNCTION(BlueprintPure, Category = "QTE|State")
// 	float GetQTEProgress() const { return CurrentProgress; }
//
// 	UFUNCTION(BlueprintPure, Category = "QTE|State")
// 	float GetRemainingTime() const;
//
// 	UFUNCTION(BlueprintPure, Category = "QTE|State")
// 	FQTEInfo GetCurrentQTEInfo() const { return CurrentQTEInfo; }
//
// 	UFUNCTION(BlueprintPure, Category = "QTE|State")
// 	int32 GetCorrectInputCount() const { return CorrectInputCount; }
//
// 	UFUNCTION(BlueprintPure, Category = "QTE|State")
// 	int32 GetTotalInputCount() const { return TotalInputCount; }
//
// 	UFUNCTION(BlueprintPure, Category = "QTE|State")
// 	float GetCurrentAccuracy() const;
//
// #pragma endregion
//
// #pragma region Settings
//
// 	UFUNCTION(BlueprintCallable, Category = "QTE|Settings")
// 	void SetQTESettings(const FQTESettings& NewSettings);
//
// 	UFUNCTION(BlueprintPure, Category = "QTE|Settings")
// 	FQTESettings GetQTESettings() const { return QTESettings; }
//
// 	UFUNCTION(BlueprintCallable, Category = "QTE|Settings")
// 	void SetQTEDataTable(UDataTable* NewDataTable);
//
// #pragma endregion
//
// #pragma region Input_Processing
//
// 	UFUNCTION(BlueprintCallable, Category = "QTE|Input")
// 	void QTEInputCheck(FKey RequiredKey);
//
// 	UFUNCTION(BlueprintCallable, Category = "QTE|Input")
// 	bool IsValidInputKey(FKey InputKey) const;
//
// 	UFUNCTION(BlueprintCallable, Category = "QTE|Input")
// 	FKey GetNextRequiredKey() const;
//
// 	UFUNCTION(BlueprintCallable, Category = "QTE|Input")
// 	TArray<FKey> GetRemainingKeys() const;
//
// #pragma endregion
//
// #pragma region Widget_Management
//
// 	UFUNCTION(BlueprintCallable, Category = "QTE|UI")
// 	void ShowQTEWidget();
//
// 	UFUNCTION(BlueprintCallable, Category = "QTE|UI")
// 	void HideQTEWidget();
//
// 	UFUNCTION(BlueprintCallable, Category = "QTE|UI")
// 	void UpdateQTEWidget();
//
// 	UFUNCTION(BlueprintPure, Category = "QTE|UI")
// 	UQTEWidget* GetQTEWidget() const { return QTEWidget; }
//
// #pragma endregion
//
// #pragma region Events
//
// 	UFUNCTION(BlueprintImplementableEvent, Category = "QTE|Events")
// 	void OnQTEPerfectTiming();
//
// 	UFUNCTION(BlueprintImplementableEvent, Category = "QTE|Events")
// 	void OnQTEGoodTiming();
//
// 	UFUNCTION(BlueprintImplementableEvent, Category = "QTE|Events")
// 	void OnQTEMissedInput();
//
// 	UFUNCTION(BlueprintImplementableEvent, Category = "QTE|Events")
// 	void OnQTESequenceAdvanced(int32 CurrentStep, int32 TotalSteps);
//
// 	UFUNCTION(BlueprintImplementableEvent, Category = "QTE|Events")
// 	void OnQTEHoldProgress(float HoldProgress);
//
// #pragma endregion
//
// #pragma region Utility
//
// 	UFUNCTION(BlueprintCallable, Category = "QTE|Utility")
// 	FQTEResult CreateQTEResult() const;
//
// 	UFUNCTION(BlueprintCallable, Category = "QTE|Utility")
// 	void ResetStatistics();
//
// 	UFUNCTION(BlueprintPure, Category = "QTE|Utility")
// 	bool CanRetry() const;
//
// 	UFUNCTION(BlueprintCallable, Category = "QTE|Debug")
// 	void SetDebugMode(bool bEnabled);
//
// 	UFUNCTION(BlueprintCallable, Category = "QTE|Debug")
// 	void LogQTEInfo() const;
//
// #pragma endregion
//
// private:
// 	// === 디버그 ===
// 	bool bDebugMode = false;
// 	void DebugLogQTE(const FString& Message) const;
};

