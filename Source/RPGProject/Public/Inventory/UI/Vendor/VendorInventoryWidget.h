// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interface/WidgetSlotListener.h"
#include "Item/Data/ItemDataStructs.h"
#include "VendorInventoryWidget.generated.h"

class UVendorSlotWidget;
class UInventoryCoreComponent;
class UBorder;
class UImage;
class UUniformGridPanel;
class UWidgetSwitcher;
class UScrollBox;
class UTextBlock;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UVendorInventoryWidget : public UUserWidget,public IWidgetSlotListener
{
	GENERATED_BODY()

public:

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void PlayConstructAnimation();
	
	void RefreshInventory(EInventoryPanel InPanel);
	void SetOwnerInventory();
	// void ResetSlotFocus();
	void CreateInventoryPanel(EInventoryPanel InPanel);
	
	// Interface
	virtual void OnSlotReleased(UUserWidget* InSlotWidget, FKey ReleasedKey) override;
	virtual void OnSlotPressed(UUserWidget* InSlotWidget, FKey PressedKey) override;
	virtual void OnSlotUnhovered(UUserWidget* InSlotWidget) override;
	virtual void OnSlotHovered(UUserWidget* InSlotWidget) override;
	virtual void OnSlotUnfocused(UUserWidget* InSlotWidget) override;
	virtual void OnSlotFocused(UUserWidget* InSlotWidget) override;
	virtual void OnSlotClicked(UUserWidget* InSlotWidget) override;

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
	TSubclassOf<UVendorSlotWidget> VendorSlotWidgetClass;
	
	UPROPERTY( BlueprintReadOnly, Category="Component")
	TObjectPtr<UInventoryCoreComponent> OwnerInventory;

	UPROPERTY( BlueprintReadOnly, Category="Component")
	TObjectPtr<UInventoryCoreComponent> PlayerInventory;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Default") // ,meta=(BindWidget)
	TObjectPtr<UBorder> Border_Background;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Default") // ,meta=(BindWidget)
	TObjectPtr<UImage> Image_Background;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Default") // ,meta=(BindWidget)
	TObjectPtr<UImage> Image_Gold;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Default") // ,meta=(BindWidget)
	TObjectPtr<UUniformGridPanel> UniformGridPanel_Inventory;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Default") // ,meta=(BindWidget)
	TObjectPtr<UWidgetSwitcher> PanelSwitcher;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Default") // ,meta=(BindWidget)
	TObjectPtr<UScrollBox> ScrollBox;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Default") // ,meta=(BindWidget)
	TObjectPtr<UTextBlock> TextBlock_Gold;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget") // ,meta=(BindWidget)
	TObjectPtr<UWidgetSwitcher> Switcher_Panel;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	int32 RowPosition;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	int32 ColumnPosition;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	int32 RowLength;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	int32 FocusedSlot;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	int32 SampleSlotsToDisplay = 30;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FVector2D DraggedImageSize{100,100};

	FDelegateHandle RefreshInventoryHandle;
	FDelegateHandle HighlightInventorySlotHandle;
	FDelegateHandle SwitchedActivePanelHandle;
};
