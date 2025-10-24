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
	UFUNCTION(BlueprintImplementableEvent)
	void PlayAnimationBorder();
    
	UFUNCTION(BlueprintImplementableEvent)
	void StopAnimationBorder();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
	FItemData ItemData;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
	TObjectPtr<ULootBarWidget> LootBarWidget;

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;

	UFUNCTION()
	void OnRightClick();

	UFUNCTION()
	void EventButtonBackgroundHover();

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Background;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Border;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Item;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Name;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Quantity;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Rarity;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Type;

private:
	void SetItemDetails();
	void SetItemName();
	void SetItemType();
	void SetItemRarity();
	void SetItemRarityColor();
	void SetItemQuantity();
	void EventMouseMoved();
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config", meta=(AllowPrivateAccess="true"))
	TSubclassOf<ULootBarWidget> LootBarClass;
};
