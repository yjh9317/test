// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/Data/ItemDataStructs.h"
#include "CraftingListSlotWidgetBase.generated.h"

class UStationManagerComponent;
class UBaseCraftingStation;
class UBaseCraftingComponent;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UCraftingListSlotWidgetBase : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UCraftingListSlotWidgetBase(const FObjectInitializer& ObjectInitializer);

	//~ Begin Public API
	void AttemptSelectingSlot();
	void EventSelectSlot();
	void EventUnselectSlot();
	//~ End Public API

	//~ Begin Input Properties (Set on Creation)
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
	TObjectPtr<UStationManagerComponent> StationManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
	TObjectPtr<UBaseCraftingStation> BaseCraftingComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
	FCraftingData CraftingData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
	int32 SlotIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
	TObjectPtr<UUserWidget> ParentWidgetRef;
	//~ End Input Properties

	//~ Begin Public State
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="State")
	bool bIsSelected;
	//~ End Public State

private:
	//~ Begin Internal Helper Functions
	void SetItemData();
	//~ End Internal Helper Functions

	//~ Begin Internal State
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State", meta=(AllowPrivateAccess="true"))
	FItemData ItemData;
	//~ End Internal State
};
