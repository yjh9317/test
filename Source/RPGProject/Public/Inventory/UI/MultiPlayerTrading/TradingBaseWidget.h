// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TradingBaseWidget.generated.h"

class UPlayerInventorySlotWidget;
class UTextBlock;
class UScrollBox;
class UUniformGridPanel;
class UEditableText;
class UImage;
class UBorder;
class UTradingComponent;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UTradingBaseWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY( BlueprintReadOnly, Category="Component")
	TObjectPtr<UTradingComponent> TradingComponent;

	UPROPERTY( BlueprintReadOnly, Category="Widget") // meta=(BindWidget)
	TObjectPtr<UBorder> Border_Background;

	UPROPERTY( BlueprintReadOnly, Category="Widget") // meta=(BindWidget)
	TObjectPtr<UImage> Image_Background;

	UPROPERTY( BlueprintReadOnly, Category="Widget") // meta=(BindWidget)
	TObjectPtr<UBorder> Border_OfferSlots;

	UPROPERTY( BlueprintReadOnly, Category="Widget") // meta=(BindWidget)
	TObjectPtr<UEditableText> EditableText_Gold;

	UPROPERTY( BlueprintReadOnly, Category="Widget") // meta=(BindWidget)
	TObjectPtr<UUniformGridPanel> UniformGridPanel_ItemsGrid;

	UPROPERTY( BlueprintReadOnly, Category="Widget") // meta=(BindWidget)
	TObjectPtr<UImage> Image_Gold;

	UPROPERTY( BlueprintReadOnly, Category="Widget") // meta=(BindWidget)
	TObjectPtr<UScrollBox> ScrollBox;
	
	UPROPERTY( BlueprintReadOnly, Category="Widget") // meta=(BindWidget)
	TObjectPtr<UTextBlock> TextBlock_UpperText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	TSubclassOf<UPlayerInventorySlotWidget> PlayerInventorySlotWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	TObjectPtr<UPlayerInventorySlotWidget> PlayerInventorySlotWidget;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	int32 RowLength;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	FText UpperText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	FLinearColor DragOverBorderColor;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	FLinearColor DefaultBorderColor;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	bool bCanDragDrop;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	bool bGoldTextReadOnly;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	int32 FocusedSlotIndex;
};
