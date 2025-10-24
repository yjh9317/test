// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FocusWidgetHandler.generated.h"

UENUM(BlueprintType)
enum class EWidgetType : uint8
{
	NONE,
	INVENTORY,
	EQUIPMENT,
	CRAFTING,
	VENDOR,
	STORAGE,
	LOADGAME,
	ABILITIES
};


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFocusWidgetHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGPROJECT_API IFocusWidgetHandler
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual EWidgetType GetCurrentlyFocusedWidget() { return EWidgetType::NONE; }
	virtual void SetCurrentlyFocusedWidget(EWidgetType Type) {}
};
