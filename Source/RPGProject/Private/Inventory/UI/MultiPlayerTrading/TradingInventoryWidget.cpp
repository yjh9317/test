// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/MultiPlayerTrading/TradingInventoryWidget.h"

void UTradingInventoryWidget::OnSlotPressed(UUserWidget* SlotWidget, FKey PressedKey)
{
	Super::OnSlotPressed(SlotWidget, PressedKey);
}

void UTradingInventoryWidget::OnSlotReleased(UUserWidget* SlotWidget, FKey ReleasedKey)
{
	Super::OnSlotReleased(SlotWidget, ReleasedKey);
}

void UTradingInventoryWidget::OnSlotClicked(UUserWidget* SlotWidget)
{
	Super::OnSlotClicked(SlotWidget);
}

void UTradingInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UTradingInventoryWidget::BuildInventorySlots(const TArray<FItemData>& Inventory, int32 Size,
	UUniformGridPanel* UniformGridPanel)
{
	Super::BuildInventorySlots(Inventory, Size, UniformGridPanel);
}

void UTradingInventoryWidget::RefreshAllPanels()
{
}
