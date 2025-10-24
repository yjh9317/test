// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RPGQuest.generated.h"

/**
 * 
 */

enum class EQuestNotifyEventType : uint8;

UENUM(BlueprintType)
enum class EQuestState : uint8
{
	Active				UMETA(DisplayName = "Active"),
	Pending				UMETA(DisplayName = "Pending"),
	Valid				UMETA(DisplayName = "Valid"),
	Archive				UMETA(DisplayName = "Archive"),
	Failed				UMETA(DisplayName = "Failed"),
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQuestStateChangedDelegate, URPGQuest*, QuestUpdate, EQuestState, QuestState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPositiveObjectiveUpdateDelegate, URPGQuest*, QuestUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNegativeObjectiveUpdateDelegate, URPGQuest*, QuestUpdate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestRequirementMetDelegate, URPGQuest*, QuestUpdate);

class URPGQuestData;

UCLASS(Blueprintable,BlueprintType)
class RPGPROJECT_API URPGQuest : public UObject
{
	GENERATED_BODY()

public:
	URPGQuest();
	~URPGQuest();
	
	/* Quest Data */
	UPROPERTY(BlueprintReadWrite, Category = "Advanced Quest | Quest")
	EQuestState QuestState = EQuestState::Pending;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest")
	URPGQuestData* QuestData = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "Advanced Quest | Quest")
	bool AllObjectivesCompleted = false;

	UFUNCTION(BlueprintCallable, Category = "Advanced Quest | Events")
	void EnableQuest();
	
	UFUNCTION(BlueprintCallable, Category = "Advanced Quest | Events")
	void DisableQuest();
	
	UFUNCTION(BlueprintCallable, Category = "Advanced Quest | Events")
	void ResetQuest();
	
	UFUNCTION(BlueprintCallable, Category = "Advanced Quest | Events")
	void ResetObjectives();
	
	UFUNCTION(BlueprintCallable, Category = "Advanced Quest | Events")
	void QuestFailed();
	
	void SetQuestData(URPGQuestData* _QuestData);
	void UpdateQuestState();

	/* Display Properties */
	UPROPERTY(BlueprintReadWrite, Category = "Advanced Quest | UI")
	bool bIsDisplayQuestLog = false;

	UPROPERTY(BlueprintReadWrite, Category = "Advanced Quest | UI")
	bool bIsDisplayQuestTracker = false;

	UPROPERTY(BlueprintReadWrite, Category = "Advanced Quest | UI")
	int IndexQuestTracker = 0;


	/* Requirements */
	UPROPERTY(BlueprintReadWrite, Category = "Advanced Quest | Quest")
	bool bIsRequirementMet = true;


	/* Event Listeners */
	// UFUNCTION()
	
	void OnNotify(UObject* entity, EQuestNotifyEventType eventTypeP, float amount = 1);
	void UpdateCurrentObjective(int i, float amount = 1);
	
	UFUNCTION()
	void OnQuestRequirementChange(URPGQuest* questUpdateP, EQuestState questStateP);
	UFUNCTION()
	void OnEventRequirementChange(UQuestSpecialEventData* specialEvent);
	UFUNCTION()
	void OnLevelRequirementChange(int PlayerLevel);
	// UFUNCTION()
	// void OnNewDay();

	/* Delegates */
	FQuestStateChangedDelegate QuestStateChangedDelegate;
	FPositiveObjectiveUpdateDelegate PositiveObjectiveUpdateDelegate;
	FNegativeObjectiveUpdateDelegate NegativeObjectiveUpdateDelegate;
	FQuestRequirementMetDelegate QuestRequirementMetDelegate;


private:
	/* Objectives */
	// bool IsSameObject(int objectiveIndexP,UObject* entityP);
	// bool IsSameItem(int objectiveIndexP, UObject* entityP);
	// bool IsSameEventType(int objectiveIndexP, EAQ_NotifyEventType eventTypeP);
	//
	// /* Requirements */
	// void CheckIfRequirementsMet();
};

