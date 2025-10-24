// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/UI/QuestLog/QuestLogButtonWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Quest/UI/QuestLog/QuestListWidget.h"

UQuestLogButtonWidget::UQuestLogButtonWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
		
}

void UQuestLogButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(Quest)
	{
		UpdateFollowButtonStyle(Quest->bIsDisplayQuestTracker);
	}
	if(Button_Quest)
	{
		Button_Quest->OnClicked.AddDynamic(this,&UQuestLogButtonWidget::QuestButtonClicked);
		Button_Quest->OnHovered.AddDynamic(this,&UQuestLogButtonWidget::QuestButtonHovered);
	}
}

void UQuestLogButtonWidget::OnDeselect()
{
	Button_Quest->SetStyle(NormalStyle);
	TextBlock_QuestTitle->SetColorAndOpacity(NormalColor);

	IsSelected = false;
	Quest->bIsDisplayQuestLog = false;
}

void UQuestLogButtonWidget::UpdateFollowButtonStyle(bool IsFollowed)
{
	if(IsFollowed)
	{
		Button_Follow->SetStyle(FillShieldStyle);
		IsQuestFollowed = true;
	}
	else
	{
		Button_Follow->SetStyle(EmptyShieldStyle);
		IsQuestFollowed = false;
	}
}

void UQuestLogButtonWidget::UpdateSpecifier()
{
	if(Quest && Quest->QuestState == EQuestState::Valid)
	{
		TextBlock_Specifier->SetText(FText::FromString("(Complete)"));
	}
	else
	{
		TextBlock_Specifier->SetText(FText::FromString("(Failed)"));
	}
}

void UQuestLogButtonWidget::QuestButtonClicked()
{
	if(!IsSelected)
	{
		Button_Quest->SetStyle(SelectedStyle);
		IsSelected = true;
		Quest->bIsDisplayQuestLog = true;

		if(OnQuestLogButtonSelected.IsBound())
		{
			OnQuestLogButtonSelected.Execute(this);
		}
		else
		{
			UE_LOG(LogTemp,Warning,TEXT("QuestButtonClicked : OnQuestLogButtonSelected is not bound"));
		}
	}
}

void UQuestLogButtonWidget::QuestButtonHovered()
{
	TextBlock_QuestTitle->SetColorAndOpacity(SelectedColor);
}

void UQuestLogButtonWidget::QuestButtonUnhovered()
{
	TextBlock_QuestTitle->SetColorAndOpacity(NormalColor);
}

void UQuestLogButtonWidget::FollowButtonClicked()
{
	if(OnQuestFollowed.IsBound())
	{
		OnQuestFollowed.Execute(this);
	}
	else
	{
		UE_LOG(LogTemp,Warning,TEXT("FollowButtonClicked : OnQuestFollowed is not bound"));
	}

	UpdateFollowButtonStyle(Quest->bIsDisplayQuestTracker);
}



