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
	
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void PlayHighlightAniamtion();

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void StopHighlightAniamtion();
	
	UFUNCTION()
	void CreateItemTooltip();

	UFUNCTION()
	void ShowItemComparison();

	UFUNCTION()
	void HideItemComparison();
	
	void SetItemImageAsync();
	void SetItemQuantity();
	void EmptySlot();
	void HighlightSlot();
	void RefreshToolTip();
	bool IsAnyPopupActive();
	
	UPROPERTY( BlueprintReadOnly, Category="Component")
	TObjectPtr<UInventoryCoreComponent> PlayerInventory;

	UPROPERTY( BlueprintReadOnly, Category="Component")
	TObjectPtr<UInventoryCoreComponent> ActorInventory;

	UPROPERTY( BlueprintReadOnly, Category="Reference")
	TObjectPtr<UUserWidget> ParentWidgetRef;

	UPROPERTY( BlueprintReadOnly, Category="Default")
	FVector2D DraggedImageSize = FVector2D{100.f, 100.f};

	UPROPERTY( BlueprintReadOnly, Category="Default")
	int32 SlotIndex;
	
	UPROPERTY( BlueprintReadOnly, Category="Default")
	FItemData ItemData;
protected:

	UPROPERTY(BlueprintReadOnly, Category="Widget")
	TObjectPtr<UItemToolTipWidget> ItemToolTipWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Class")
	TSubclassOf<UItemDragAndDropWidget> ItemDragAndDropWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class")
	TSubclassOf<UItemDataDragDropOperation> ItemDataDragDropOperationClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Class")
	TSubclassOf<UItemToolTipWidget> ItemToolTipWidgetClass;
	

	UPROPERTY( BlueprintReadOnly, Category="Default") // meta=(BindWidget)
	TObjectPtr<UBorder> Border;

	UPROPERTY( BlueprintReadOnly, Category="Default") // meta=(BindWidget)
	TObjectPtr<UImage> Image_Item;

	UPROPERTY( BlueprintReadOnly, Category="Default") // meta=(BindWidget)
	TObjectPtr<UButton> Button_Item;

	UPROPERTY( BlueprintReadOnly, Category = "Default") // ,meta=(BindWidget)
	TObjectPtr<UTextBlock> TextBlock_Quantity;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	FLinearColor UnHoveredBorderColor = FLinearColor{0.558341f, 0.258183f, 0.f, 1.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	FLinearColor HoveredBorderColor = FLinearColor{0.558341f, 0.258183f, 0.f, 1.f};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	FLinearColor BorderGreenColor = FLinearColor{0.116971f,0.48515f,0.08022f,1.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	FLinearColor BorderRedColor = FLinearColor{0.737911f,0.f,0.028426f,1.f};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	FLinearColor AlreadyUsedColor = FLinearColor{0.223228f, 0.219526f, 0.212231f, 1.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	FLinearColor NotUsedColor = FLinearColor{1.f, 1.f, 1.f, 1.f};

};
