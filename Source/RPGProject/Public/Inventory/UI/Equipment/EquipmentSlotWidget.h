// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/Data/ItemDataStructs.h"
#include "EquipmentSlotWidget.generated.h"

class UItemDataDragDropOperation;
class UItemDragAndDropWidget;
enum class EWidgetType : uint8;
class UItemToolTipWidget;
class UEquipmentWidget;
class UOverlay;
class UTextBlock;
class UButton;
class UImage;
class UInventoryComponent;
class UEquipmentComponent;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UEquipmentSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    void OverwriteSlot(UEquipmentWidget* InEquipmentWidget, const FItemData& InItemData);
    void EmptySlot();

protected:
    virtual void NativePreConstruct() override;
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

    UFUNCTION()
    void OnRightClick();
    
    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UImage> Image_Item;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UButton> Button_Item;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UImage> Image_Border;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UTextBlock> TextBlock_Label;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UOverlay> Overlay_Label;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UTextBlock> TextBlock_SlotName;

private:
    void SetItemQuantity();
    ESlateVisibility SetLabelVisibility();
    FText SetLabelCharacter() const;
    void RefreshToolTip();
    EWidgetType GetActiveWidget();
    void SetItemImageAsync(const FItemData& InItemData);

    UPROPERTY(BlueprintReadOnly, Category="Component", meta=(AllowPrivateAccess = "true"))
    TObjectPtr<UEquipmentComponent> PlayerEquipment;

    UPROPERTY(BlueprintReadOnly, Category="Component", meta=(AllowPrivateAccess = "true"))
    TObjectPtr<UInventoryComponent> PlayerInventory;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Class", meta=(AllowPrivateAccess = "true"))
    TSubclassOf<UItemDragAndDropWidget> ItemDragAndDropWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Class", meta=(AllowPrivateAccess = "true"))
    TSubclassOf<UItemDataDragDropOperation> ItemDataDragAndDropOperationClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Class", meta=(AllowPrivateAccess = "true"))
    TSubclassOf<UItemToolTipWidget> ItemToolTipWidgetClass;
    
    UPROPERTY(BlueprintReadOnly, Category="Default", meta=(AllowPrivateAccess = "true"))
    TObjectPtr<UEquipmentWidget> EquipmentWidget;

    UPROPERTY(BlueprintReadOnly, Category="Default", meta=(AllowPrivateAccess = "true"))
    TObjectPtr<UItemToolTipWidget> ItemToolTipWidget;

    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
    EItemSlot EquipmentSlot;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default", meta=(AllowPrivateAccess = "true"))
    FLinearColor UnHoveredBorderColor = FLinearColor{0.558341f, 0.258183f, 0.f, 1.f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default", meta=(AllowPrivateAccess = "true"))
    FLinearColor HoveredBorderColor = FLinearColor{0.558341f, 0.258183f, 0.f, 1.f};
    
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
    FSlateBrush EmptySlotStyle;
    
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
    FText EquipmentSlotName;
    
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite, meta=(AllowPrivateAccess = "true"))
    FText LabelCharText;

    FItemData ItemData;
};
