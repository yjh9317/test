// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/Data/ItemDataStructs.h"
#include "CraftingListSlotWidgetBase.generated.h"

class UBaseCraftingStation;
class UBaseCraftingComponent;
class UStationsManager;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UCraftingListSlotWidgetBase : public UUserWidget
{
	GENERATED_BODY()
public:
	UCraftingListSlotWidgetBase(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION()
	void AttemptSelectingSlot();

	UFUNCTION()
	void EventSelectSlot();

	UFUNCTION()
	void EventUnselectSlot();

	void SetItemData();

	UPROPERTY(BlueprintReadOnly,Category="Component")
	TObjectPtr<UStationsManager> StationsManager;

	UPROPERTY(BlueprintReadOnly,Category="Component")
	TObjectPtr<UBaseCraftingStation> BaseCraftingComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Default")
	FCraftingData CraftingData;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Default")
	FItemData ItemData;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Default")
	int32 SlotIndex;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Reference")
	TObjectPtr<UUserWidget> ParentWidgetRef;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Reference")
	bool bIsSelected;
};
