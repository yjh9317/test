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
private:
	TSharedPtr<FStreamableHandle> ImageLoadHandle;
	
public:
	void RefreshSlotDisplay();
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	void SetItemImageAsync();
	void CreateItemTooltip();
	void OnRightClick();

	void InitializeWidgetReferences();
	void BindEventHandlers();

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable,Category = "Animation")
	void PlayHighlightAnimation();

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable,Category = "Animation")
	void StopHighlightAnimation();
	
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	// virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	// virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	
	// OnDrop
	void DraggedFromEquipment(const UItemDataDragDropOperation* DragDrop);
	void DraggedFromInventory(const UItemDataDragDropOperation* DragDrop);
	void DraggedFromOtherInventory(const UItemDataDragDropOperation* DragDrop);
	
	// Hide
	void ShowItemComparison();
	void HideItemComparison();
	void EmptySlot();
	bool AttemptSplitting(UItemDataDragDropOperation* InDragDropOperation); // DragDropOperation을 상속받은 클래스 만들기
	void HighlightSlot();
	void RefreshToolTip();
	void SetItemQuantity();
	bool IsAnyPopupActive();
	void SetInTradeIcon();

	// Bind Function
	UFUNCTION()
	void OnItemButtonClicked();

	UFUNCTION()
	void OnItemButtonHovered();

	UFUNCTION()
	void OnItemButtonUnhovered();

	//Component
	UPROPERTY(BlueprintReadOnly,Category="Component")
	TObjectPtr<UInventoryCoreComponent> ReceiverInventory;

	UPROPERTY(BlueprintReadOnly,Category="Component")
	TObjectPtr<UInventoryCoreComponent> PlayerInventory;

	UPROPERTY(BlueprintReadOnly,Category="Component")
	TObjectPtr<UEquipmentComponent> PlayerEquipment;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Class")
	TSubclassOf<UItemDragAndDropWidget> ItemDragAndDropWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Class")
	TSubclassOf<UItemDataDragDropOperation> ItemDataDragDropOperationClass;
	
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category = "Widget")
	TObjectPtr<UBorder> Border;
	
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category = "Widget")
	TObjectPtr<UImage> Image_Item;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category = "Widget")
	TObjectPtr<UImage> Image_ItemInTrade;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category = "Widget")
	TObjectPtr<UButton> Button_Item;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category = "Widget")
	TObjectPtr<UTextBlock> TextBlock_Quantity;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Class")
	TSubclassOf<UUserWidget> ItemToolTipWidgetClass;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Reference")
	TObjectPtr<UUserWidget> ParentWidget;
	
	// ToolTipWidget Binding
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default")
	FItemData ItemData;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Widget")
	TObjectPtr<UItemToolTipWidget> ItemToolTipWidget;

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly,Category="Default")
	FLinearColor UnHoveredBorderColor;

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly ,Category="Default")
	FLinearColor HoveredBorderColor;
	
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly,Category="Default")
	FVector2D DraggedImageSize{100.f,100.f};

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly,Category="Default")
	int32 SlotIndex;

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly,Category="Default")
	bool bItemInTrade = false;

	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly,Category="Default")
	bool bCanDrop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	FLinearColor BorderGreenColor = FLinearColor{0.116971f,0.48515f,0.08022f,1.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	FLinearColor BorderRedColor = FLinearColor{0.737911f,0.f,0.028426f,1.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Texture")
	UTexture2D* ButtonBackgroundImage;
};
