// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/UI/PlayerInventoryWidget.h"
#include "TradingInventoryWidget.generated.h"

/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemSlotClicked);

UCLASS()
class RPGPROJECT_API UTradingInventoryWidget : public UPlayerInventoryWidget
{
	GENERATED_BODY()
public:
	FOnItemSlotClicked OnItemSlotClicked;
	
	virtual void OnSlotPressed(UUserWidget* SlotWidget, FKey PressedKey) override;
	virtual void OnSlotReleased(UUserWidget* SlotWidget, FKey ReleasedKey) override;
	virtual void OnSlotClicked(UUserWidget* SlotWidget) override;
	virtual void NativeOnInitialized() override;
	virtual void BuildInventorySlots(const TArray<FItemData>& Inventory, int32 Size, UUniformGridPanel* UniformGridPanel) override;
	
	UFUNCTION()
	void RefreshAllPanels();
};
