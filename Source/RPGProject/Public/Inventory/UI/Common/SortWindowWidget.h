// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/Data/InventoryDatas.h"
#include "SortWindowWidget.generated.h"

class UInventoryComponent;
class USortSlotWidget;
/**
 * 
 */
DECLARE_MULTICAST_DELEGATE(FOnEndSorting);

UCLASS()
class RPGPROJECT_API USortWindowWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FOnEndSorting OnEndSorting;
	
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void EventOnSort(ESortMethod SortMethod);

	UPROPERTY( BlueprintReadOnly, Category="Component")
	TObjectPtr<UInventoryComponent> PlayerInventory;
	
	UPROPERTY( BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<USortSlotWidget> SortButton_Type;
	
	UPROPERTY( BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<USortSlotWidget> SortButton_Rarity;

	UPROPERTY( BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<USortSlotWidget> SortButton_Value;

	UPROPERTY( BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<USortSlotWidget> SortButton_Weight;

	FDelegateHandle SortButtonTypeHandle;
	FDelegateHandle SortButtonRarityHandle;
	FDelegateHandle SortButtonValueHandle;
	FDelegateHandle SortButtonWeightHandle;
};

