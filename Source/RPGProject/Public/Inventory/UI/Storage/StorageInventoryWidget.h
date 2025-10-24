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
	
public:
    UStorageInventoryWidget(const FObjectInitializer& ObjectInitializer);

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void PlayConstructAnimation();
    
    //~ Begin IWidgetSlotListener Interface
    virtual void OnSlotFocused(UUserWidget* InSlotWidget) override;
    virtual void OnSlotUnfocused(UUserWidget* InSlotWidget) override;
    virtual void OnSlotHovered(UUserWidget* InSlotWidget) override;
    virtual void OnSlotUnhovered(UUserWidget* InSlotWidget) override;
    virtual void OnSlotClicked(UUserWidget* InSlotWidget) override;
    virtual void OnSlotPressed(UUserWidget* InSlotWidget, FKey PressedKey) override;
    virtual void OnSlotReleased(UUserWidget* InSlotWidget, FKey ReleasedKey) override;
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
    TObjectPtr<UUniformGridPanel> UniformGridPanel_Inventory;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UWidgetSwitcher> Switcher_Panel;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UScrollBox> ScrollBox;

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
    TSubclassOf<UStorageSlotWidget> StorageSlotWidgetClass;

    UPROPERTY(BlueprintReadOnly, Category="Component", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UInventoryCoreComponent> OwnerInventory;

    UPROPERTY(BlueprintReadOnly, Category="Component", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UInventoryComponent> PlayerInventory;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State", meta=(AllowPrivateAccess="true"))
    int32 ColumnPosition;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State", meta=(AllowPrivateAccess="true"))
    int32 RowPosition;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config", meta=(AllowPrivateAccess="true"))
    int32 RowLength;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State", meta=(AllowPrivateAccess="true"))
    int32 FocusedSlot;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config", meta=(AllowPrivateAccess="true"))
    int32 SampleSlotsToDisplay;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config", meta=(AllowPrivateAccess="true"))
    FVector2D DraggedImageSize{100.f, 100.f};

    FDelegateHandle RefreshInventoryHandle;
    FDelegateHandle HighlightInventorySlotHandle;
    FDelegateHandle SwitchedActivePanelHandle;
};
