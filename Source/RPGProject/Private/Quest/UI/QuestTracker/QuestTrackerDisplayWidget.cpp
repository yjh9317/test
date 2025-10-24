// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/UI/QuestTracker/QuestTrackerDisplayWidget.h"

#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Quest/RPGQuest.h"
#include "Quest/UI/QuestTracker/QuestTrackerObjectivesWidget.h"

UQuestTrackerDisplayWidget::UQuestTrackerDisplayWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UQuestTrackerDisplayWidget::ObjectiveToText(const FQuestObjectives& InQuestObjective,
	UQuestTrackerObjectivesWidget* InQuestTrackerObjective)
{
	if(!InQuestTrackerObjective || !InQuestTrackerObjective->TextBlock_Objective) return;
	
	if(InQuestObjective.AmountNeeded == 0)
	{
		InQuestTrackerObjective->TextBlock_Objective->SetText(FText::FromString(InQuestObjective.ObjectiveSummary));
	}
	else
	{
		int32 CurrentAmount = InQuestObjective.CurrentAmount;
		int32 AmountNeeded = InQuestObjective.AmountNeeded;
		if (CurrentAmount > AmountNeeded)
		{
			CurrentAmount = InQuestObjective.AmountNeeded;
		}
		
		FString NewString = TEXT("-");
		NewString += FText::AsNumber(CurrentAmount).ToString();
		NewString += TEXT("/");
		NewString += FText::AsNumber(AmountNeeded).ToString();
		NewString += TEXT(" ");
		NewString += InQuestObjective.ObjectiveSummary;
		
		InQuestTrackerObjective->TextBlock_Objective->SetText(FText::FromString(NewString));

		// If this objective has been updated, trigger the OnUpdate Animation
		if(InQuestTrackerObjective->CurrentObjectiveAmount < CurrentAmount)
		{
			InQuestTrackerObjective->OnObjectiveUpdate();
		}
		InQuestTrackerObjective->CurrentObjectiveAmount = CurrentAmount;
	}
}

void UQuestTrackerDisplayWidget::SetDisplay(URPGQuest* InQuest)
{
	if(!InQuest) return;
	Quest = InQuest;

	// Set Quest Title Text
	UTextBlock* NewTextBlock = NewObject<UTextBlock>();
	if(NewTextBlock)
	{
		QuestTitle = NewTextBlock;
		QuestTitle->SetText(FText::FromString(InQuest->QuestData->QuestTitle));
		QuestTitle->SetFont(TitleFont);
		VerticalBox_Quest->AddChild(QuestTitle);
		QuestTitle->SetAutoWrapText(true);
		QuestTitle->SetColorAndOpacity(TitleColor);
	}
	// Set Quest Objectives Text
	const TArray<FQuestObjectives>& InQuestObjectives = InQuest->QuestData->Objectives;
	for (const auto& InQuestObjective : InQuestObjectives)
	{
		UQuestTrackerObjectivesWidget* NewQuestTrackerObjectives = NewObject<UQuestTrackerObjectivesWidget>();
		if(NewQuestTrackerObjectives)
		{
			VerticalBox_Quest->AddChild(NewQuestTrackerObjectives);
			ObjectiveToText(InQuestObjective,NewQuestTrackerObjectives);
			QuestObjectives.AddUnique(NewQuestTrackerObjectives);
		}
	}
}

void UQuestTrackerDisplayWidget::UpdateObjectivesDisplay(const URPGQuestData& InQuestData)
{
	for(int i=0;i<QuestObjectives.Num();++i)
	{
		if(InQuestData.Objectives.IsValidIndex(i))
		{
			ObjectiveToText(InQuestData.Objectives[i],QuestObjectives[i]);
		}
	}
}
