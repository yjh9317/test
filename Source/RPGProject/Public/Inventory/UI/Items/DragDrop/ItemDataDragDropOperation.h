// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Item/Data/ItemDataStructs.h"
#include "ItemDataDragDropOperation.generated.h"

class UItemDragAndDropWidget;
class UInventoryCoreComponent;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UItemDataDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	void ClearDraggableIcon();
	void ShowIconDrop();
	void ShowIconSwap();
	void ShowIconWrongSlot();
	
public:
	UPROPERTY(EditDefaultsOnly)
	UInventoryCoreComponent* InventoryComponent;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UItemDragAndDropWidget> DraggableWidgetClass;

	UPROPERTY()
	TObjectPtr<UItemDragAndDropWidget> DraggableWidget;

	UPROPERTY(EditDefaultsOnly)
	FItemData ItemData;

	UPROPERTY(EditDefaultsOnly)
	EItemDestination DraggedFrom;
};
