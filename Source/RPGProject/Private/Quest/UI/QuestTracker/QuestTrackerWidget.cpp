// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/UI/QuestTracker/QuestTrackerWidget.h"

#include "Components/VerticalBox.h"
#include "Quest/RPGQuest.h"
#include "Quest/UI/QuestTracker/QuestTrackerDisplayWidget.h"

UQuestTrackerWidget::UQuestTrackerWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UQuestTrackerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
}

bool UQuestTrackerWidget::CanDisplay()
{
	return VerticalBox_QuestTracker->GetChildrenCount() < MaxQuestDisplayable;
}

void UQuestTrackerWidget::AddQuestDisplay(URPGQuest* InQuest)
{
	if(!InQuest) return;
	
	UQuestTrackerDisplayWidget* NewQuestTrackerDisplay = NewObject<UQuestTrackerDisplayWidget>();

	if(NewQuestTrackerDisplay)
	{
		VerticalBox_QuestTracker->AddChild(NewQuestTrackerDisplay);
		NewQuestTrackerDisplay->SetDisplay(InQuest);
		InQuest->IndexQuestTracker = VerticalBox_QuestTracker->GetChildIndex(VerticalBox_QuestTracker);
		InQuest->bIsDisplayQuestTracker = true;
	}
}

void UQuestTrackerWidget::RemoveQuestDisplay(int32 InQuestIndex)
{
	UWidget* ChildWidget = VerticalBox_QuestTracker->GetChildAt(InQuestIndex);
	if(ChildWidget)
	{
		ChildWidget->RemoveFromParent();
	}

	// Update Displayed Index
	const TArray<UWidget*>& ChildWidgetArray = VerticalBox_QuestTracker->GetAllChildren();

	for (int i = 0; i < ChildWidgetArray.Num(); ++i)
	{
		if(UQuestTrackerDisplayWidget* QuestTrackerDisplay = Cast<UQuestTrackerDisplayWidget>(ChildWidgetArray[i]))
		{
			if(QuestTrackerDisplay->Quest)
			{
				QuestTrackerDisplay->Quest->IndexQuestTracker = i;
			}
		}
	}
}

void UQuestTrackerWidget::OnQuestUpdate(int32 InQuestIndex,const URPGQuestData& InQuestData)
{
	if(UWidget* ChildWidget = VerticalBox_QuestTracker->GetChildAt(InQuestIndex))
	{
		UQuestTrackerDisplayWidget* QuestTrackerDisplay = Cast<UQuestTrackerDisplayWidget>(ChildWidget);
		if(QuestTrackerDisplay)
		{
			QuestTrackerDisplay->UpdateObjectivesDisplay(InQuestData);
		}
	}
}
