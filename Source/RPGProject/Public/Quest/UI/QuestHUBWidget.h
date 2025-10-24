// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestHUBWidget.generated.h"

class URPGQuest;
//class IAQ_PlayerChannelsFacade;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQuestEnableDelegate, URPGQuest*, InQuest);
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UQuestHUBWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	/* Widgets */
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Quest | UI")
	UUserWidget* QuestLogWidget = nullptr;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Quest | UI")
	UUserWidget* QuestGiverWidget = nullptr;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Quest | UI")
	void UpdateQuestWidgets(URPGQuest* quest);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Quest | UI")
	void DisplayQuestGiverSummary(const TArray<URPGQuest*>& questsToDisplay);
	
	UFUNCTION(BlueprintCallable, Category = "Quest | UI")
	void OpenQuestLog();

	UFUNCTION(BlueprintCallable, Category = "Quest | UI")
	void OpenQuestGiverSummary();

	UFUNCTION(BlueprintCallable, Category = "Quest | UI")
	void CloseAll();


	/* Quests */
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Quest | UI")
	TArray<URPGQuest*> Quests;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Quest | UI")
	void AddQuest(URPGQuest* quest);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Quest | UI")
	void RemoveQuest(URPGQuest* quest);


	/* References */
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI")
	APlayerController* PlayerController = nullptr;


	/* Load Save */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Quest | Save-Load")
	void OnLoadQuests(URPGQuest* quest);


	/* Delegates */
	UPROPERTY(BlueprintAssignable, Category = "Quest | Events")
	FOnQuestEnableDelegate OnQuestEnableDelegate;

	UFUNCTION(BlueprintCallable, Category = "Quest | Events")
	void OnQuestEnableBroadcast(URPGQuest* quest);
};
