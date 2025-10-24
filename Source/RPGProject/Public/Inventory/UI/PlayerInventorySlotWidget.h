// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Engine/StreamableManager.h"
#include "Item/ItemBase.h"
#include "PlayerInventorySlotWidget.generated.h"

class UItemToolTipWidget;
class UItemDataDragDropOperation;
class UItemDragAndDropWidget;
class UEquipmentComponent;
class UInventoryCoreComponent;
class UButton;
class UImage;
class UBorder;
class UTextBlock;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UPlayerInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:
    // Public API for parent widget to control this slot
    void RefreshSlotDisplay();
    void HighlightSlot();
    void EmptySlot();
    void CreateItemTooltip();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Animation")
    void PlayHighlightAnimation();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Animation")
    void StopHighlightAnimation();

    // Input properties to be set on creation
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
    TObjectPtr<UInventoryCoreComponent> ReceiverInventory;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
    TObjectPtr<UInventoryCoreComponent> PlayerInventory;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
    TObjectPtr<UEquipmentComponent> PlayerEquipment;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
    TObjectPtr<UUserWidget> ParentWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
    FItemData ItemData;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
    int32 SlotIndex;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="State", meta=(ExposeOnSpawn="true"))
    bool bItemInTrade = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="State", meta=(ExposeOnSpawn="true"))
    bool bCanDrop;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config", meta=(AllowPrivateAccess="true"))
    FVector2D DraggedImageSize{100.f, 100.f};
    
protected:
    // Engine Overrides
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
    virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
    virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    // Delegate Handlers
    UFUNCTION()
    void OnItemButtonClicked();

    UFUNCTION()
    void OnItemButtonHovered();

    UFUNCTION()
    void OnItemButtonUnhovered();

    // Bound Widget Components
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UBorder> Border;
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UImage> Image_Item;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UImage> Image_ItemInTrade;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UButton> Button_Item;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UTextBlock> TextBlock_Quantity;

private:
    // Internal Helper Functions
    void SetItemImageAsync();
    void OnRightClick();
    void InitializeWidgetReferences();
    void BindEventHandlers();
    void DraggedFromEquipment(const UItemDataDragDropOperation* DragDrop);
    void DraggedFromInventory(const UItemDataDragDropOperation* DragDrop);
    void DraggedFromOtherInventory(const UItemDataDragDropOperation* DragDrop);
    void ShowItemComparison();
    void HideItemComparison();
    bool AttemptSplitting(UItemDataDragDropOperation* InDragDropOperation);
    void RefreshToolTip();
    void SetItemQuantity();
    bool IsAnyPopupActive();
    void SetInTradeIcon();

    // Internal State & Handles
    TSharedPtr<FStreamableHandle> ImageLoadHandle;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Widget", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UItemToolTipWidget> ItemToolTipWidget;

    // Configuration Properties
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class", meta=(AllowPrivateAccess="true"))
    TSubclassOf<UItemDragAndDropWidget> ItemDragAndDropWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class", meta=(AllowPrivateAccess="true"))
    TSubclassOf<UItemDataDragDropOperation> ItemDataDragDropOperationClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class", meta=(AllowPrivateAccess="true"))
    TSubclassOf<UUserWidget> ItemToolTipWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config", meta=(AllowPrivateAccess="true"))
    FLinearColor UnHoveredBorderColor;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config", meta=(AllowPrivateAccess="true"))
    FLinearColor HoveredBorderColor;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config", meta=(AllowPrivateAccess="true"))
    FLinearColor BorderGreenColor = FLinearColor{0.116971f, 0.48515f, 0.08022f, 1.f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config", meta=(AllowPrivateAccess="true"))
    FLinearColor BorderRedColor = FLinearColor{0.737911f, 0.f, 0.028426f, 1.f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config", meta=(AllowPrivateAccess="true"))
    UTexture2D* ButtonBackgroundImage;
};
