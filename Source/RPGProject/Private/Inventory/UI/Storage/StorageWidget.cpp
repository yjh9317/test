// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/Storage/StorageWidget.h"

#include "Components/WidgetSwitcher.h"
#include "Interface/ComponentManager.h"
#include "Inventory/Components/InventoryComponent.h"
#include "Inventory/UI/PlayerInventoryWidget.h"
#include "Inventory/UI/Storage/StorageInventoryWidget.h"
#include "Kismet/KismetInputLibrary.h"

void UStorageWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ForceLayoutPrepass();

	APlayerController* LocalPC = GetOwningPlayer();
	if(LocalPC)
	{
		IComponentManager* ComponentManager = Cast<IComponentManager>(LocalPC);
		if(ComponentManager)
		{
			PlayerInventory = ComponentManager->GetInventoryComponent();
			// UWidgetManager* WidgetManager = ComponentManager->GetWidgetManagerComponent();
			// if(WidgetManager)
			// {
			// 	WidgetManager;
			// }
		}
		
	}
}

void UStorageWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

FReply UStorageWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey InputKey = UKismetInputLibrary::GetKey(InKeyEvent);
	if(InputKey == EKeys::F && PlayerInventory)
	{
		PlayerInventory->Server_SortInventory(PlayerInventory,ESortMethod::Quicksort,PlayerInventory->ActivePanel,false);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UStorageWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	if(PlayerInventoryWidget)
	{
		PlayerInventoryWidget->SetKeyboardFocus();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UStorageWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FVector2D MouseCursorDelta = UKismetInputLibrary::PointerEvent_GetCursorDelta(InMouseEvent);
	if(MouseCursorDelta.X != 0 || MouseCursorDelta.Y !=0)
	{
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UStorageWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Handled();
}

void UStorageWidget::OnSwitchTab(EWidgetType WidgetType)
{
	
	if(WidgetType == EWidgetType::INVENTORY)
	{
		if(PlayerInventoryWidget)
		{
			Switcher->SetActiveWidget(PlayerInventoryWidget);
		}
	}
	else if (WidgetType == EWidgetType::STORAGE)
	{
		if(StorageInventoryWidget)
		{
			 Switcher->SetActiveWidget(StorageInventoryWidget);
		}
	}
	
	
}
