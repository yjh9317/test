// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Quest/Data/RPGQuestData.h"
#include "Quest/Data/QuestStruct.h"
#include "QuestTrackerDisplayWidget.generated.h"

class URPGQuest;
class UVerticalBox;
class UQuestTrackerObjectivesWidget;
class UTextBlock;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UQuestTrackerDisplayWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UQuestTrackerDisplayWidget(const FObjectInitializer& ObjectInitializer);

	void SetDisplay(URPGQuest* InQuest);
	void UpdateObjectivesDisplay(const URPGQuestData& InQuestData);
	void ObjectiveToText(const FQuestObjectives& InQuestObjective,UQuestTrackerObjectivesWidget* InQuestTrackerObjective);

public:
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI")
	TObjectPtr<UTextBlock> QuestTitle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variable")
	FSlateFontInfo TitleFont;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variable")
	FSlateFontInfo ObjectivesFont;

	UPROPERTY(VisibleAnywhere)
	TArray<UQuestTrackerObjectivesWidget*> QuestObjectives;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variable")
	FLinearColor TitleColor;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_Quest;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variable")
	TObjectPtr<URPGQuest> Quest;

protected:

};
