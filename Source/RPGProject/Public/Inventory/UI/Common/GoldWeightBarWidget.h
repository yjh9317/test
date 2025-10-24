// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GoldWeightBarWidget.generated.h"

class UInventoryComponent;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UGoldWeightBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	
	UPROPERTY( BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UImage> Image_Gold;

	UPROPERTY( BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UImage> Image_Weight;

	UPROPERTY( BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_CurrentWeight;

	UPROPERTY( BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_CurrentGold;

	UPROPERTY( BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_WeightSpacer;

	UPROPERTY( BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_MaxWeight;
	

private:
	void SetOwnerGold();
	void SetCurrentWeight();
	void SetMaxWeight();

	UPROPERTY(BlueprintReadOnly, Category="Component", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInventoryComponent> PlayerInventory;
	
	FDelegateHandle SetOwnerGoldHandle;
	FDelegateHandle SetCurrentWeightHandle;
	FDelegateHandle SetMaxWeightHandle;
};
