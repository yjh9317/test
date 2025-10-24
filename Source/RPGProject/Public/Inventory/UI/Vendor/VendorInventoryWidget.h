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
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void PlayConstructAnimation();
    
    //~ Begin IWidgetSlotListener Interface
    virtual void OnSlotReleased(UUserWidget* InSlotWidget, FKey ReleasedKey) override;
    virtual void OnSlotPressed(UUserWidget* InSlotWidget, FKey PressedKey) override;
    virtual void OnSlotUnhovered(UUserWidget* InSlotWidget) override;
    virtual void OnSlotHovered(UUserWidget* InSlotWidget) override;
    virtual void OnSlotUnfocused(UUserWidget* InSlotWidget) override;
    virtual void OnSlotFocused(UUserWidget* InSlotWidget) override;
    virtual void OnSlotClicked(UUserWidget* InSlotWidget) override;
    //~ End IWidgetSlotListener Interface

protected:
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

    void RefreshInventory(EInventoryPanel InPanel);
    void HighlightSlot(int32 InSlotIndex);
    void SwitchInventoryPanel(EInventoryPanel NewPanel);

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UBorder> Border_Background;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UImage> Image_Background;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UImage> Image_Gold;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UUniformGridPanel> UniformGridPanel_Inventory;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UWidgetSwitcher> PanelSwitcher;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UScrollBox> ScrollBox;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UTextBlock> TextBlock_Gold;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UWidgetSwitcher> Switcher_Panel;

private:
    void SetOwnerInventory();
    void CreateInventoryPanel(EInventoryPanel InPanel);
    void SetFocusToSlot(int32 InSlotIndex);
    void BuildInventorySlots(const TArray<FItemData>& Inventory, int32 Size, UUniformGridPanel* UniformGrid);
    EInventoryPanel GetActivePanel();
    UUniformGridPanel* GetUniformGridFromPanel(EInventoryPanel Panel);
    void AssignCurrentlyFocusedSlot(UUserWidget* InSlot);
    void DisplaySampleSlots(int32 InSlots);

    // Interface Implementation Details
    void ClickedItemSlot(UUserWidget* InSlotWidget);
    void FocusItemSlot(UUserWidget* InSlotWidget);
    void UnfocusItemSlot(UUserWidget* InSlotWidget);
    void HoverItemSlot(UUserWidget* InSlotWidget);
    void UnhoverItemSlot(UUserWidget* InSlotWidget);
    void PressedItemSlot(UUserWidget* InSlotWidget, FKey PressedKey);
    void ReleasedItemSlot(UUserWidget* InSlotWidget, FKey PressedKey);
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config", meta=(AllowPrivateAccess="true"))
    TSubclassOf<UVendorSlotWidget> VendorSlotWidgetClass;
    
    UPROPERTY(BlueprintReadOnly, Category="Component", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UInventoryCoreComponent> OwnerInventory;

    UPROPERTY(BlueprintReadOnly, Category="Component", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UInventoryCoreComponent> PlayerInventory;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config", meta=(AllowPrivateAccess="true"))
    int32 RowPosition;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config", meta=(AllowPrivateAccess="true"))
    int32 ColumnPosition;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config", meta=(AllowPrivateAccess="true"))
    int32 RowLength;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State", meta=(AllowPrivateAccess="true"))
    int32 FocusedSlot;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config", meta=(AllowPrivateAccess="true"))
    int32 SampleSlotsToDisplay = 30;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config", meta=(AllowPrivateAccess="true"))
    FVector2D DraggedImageSize{100, 100};

    FDelegateHandle RefreshInventoryHandle;
    FDelegateHandle HighlightInventorySlotHandle;
    FDelegateHandle SwitchedActivePanelHandle;
};
