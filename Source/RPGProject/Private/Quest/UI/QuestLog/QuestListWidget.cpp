// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/UI/QuestLog/QuestListWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Quest/Data/RPGQuestData.h"
#include "Quest/UI/QuestLog/QuestLogButtonWidget.h"

UQuestListWidget::UQuestListWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	
}

void UQuestListWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if (Button_Zone)
	{
		Button_Zone->OnClicked.AddDynamic(this, &UQuestListWidget::ZoneButtonClick);
	}
}

UQuestLogButtonWidget* UQuestListWidget::CreateButton(URPGQuest* InQuest)
{
	UQuestLogButtonWidget* NewQuestLogButtonWidget = NewObject<UQuestLogButtonWidget>();
	if(NewQuestLogButtonWidget)
	{
		NewQuestLogButtonWidget->Quest = InQuest;
		VerticalBox_QuestList->AddChild(NewQuestLogButtonWidget);
		
		NewQuestLogButtonWidget->TextBlock_QuestTitle->SetText(FText::FromString(InQuest->QuestData->QuestTitle));
		NewQuestLogButtonWidget->SetPadding({0.f,5.f,0.f,0.f});
		NewQuestLogButtonWidget->UpdateSpecifier();
	}
	return nullptr;
}

void UQuestListWidget::SetTextZones(const FText& InText)
{
	TextBlock_ZoneTitle->SetText(InText);
}


void UQuestListWidget::ZoneButtonClick()
{
	ESlateVisibility CurrentVisibility = VerticalBox_QuestList->GetVisibility();

	if (CurrentVisibility == ESlateVisibility::Visible)
	{
		VerticalBox_QuestList->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		VerticalBox_QuestList->SetVisibility(ESlateVisibility::Visible);
	}
}
