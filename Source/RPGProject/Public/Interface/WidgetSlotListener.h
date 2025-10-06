// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "WidgetSlotListener.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWidgetSlotListener : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGPROJECT_API IWidgetSlotListener
{
	GENERATED_BODY()

public:
	virtual void OnSlotClicked(UUserWidget* SlotWidget){}
	virtual void OnSlotFocused(UUserWidget* SlotWidget){}
	virtual void OnSlotUnfocused(UUserWidget* SlotWidget){}
	virtual void OnSlotHovered(UUserWidget* SlotWidget){}
	virtual void OnSlotUnhovered(UUserWidget* SlotWidget){}
	virtual void OnSlotPressed(UUserWidget* SlotWidget,FKey PressedKey){}
	virtual void OnSlotReleased(UUserWidget* SlotWidget, FKey ReleasedKey) {}
};
