// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest/Data/QuestSpecialEventData.h"
#include "UObject/NoExportTypes.h"
#include "Quest/RPGQuest.h"
#include "QuestChannel.generated.h"

class UQuestSounds;
class UQuestComponent;
class UQuest_AudioChannel;
class UWidgetComponent;
class UUserWidget;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLevelRequirementChangedDelegate, int, PlayerLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQuestCompletionDelegate, URPGQuest*, Quest);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpecialEventTriggerDelegate, UQuestSpecialEventData*, SpecialEvent);

UCLASS()
class RPGPROJECT_API UQuestChannel : public UObject
{
	GENERATED_BODY()
public:
	/* Widget */
	TSubclassOf<UUserWidget> QuestWidgetsClass;
	void SetWidgetClass(TSubclassOf<UUserWidget> widgetClass) { QuestWidgetsClass = widgetClass;}
	void AddWidgetToViewport();
	
	// UFUNCTION(BlueprintCallable, Category = "Advanced Quest | Events")
	// void SetQuestWidget(UAQ_BookQuest* InQuestWidget) { QuestWidgets = InQuestWidget; }
	//
	// UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Advanced Quest | UI")
	// UAQ_BookQuest* QuestWidgets = nullptr;
	
	/* Events */
	void OnPlayerLevelChange(int newLevel);
	void OnSpecialEventTrigger(UQuestSpecialEventData* specialEvent);
	
	/* Delegates */
	UFUNCTION()
	void OnQuestStateChanged(URPGQuest* QuestUpdate, EQuestState QuestState);
	
	void PlayQuestEndSound(URPGQuest* InQuest);
	void PlayQuestStartSound(URPGQuest* InQuest);
	
	UFUNCTION()
	void OnQuestProgress(URPGQuest* QuestUpdate);
	
	UFUNCTION()
	void OnQuestRegression(URPGQuest* QuestUpdate);
	
	FLevelRequirementChangedDelegate LevelRequirementChangedDelegate;
	FSpecialEventTriggerDelegate SpecialEventTriggerDelegate;
	FQuestCompletionDelegate QuestCompletionDelegate;
	
	/* References */
	UQuest_AudioChannel* AudioChannel = nullptr;
	UQuestSounds* SoundBank = nullptr;	
};


