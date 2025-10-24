// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/WidgetManager.h"
#include "StorageWidget.generated.h"

class UInventoryComponent;
class UStorageInventoryWidget;
class UWidgetSwitcher;
class UEquipmentWidget;
class UPlayerInventoryWidget;
class UImage;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UStorageWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION()
	void OnSwitchTab(EWidgetType WidgetType);
	
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void PlayConstructAnimation();
	

	UPROPERTY( BlueprintReadOnly, Category="Component")
	TObjectPtr<UInventoryComponent> PlayerInventory;
	
	UPROPERTY( BlueprintReadOnly, Category = "Widget") // ,meta=(BindWidget)
	TObjectPtr<UImage> Image_Background;

	UPROPERTY( BlueprintReadOnly, Category = "Widget") // ,meta=(BindWidget)
	TObjectPtr<UPlayerInventoryWidget> PlayerInventoryWidget;

	UPROPERTY( BlueprintReadOnly, Category = "Widget") // ,meta=(BindWidget)
	TObjectPtr<UEquipmentWidget> EquipmentWidget;

	UPROPERTY( BlueprintReadOnly, Category = "Widget") // ,meta=(BindWidget)
	TObjectPtr<UWidgetSwitcher> Switcher;

	UPROPERTY( BlueprintReadOnly, Category = "Widget") // ,meta=(BindWidget)
	TObjectPtr<UStorageInventoryWidget> StorageInventoryWidget;

	UPROPERTY( BlueprintReadOnly, Category = "Widget") // ,meta=(BindWidget)
	TObjectPtr<UUserWidget> GoldWeightBarWidget;

	UPROPERTY( BlueprintReadOnly, Category = "Widget") // ,meta=(BindWidget)
	TObjectPtr<UUserWidget> KeyBindingsSwitcher;

	UPROPERTY( BlueprintReadOnly, Category = "Widget") // ,meta=(BindWidget)
	TObjectPtr<UUserWidget> UpperUIBarWidget;
};
