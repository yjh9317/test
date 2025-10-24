// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/Data/ItemDataStructs.h"
#include "TradingSplitStackWidget.generated.h"

class USlider;
class USizeBox;
class UTextBlock;
class UProgressBar;
class UButton;
class UImage;
class UInventoryComponent;
class UInventoryCoreComponent;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAccepted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCancel);

UCLASS()
class RPGPROJECT_API UTradingSplitStackWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION()
	void OnPressAccept();
	UFUNCTION()
	void OnPressCancel();

	UFUNCTION()
	void OnSliderValueChanged();
	
	UFUNCTION()
	void SetInputIconAsync();

	UFUNCTION()
	void ShowSplitStackWidget(const FItemData& InItemData);

	float GetSliderValue() const;
	FText GetItemQuantity() const;
	FString SetCurrentQuantityText();
	void SetCurrentWeight();
	void SetCurrentValue();
	TSoftObjectPtr<UTexture2D> GetInputIconAccept();
	TSoftObjectPtr<UTexture2D> GetInputIconCancel();
	int32 GetCurrentQuantity() const;
	void ModifyCurrentQuantity(int32 ValueToAdd);
	
	UPROPERTY( BlueprintReadOnly, Category="Component")
	TObjectPtr<UInventoryCoreComponent> Sender;

	UPROPERTY( BlueprintReadOnly, Category="Component")
	TObjectPtr<UInventoryCoreComponent> Receiver;

	UPROPERTY( BlueprintReadOnly, Category="Component")
	TObjectPtr<UInventoryComponent> PlayerInventory;

	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<UImage> Image_Background;

	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<UButton> Button_Accept;
	
	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<UButton> Button_Cancel;

	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<UImage> Image_FullScreen;

	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<UImage> Image_InputAccept;

	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<UImage> Image_InputCancel;

	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<UProgressBar> ProgressBar_Quantity;

	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<UTextBlock> TextBlock_Quantity;

	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<USizeBox> SizeBox_Accept;

	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<USizeBox> SizeBox_Cancel;

	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<USlider> Slider;

	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<UUserWidget> ValueWidget;

	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<UUserWidget> WeightWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	FItemData TradingItemData;

	UPROPERTY( BlueprintReadOnly, Category="Variables")
	float SliderValue;

	UPROPERTY( BlueprintReadOnly, Category="Variables")
	FString EditableText;
};
