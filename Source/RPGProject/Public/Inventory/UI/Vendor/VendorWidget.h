// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "VendorWidget.generated.h"

class UWidgetSwitcher;
class UVendorInventoryWidget;
class UPlayerInventoryWidget;
class UEquipmentWidget;
class UImage;
class UInventoryComponent;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UVendorWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UPROPERTY(BlueprintReadOnly, Category="Component")
	TObjectPtr<UInventoryComponent> PlayerInventory;
 
	UPROPERTY(BlueprintReadOnly, Category="Default") // meta=(BindWidget)
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	UPROPERTY(BlueprintReadOnly, Category="Default") // meta=(BindWidget)
	TObjectPtr<UImage> Image_Background;

	UPROPERTY(BlueprintReadOnly, Category="Default") // meta=(BindWidget)
	TObjectPtr<UEquipmentWidget> EquipmentWidget;

	UPROPERTY(BlueprintReadOnly, Category="Default") // meta=(BindWidget)
	TObjectPtr<UUserWidget> GoldWeightBarWidget;

	UPROPERTY(BlueprintReadOnly, Category="Default") // meta=(BindWidget)
	TObjectPtr<UPlayerInventoryWidget> PlayerInventoryWidget;

	UPROPERTY(BlueprintReadOnly, Category="Default") // meta=(BindWidget)
	TObjectPtr<UUserWidget> UpperUIBarWidget;

	UPROPERTY(BlueprintReadOnly, Category="Default") // meta=(BindWidget)
	TObjectPtr<UVendorInventoryWidget> VendorInventoryWidget;
	
	
};
