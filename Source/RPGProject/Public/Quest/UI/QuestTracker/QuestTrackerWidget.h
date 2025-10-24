// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Quest/Data/QuestStruct.h"
#include "QuestTrackerWidget.generated.h"

class URPGQuest;
class UVerticalBox;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UQuestTrackerWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UQuestTrackerWidget(const FObjectInitializer& ObjectInitializer);

	bool CanDisplay();
	void AddQuestDisplay(URPGQuest* InQuest);
	void RemoveQuestDisplay(int32 InQuestIndex);
	void OnQuestUpdate(int32 InQuestIndex,const URPGQuestData& InQuestData);
	
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_QuestTracker;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Variable")
	int32 MaxQuestDisplayable = 5;


protected:
	virtual void NativeConstruct() override;
};

