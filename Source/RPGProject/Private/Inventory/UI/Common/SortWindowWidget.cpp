// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/Common/SortWindowWidget.h"

#include "Interface/ComponentManager.h"
#include "Inventory/Components/InventoryComponent.h"
#include "Inventory/UI/Common/SortSlotWidget.h"

void USortWindowWidget::NativeConstruct()
{
	if(APlayerController* PC = GetOwningPlayer())
	{
		if(IComponentManager* ComponentManager = Cast<IComponentManager>(PC))
		{
			PlayerInventory = ComponentManager->GetInventoryComponent();
		}
	}

	SortButtonTypeHandle = SortButton_Type->OnSortButtonClicked.AddUObject(this,&USortWindowWidget::EventOnSort);
	SortButtonRarityHandle = SortButton_Rarity->OnSortButtonClicked.AddUObject(this,&USortWindowWidget::EventOnSort);
	SortButtonValueHandle = SortButton_Value->OnSortButtonClicked.AddUObject(this,&USortWindowWidget::EventOnSort);
	SortButtonWeightHandle = SortButton_Weight->OnSortButtonClicked.AddUObject(this,&USortWindowWidget::EventOnSort);
}

void USortWindowWidget::NativeDestruct()
{
	Super::NativeDestruct();

	SortButton_Type->OnSortButtonClicked.Remove(SortButtonTypeHandle);
	SortButton_Rarity->OnSortButtonClicked.Remove(SortButtonRarityHandle);
	SortButton_Value->OnSortButtonClicked.Remove(SortButtonValueHandle);
	SortButton_Weight->OnSortButtonClicked.Remove(SortButtonWeightHandle);
}


void USortWindowWidget::EventOnSort(ESortMethod SortMethod)
{
	if(PlayerInventory)
	{
		PlayerInventory->Server_SortInventory(PlayerInventory,SortMethod,PlayerInventory->ActivePanel,false);
		OnEndSorting.Broadcast();		
	}
}
