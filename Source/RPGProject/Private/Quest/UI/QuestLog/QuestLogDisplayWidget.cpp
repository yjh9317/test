// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/UI/QuestLog/QuestLogDisplayWidget.h"

#include "Components/HorizontalBox.h"
#include "Components/Image.h"
#include "Components/Spacer.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Item/ItemBase.h"
#include "Quest/RPGQuest.h"

UQuestLogDisplayWidget::UQuestLogDisplayWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UQuestLogDisplayWidget::AddObjectiveTexts(const FQuestObjectives& InObjective)
{
	UTextBlock* NewTextBlock = NewObject<UTextBlock>();
	if(NewTextBlock)
	{
		NewTextBlock->SetFont(ObjectivesFont);

		FSlateColor TextSlateColor{};
		TextSlateColor = FSlateColor(FLinearColor{0.007499f,0.01096f,0.01096f,1.f});
		NewTextBlock->SetColorAndOpacity(TextSlateColor);

		SetObjectiveAmountText(InObjective,NewTextBlock);
		VerticalBox_Objectives->AddChild(NewTextBlock);
		NewTextBlock->SetAutoWrapText(true);
	}
}

void UQuestLogDisplayWidget::UpdateObjectiveText(URPGQuest* InQuest)
{
	const TArray<FQuestObjectives>& Objectives = InQuest->QuestData->Objectives;
	const TArray<UWidget*>& ObjectivesChildren = VerticalBox_Objectives->GetAllChildren();
	for (int i = 0; i < Objectives.Num(); ++i)
	{
		if(ObjectivesChildren.IsValidIndex(i))
		{
			UTextBlock* ObjectivesTextBlock = Cast<UTextBlock>(ObjectivesChildren[i]);
			if(ObjectivesTextBlock)
			{
				SetObjectiveAmountText(Objectives[i],ObjectivesTextBlock);
			}
		}
	}
	
}

void UQuestLogDisplayWidget::SetObjectiveAmountText(const FQuestObjectives& CurrentObjective, UTextBlock* InTextBlock)
{
	if(!InTextBlock) return;
	
	if(CurrentObjective.AmountNeeded == 0)
	{
		InTextBlock->SetText(FText::FromString(CurrentObjective.ObjectiveSummary));
	}
	else
	{
		int32 CurrentAmount = CurrentObjective.CurrentAmount;
		int32 AmountNeeded = CurrentObjective.AmountNeeded;
		if (CurrentAmount > AmountNeeded)
		{
			CurrentAmount = CurrentObjective.AmountNeeded;
		}
		
		FString NewString = CurrentObjective.ObjectiveSummary;
		NewString += TEXT(":");
		NewString += FText::AsNumber(CurrentAmount).ToString();
		NewString += TEXT("/");
		NewString += FText::AsNumber(AmountNeeded).ToString();
		NewString += TEXT(":");
		
		InTextBlock->SetText(FText::FromString(NewString));
	}
}


void UQuestLogDisplayWidget::DisplayQuest(URPGQuest* InQuest)
{
	if(!InQuest) return;
	
	VerticalBox_Objectives->ClearChildren();

	// Quest Description
	TextBlock_QuestDescription->SetText(FText::FromString(InQuest->QuestData->QuestDescription));
	// Quest Title
	TextBlock_QuestTitle->SetText(FText::FromString(InQuest->QuestData->QuestTitle));
	// Quest Summary
	for(const auto& Objective : InQuest->QuestData->Objectives)
	{
		AddObjectiveTexts(Objective);
	}
	// Item Rewards
	if(InQuest->QuestData->ItemsReward.IsEmpty())
	{
		TextBlock_ItemRewards->SetVisibility(ESlateVisibility::Collapsed);
		HorizontalBox_Rewards->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		AddImages(InQuest);
		TextBlock_ItemRewards->SetVisibility(ESlateVisibility::Visible);
		HorizontalBox_Rewards->SetVisibility(ESlateVisibility::Visible);
	}
	// Quest Rewards
	FString NewString = TEXT("Gold:");
	NewString += FString::FromInt(InQuest->QuestData->GoldReward);
	TextBlock_Gold->SetText(FText::FromString(NewString));
	
	NewString.Empty();
	NewString = TEXT("Experience:");
	NewString += FString::FromInt(InQuest->QuestData->XpReward);
	TextBlock_Experience->SetText(FText::FromString(NewString));
	// Set the quest display to visible
	VerticalBox_QuestDisplay->SetVisibility(ESlateVisibility::Visible);
	SetVisibility(ESlateVisibility::Visible);
}

void UQuestLogDisplayWidget::AddImages(URPGQuest* InQuest)
{
	HorizontalBox_Rewards->ClearChildren();
	TArray<UItemBase*> TempKeyArray{}; 
	InQuest->QuestData->ItemsReward.GenerateKeyArray(TempKeyArray);

	for(const auto& ItemBase : TempKeyArray)
	{
		if(ItemBase->Image)
		{
			UTexture2D* ItemImage = ItemBase->Image.Get();
			UImage* NewImage = NewObject<UImage>();
			if(NewImage)
			{
				FSlateBrush NewSlateBrush;
				NewSlateBrush.ImageSize.X = ItemSpriteSize;
				NewSlateBrush.ImageSize.Y = ItemSpriteSize;
				NewSlateBrush.SetResourceObject(ItemImage);
				NewImage->SetBrush(NewSlateBrush);
				HorizontalBox_Rewards->AddChild(NewImage);

				USpacer* NewSpacer = NewObject<USpacer>();
				HorizontalBox_Rewards->AddChild(NewSpacer);
				NewSpacer->SetSize({10.f,0.f});
			}
		}
	}
}
