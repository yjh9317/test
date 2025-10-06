// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/Data/ItemDataStructs.h"
#include "LootBarSlotWidget.generated.h"

class ULootBarWidget;
class UTextBlock;
class UImage;
class UButton;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API ULootBarSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
	void SetItemDetails();
	
	void SetItemName();
	void SetItemType();
	void SetItemRarity();
	void SetItemRarityColor();
	void SetItemQuantity();

	UFUNCTION()
	void OnRightClick();

	UFUNCTION()
	void EventMouseMoved();

	UFUNCTION()
	void EventButtonBackgroundHover();

	UFUNCTION(BlueprintImplementableEvent)
	void PlayAnimationBorder();
	
	UFUNCTION(BlueprintImplementableEvent)
   	void StopAnimationBorder();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default",meta=(BindWidget))
	TObjectPtr<UButton> Button_Background;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default",meta=(BindWidget))
	TObjectPtr<UImage> Image_Border;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default",meta=(BindWidget))
	TObjectPtr<UImage> Image_Item;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default",meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Name;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default",meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Quantity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default",meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Rarity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default",meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Type;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	FItemData ItemData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Default")
	TSubclassOf<ULootBarWidget> LootBarClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	TObjectPtr<ULootBarWidget> LootBarWidget;
};
