// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/Data/ItemDataStructs.h"
#include "VendorSlotWidget.generated.h"

class UTextBlock;
class UItemDragAndDropWidget;
class UItemDataDragDropOperation;
class UItemToolTipWidget;
class UButton;
class UImage;
class UBorder;
class UInventoryCoreComponent;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UVendorSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void PlayHighlightAniamtion();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void StopHighlightAniamtion();
    
    void CreateItemTooltip();
    void EmptySlot();
    void HighlightSlot();

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
    TObjectPtr<UInventoryCoreComponent> PlayerInventory;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
    TObjectPtr<UInventoryCoreComponent> ActorInventory;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
    TObjectPtr<UUserWidget> ParentWidgetRef;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
    FVector2D DraggedImageSize = FVector2D{100.f, 100.f};

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
    int32 SlotIndex;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
    FItemData ItemData;

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
    virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
    virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

    UFUNCTION()
    void OnSlotRightClicked();
    
    UFUNCTION()
    void OnSlotHovered();

    UFUNCTION()
    void OnSlotUnhovered();

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UBorder> Border;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UImage> Image_Item;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UButton> Button_Item;

    UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
    TObjectPtr<UTextBlock> TextBlock_Quantity;

private:
    UFUNCTION()
    void ShowItemComparison();

    UFUNCTION()
    void HideItemComparison();
    
    void SetItemImageAsync();
    void SetItemQuantity();
    void RefreshToolTip();
    bool IsAnyPopupActive();

    UPROPERTY(BlueprintReadOnly, Category="Widget", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UItemToolTipWidget> ItemToolTipWidget;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class", meta=(AllowPrivateAccess="true"))
    TSubclassOf<UItemDragAndDropWidget> ItemDragAndDropWidgetClass;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class", meta=(AllowPrivateAccess="true"))
    TSubclassOf<UItemDataDragDropOperation> ItemDataDragDropOperationClass;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class", meta=(AllowPrivateAccess="true"))
    TSubclassOf<UItemToolTipWidget> ItemToolTipWidgetClass;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default", meta=(AllowPrivateAccess="true"))
    FLinearColor UnHoveredBorderColor = FLinearColor{0.558341f, 0.258183f, 0.f, 1.f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default", meta=(AllowPrivateAccess="true"))
    FLinearColor HoveredBorderColor = FLinearColor{0.558341f, 0.258183f, 0.f, 1.f};
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default", meta=(AllowPrivateAccess="true"))
    FLinearColor BorderGreenColor = FLinearColor{0.116971f, 0.48515f, 0.08022f, 1.f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default", meta=(AllowPrivateAccess="true"))
    FLinearColor BorderRedColor = FLinearColor{0.737911f, 0.f, 0.028426f, 1.f};
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default", meta=(AllowPrivateAccess="true"))
    FLinearColor AlreadyUsedColor = FLinearColor{0.223228f, 0.219526f, 0.212231f, 1.f};

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default", meta=(AllowPrivateAccess="true"))
    FLinearColor NotUsedColor = FLinearColor{1.f, 1.f, 1.f, 1.f};

};
