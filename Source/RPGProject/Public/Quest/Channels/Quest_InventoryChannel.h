// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Quest_InventoryChannel.generated.h"

enum class EQuestNotifyEventType : uint8;

UENUM(BlueprintType)
enum class EQuestInventoryEventType : uint8
{
	Collect					UMETA(DisplayName = "Collect"),
	RemoveFromInventory		UMETA(DisplayName = "Remove From Inventory"),
};

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnInventoryEventDelegate, UObject* entity, EQuestNotifyEventType eventType, float amount);


/**
 * 
 */
UCLASS()
class RPGPROJECT_API UQuest_InventoryChannel : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Advanced Quest | Events")
	void OnInventoryEventNotify(EQuestInventoryEventType eventType, UObject* entity, float amount = 1);

	FOnInventoryEventDelegate OnInventoryEventDelegate;
};
