// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/UI/Giver/QuestMarkerWidget.h"

#include "Components/Image.h"

void UQuestMarkerWidget::SetImageQuest(bool isEnded, EQuestType questType)
{
	UTexture2D* MarkerTexture = nullptr;
	
	if(isEnded)
	{
		switch (questType)
		{
		case EQuestType::EQuest_MainQuest:
			{
				MarkerTexture = MainQuestAvailableIcon;
			}
			break;
		case EQuestType::EQuest_SubQuest:
			{
				MarkerTexture = MainQuestCompleteIcon;
			}
			break;
		}
		
		FSlateBrush MarkerBrush;
		MarkerBrush.SetResourceObject(MarkerTexture);
		MarkerBrush.ImageSize.X = ImageSize;
		MarkerBrush.ImageSize.Y = ImageSize;
		Image_Marker->SetBrush(MarkerBrush);
	}
	else
	{
		switch (questType)
		{
		case EQuestType::EQuest_MainQuest:
			{
				MarkerTexture = SubQuestAvailableIcon;
			}
			break;
		case EQuestType::EQuest_SubQuest:
			{
				MarkerTexture = SubQuestCompleteIcon;
			}
			break;
		}
		
		FSlateBrush MarkerBrush;
		MarkerBrush.SetResourceObject(MarkerTexture);
		MarkerBrush.ImageSize.X = ImageSize;
		MarkerBrush.ImageSize.Y = ImageSize;
		Image_Marker->SetBrush(MarkerBrush);
	}
}
