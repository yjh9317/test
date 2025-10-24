// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/Common/GoldWeightBarWidget.h"

#include "Components/TextBlock.h"
#include "Interface/ComponentManager.h"
#include "Inventory/Components/InventoryComponent.h"

void UGoldWeightBarWidget::NativeConstruct()
{
	if(GetOwningPlayer())
	{
		if(IComponentManager* ComponentManager = Cast<IComponentManager>(GetOwningPlayer()))
		{
			PlayerInventory = ComponentManager->GetInventoryComponent();
			if(PlayerInventory)
			{
				SetOwnerGold();
				SetCurrentWeight();
				SetMaxWeight();
				SetOwnerGoldHandle = PlayerInventory->OnOwnerGoldChanged.AddUObject(this,&UGoldWeightBarWidget::SetOwnerGold);
				SetCurrentWeightHandle = PlayerInventory->OnCurrentWeightChanged.AddUObject(this,&UGoldWeightBarWidget::SetCurrentWeight);
				SetMaxWeightHandle = PlayerInventory->OnMaxWeightChanged.AddUObject(this,&UGoldWeightBarWidget::SetMaxWeight);
			}
		}
	}
}

void UGoldWeightBarWidget::NativeDestruct()
{
	if (PlayerInventory)
	{
		PlayerInventory->OnOwnerGoldChanged.Remove(SetOwnerGoldHandle);
		PlayerInventory->OnCurrentWeightChanged.Remove(SetCurrentWeightHandle);
		PlayerInventory->OnMaxWeightChanged.Remove(SetMaxWeightHandle);
	}
}

void UGoldWeightBarWidget::SetOwnerGold()
{
	if(PlayerInventory)
	{
		TextBlock_CurrentGold->SetText(FText::AsNumber(PlayerInventory->OwnerGold));
	}
}

void UGoldWeightBarWidget::SetCurrentWeight()
{
	if(PlayerInventory)
	{
		TextBlock_CurrentWeight->SetText(FText::AsNumber(PlayerInventory->CurrentInventoryWeight));

		// Set Overweight Color
		if(PlayerInventory->IsInventoryOverweight())
		{
			FSlateColor OverWeightColor{FLinearColor{0.338542f,0.001763f,0.001763f,1.f}};
			TextBlock_CurrentWeight->SetColorAndOpacity(OverWeightColor);
			TextBlock_WeightSpacer->SetColorAndOpacity(OverWeightColor);
			TextBlock_MaxWeight->SetColorAndOpacity(OverWeightColor);
		}
		else
		{
			FSlateColor NotOverWeightColor{FLinearColor{1.f,1.f,1.f,1.f}};
			TextBlock_CurrentWeight->SetColorAndOpacity(NotOverWeightColor);
			TextBlock_WeightSpacer->SetColorAndOpacity(NotOverWeightColor);
			TextBlock_MaxWeight->SetColorAndOpacity(NotOverWeightColor);
		}
	}
}

void UGoldWeightBarWidget::SetMaxWeight()
{
	if(PlayerInventory)
	{
		TextBlock_MaxWeight->SetText(FText::AsNumber(PlayerInventory->MaxInventoryWeight));
	}
}
