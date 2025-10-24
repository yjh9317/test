// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/UI/QuestLog/QuestLogWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "Quest/UI/QuestLog/QuestListWidget.h"
#include "Quest/UI/QuestLog/QuestLogButtonWidget.h"
#include "Quest/UI/QuestLog/QuestLogDisplayWidget.h"
#include "Quest/UI/QuestTracker/QuestTrackerWidget.h"

UQuestLogWidget::UQuestLogWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

void UQuestLogWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(FilterPerZone)
	{
		UQuestListWidget* NewQuestListWidget = NewObject<UQuestListWidget>();
		ScrollBox->AddChild(NewQuestListWidget);

		ZoneFilters.AddUnique(NewQuestListWidget);
		NewQuestListWidget->HorizontalBox_ZoneFilter->SetVisibility(ESlateVisibility::Hidden);
	}

	if(Button_Abandon)
	{
		Button_Abandon->OnClicked.AddDynamic(this,&UQuestLogWidget::AbandonButtonClicked);
	}
}

void UQuestLogWidget::DisplayQuest(UQuestLogButtonWidget* InQuestLogButtonWidget)
{
	if(!InQuestLogButtonWidget || !QuestLogDisplayWidget) return;

	InQuestLogButtonWidget->OnQuestLogButtonSelected.BindLambda(
	[this](UQuestLogButtonWidget* SelectedButton)
		{
			QuestButtonSelected = SelectedButton;

			for(const auto& Button : QuestButtons)
			{
				if(Button == SelectedButton)
				{
					QuestLogDisplayWidget->DisplayQuest(Button->Quest);
				}
				else
				{
					Button->OnDeselect();
				}
			}
		});

	Button_Abandon->SetVisibility(ESlateVisibility::Visible);
}

void UQuestLogWidget::QuickDisplay(UQuestLogButtonWidget* InQuestLogButtonWidget)
{
	if(!InQuestLogButtonWidget || !QuestTrackerWidget) return;
	
	InQuestLogButtonWidget->OnQuestFollowed.BindLambda(
	[this](UQuestLogButtonWidget* SelectedButton)
		{
			URPGQuest* CurrentQuest = SelectedButton->Quest;
			if(!CurrentQuest) return;
		
			if(CurrentQuest->bIsDisplayQuestTracker)
			{
				// Remove Quest from Quest Tracker Display
				QuestTrackerWidget->RemoveQuestDisplay(CurrentQuest->IndexQuestTracker);
				CurrentQuest->bIsDisplayQuestTracker = false;
			}
			else
			{
				// Add Quest to Quest Tracker Display
				if(QuestTrackerWidget->CanDisplay())
				{
					QuestTrackerWidget->AddQuestDisplay(CurrentQuest);
				}
			}
		}
	);
}

void UQuestLogWidget::AddQuestButton(URPGQuest* InQuest)
{
	if(FilterPerZone)
	{
		if(UQuestListWidget* QuestList = CheckZoneFilters(InQuest))
		{
			CreateButton(QuestList,InQuest);
		}
		else
		{
			if(ZoneFilters.IsValidIndex(0))
			{
				CreateButton(ZoneFilters[0],InQuest);
			}
		}
	}
}

void UQuestLogWidget::RemoveQuestButton(URPGQuest* InQuest)
{
	RemoveButton(InQuest);
	if(FilterPerZone)
	{
		UpdateFilters();
	}
}

void UQuestLogWidget::OnQuestUpdateEvent(URPGQuest* InQuest)
{
	QuestLogDisplayWidget->UpdateObjectiveText(InQuest);
}

UQuestListWidget* UQuestLogWidget::CheckZoneFilters(URPGQuest* InQuest)
{
	if(!ScrollBox || !InQuest) return nullptr;

	FString QuestLoc = InQuest->QuestData->QuestLocation;
	// Check if the zone filter already exist
	for(const auto& QuestList : ZoneFilters)
	{
		UTextBlock* ZoneTitleTextBlock = QuestList->TextBlock_ZoneTitle;
		if(ZoneTitleTextBlock)
		{
			if(ZoneTitleTextBlock->GetText().ToString() == QuestLoc)
			{
				return QuestList;
			}
		}
	}

	// If not, we add it to the ScrollBox
	UQuestListWidget* NewQuestListWidget = NewObject<UQuestListWidget>();
	if(NewQuestListWidget)
	{
		ScrollBox->AddChild(NewQuestListWidget);
		ZoneFilters.AddUnique(NewQuestListWidget);
		NewQuestListWidget->TextBlock_ZoneTitle->SetText(FText::FromString(QuestLoc));
		return NewQuestListWidget;
	}
	
	return nullptr;
}

void UQuestLogWidget::UpdateFilters()
{
	for(const auto& QuestList : ZoneFilters)
	{
		if(QuestList->VerticalBox_QuestList->GetChildrenCount() == 0)
		{
			QuestList->RemoveFromParent();
			ZoneFilters.Remove(QuestList);
		}
	}
}

void UQuestLogWidget::CreateButton(UQuestListWidget* InQuestList, URPGQuest* InQuest)
{
	UQuestLogButtonWidget* NewQuestLogButtonWidget = InQuestList->CreateButton(InQuest);
	if(NewQuestLogButtonWidget)
	{
		QuestButtons.Add(NewQuestLogButtonWidget);
		DisplayQuest(NewQuestLogButtonWidget);
		QuickDisplay(NewQuestLogButtonWidget);
	}
}

void UQuestLogWidget::RemoveButton(URPGQuest* InQuest)
{
	for(const auto& QuestLogButton : QuestButtons)
	{
		URPGQuest* LinkedQuest = QuestLogButton->Quest; 
		if(!LinkedQuest) continue;

		if(LinkedQuest == InQuest)
		{
			// If the quest was Displayed, we hide the display & button
			if(QuestLogButton->IsSelected)
			{
				QuestLogDisplayWidget->VerticalBox_QuestDisplay->SetVisibility(ESlateVisibility::Hidden);
				Button_Abandon->SetVisibility(ESlateVisibility::Hidden);
			}
			// Remove the button from the Quest List Box & Unbind all events from the delegates
			QuestLogButton->RemoveFromParent();
			QuestLogButton->OnQuestLogButtonSelected.Unbind();
			QuestLogButton->OnQuestFollowed.Unbind();

			// Remove also from the Quest Buttons
			QuestButtons.Remove(QuestLogButton);
		}
	}
}

void UQuestLogWidget::RefreshQuestButtons(URPGQuest* InQuest)
{
	for(const auto& QuestLogButton : QuestButtons)
	{
		if(QuestLogButton->Quest == InQuest)
		{
			QuestLogButton->UpdateSpecifier();
			return;
		}
	}
}

void UQuestLogWidget::AbandonButtonClicked()
{
	QuestButtonSelected->Quest->ResetQuest();
}

void UQuestLogWidget::ExitButtonClicked()
{
	SetVisibility(ESlateVisibility::Hidden);
	if(GetWorld())
	{
		APlayerController* LocalPC = UGameplayStatics::GetPlayerController(GetWorld(),0);
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(LocalPC,false);
		LocalPC->bShowMouseCursor = false;
		LocalPC->SetIgnoreMoveInput(false);
	}
}
