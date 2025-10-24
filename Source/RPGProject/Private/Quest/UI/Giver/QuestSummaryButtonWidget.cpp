// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/UI/Giver/QuestSummaryButtonWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Header/DebugHeader.h"
#include "Quest/RPGQuest.h"

UQuestSummaryButtonWidget::UQuestSummaryButtonWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UQuestSummaryButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(Quest && Quest->QuestState == EQuestState::Valid)
	{
		TextBlock_Specifier->SetText(FText::FromString(TEXT("(Terminate)")));
	}

	Button_Quest->OnClicked.AddDynamic(this,&UQuestSummaryButtonWidget::QuestButtonClicked);
	Button_Quest->OnHovered.AddDynamic(this,&UQuestSummaryButtonWidget::QuestButtonHovered);
	Button_Quest->OnUnhovered.AddDynamic(this,&UQuestSummaryButtonWidget::QuestButtonUnhovered);
}

void UQuestSummaryButtonWidget::QuestButtonClicked()
{
	if(OnQuestSelected.IsBound())
	{
		OnQuestSelected.Execute(this);
	}
	else
	{
		DebugHeader::Log(TEXT("QuestButtonClicked : OnQuestSelected is not bound"));
	}
}

void UQuestSummaryButtonWidget::QuestButtonHovered()
{
	TextBlock_QuestTitle->SetColorAndOpacity(SelectedColor);
}

void UQuestSummaryButtonWidget::QuestButtonUnhovered()
{
	TextBlock_QuestTitle->SetColorAndOpacity(NormalColor);
}
