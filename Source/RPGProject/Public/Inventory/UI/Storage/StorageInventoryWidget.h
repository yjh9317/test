// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/Data/ItemDataStructs.h"
#include "Interface/WidgetSlotListener.h"
#include "StorageInventoryWidget.generated.h"

class UStorageSlotWidget;
enum class EInventoryPanel;
class UInventoryComponent;
class UInventoryCoreComponent;
class UScrollBox;
class UWidgetSwitcher;
class UUniformGridPanel;
class UImage;
class UBorder;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UStorageInventoryWidget : public UUserWidget, public IWidgetSlotListener
{
	GENERATED_BODY()
	
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
public:
	UStorageInventoryWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void PlayConstructAnimation();
	
	void RefreshInventory(EInventoryPanel InPanel);
	void SetOwnerInventory();
	// void ResetSlotFocus();
	void CreateInventoryPanel(EInventoryPanel InPanel);
	// Interface
	virtual void OnSlotFocused(UUserWidget* InSlotWidget) override;
	virtual void OnSlotUnfocused(UUserWidget* InSlotWidget) override;
	virtual void OnSlotHovered(UUserWidget* InSlotWidget) override;
	virtual void OnSlotUnhovered(UUserWidget* InSlotWidget) override;
	virtual void OnSlotClicked(UUserWidget* InSlotWidget) override;
	virtual void OnSlotPressed(UUserWidget* InSlotWidget, FKey PressedKey) override;
	virtual void OnSlotReleased(UUserWidget* InSlotWidget, FKey ReleasedKey) override;

	// Fuctions
	void SetFocusToSlot(int32 InSlotIndex);
	void HighlightSlot(int32 InSlotIndex);
	void BuildInventorySlots(const TArray<FItemData>& Inventory,int32 Size,UUniformGridPanel* UniformGrid);
	EInventoryPanel GetActivePanel();
	UUniformGridPanel* GetUniformGridFromPanel(EInventoryPanel Panel);
	void SwitchInventoryPanel(EInventoryPanel NewPanel = EInventoryPanel::P1);
	void AssignCurrentlyFocusedSlot(UUserWidget* InSlot);
	void DisplaySampleSlots(int32 InSlots);
	void ClickedItemSlot(UUserWidget* InSlotWidget);
	void FocusItemSlot(UUserWidget* InSlotWidget);
	void UnfocusItemSlot(UUserWidget* InSlotWidget);
	void HoverItemSlot(UUserWidget* InSlotWidget);
	void UnhoverItemSlot(UUserWidget* InSlotWidget);
	void PressedItemSlot(UUserWidget* InSlotWidget,FKey PressedKey);
	void ReleasedItemSlot(UUserWidget* InSlotWidget,FKey PressedKey);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	TSubclassOf<UStorageSlotWidget> StorageSlotWidgetClass;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UBorder> Border_Background;

	UPROPERTY(BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UImage> Image_Background;

	UPROPERTY(BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UUniformGridPanel> UniformGridPanel_Inventory;

	UPROPERTY(BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> Switcher_Panel;

	UPROPERTY(BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox;

	UPROPERTY( BlueprintReadOnly, Category="Component")
	TObjectPtr<UInventoryCoreComponent> OwnerInventory;

	UPROPERTY( BlueprintReadOnly, Category="Component")
	TObjectPtr<UInventoryComponent> PlayerInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default")
	int32 ColumnPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default")
	int32 RowPosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default")
	int32 RowLength;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default")
	int32 FocusedSlot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	int32 SampleSlotsToDisplay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	FVector2D DraggedImageSize{100.f, 100.f};

	FDelegateHandle RefreshInventoryHandle;
	FDelegateHandle HighlightInventorySlotHandle;
	FDelegateHandle SwitchedActivePanelHandle;
};
