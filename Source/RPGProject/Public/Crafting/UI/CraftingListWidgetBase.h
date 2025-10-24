// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/WidgetSlotListener.h"
#include "Item/Data/ItemDataStructs.h"
#include "CraftingListWidgetBase.generated.h"

class UBaseCraftingComponent;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UCraftingListWidgetBase : public UUserWidget, public IWidgetSlotListener
{
	GENERATED_BODY()

public:
	UCraftingListWidgetBase(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION()
	void InitializeWidget();

	UFUNCTION()
	void CreateCraftingList();

	UFUNCTION()
	void ScrolltoSlot();

	UFUNCTION()
	void SelectFirstItemOnTheList();

	UFUNCTION()
	void SwitchList();

	virtual void OnSlotClicked(UUserWidget* SlotWidget) override {};
	virtual void OnSlotFocused(UUserWidget* SlotWidget) override {};
	virtual void OnSlotUnfocused(UUserWidget* SlotWidget) override {};
	virtual void OnSlotHovered(UUserWidget* SlotWidget) override {};
	virtual void OnSlotUnhovered(UUserWidget* SlotWidget) override {};
};
