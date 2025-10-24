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
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Payload", meta=(ExposeOnSpawn="true"))
	TObjectPtr<UInventoryCoreComponent> InventoryComponent;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Payload", meta=(ExposeOnSpawn="true"))
	FItemData ItemData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Payload", meta=(ExposeOnSpawn="true"))
	EItemDestination DraggedFrom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Payload", meta=(ExposeOnSpawn="true"))
	TObjectPtr<UItemDragAndDropWidget> DraggableWidget;

private:
	UPROPERTY(EditDefaultsOnly, Category="Config", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UItemDragAndDropWidget> DraggableWidgetClass;
};
