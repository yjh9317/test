// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/Data/ItemDataStructs.h"
#include "CraftingMaterialSlotBase.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UCraftingMaterialSlotBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UCraftingMaterialSlotBase(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Default")
	FItemData ItemData;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Default")
	int32 CurrentQuantity;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Default")
	int32 SlotIndex;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Reference")
	TObjectPtr<UUserWidget> ParentWidgetRef;
};
