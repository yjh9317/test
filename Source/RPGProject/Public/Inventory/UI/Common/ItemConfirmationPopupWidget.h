// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/Data/ItemDataStructs.h"
#include "Types/InputEnums.h"
#include "ItemConfirmationPopupWidget.generated.h"

class UInventoryComponent;
class UInventoryCoreComponent;
class UButton;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UItemConfirmationPopupWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void PlayConstructAnimation();

	UFUNCTION()
	void OnPressedAccept();
	UFUNCTION()
	void OnPressedCancel();
	
	FText GetName() const;
	FText GetQuestionText() const;
	TSoftObjectPtr<UTexture2D> GetInputIconAccept() const;
	TSoftObjectPtr<UTexture2D> GetInputIconCancel() const;
	FText GetItemType() const;
	FText GetItemRarity() const;
	FSlateColor GetItemRarityColor();

	UPROPERTY(BlueprintReadOnly, Category="Component")
	TObjectPtr<UInventoryCoreComponent> Sender;
	
	UPROPERTY(BlueprintReadOnly, Category="Component")
	TObjectPtr<UInventoryCoreComponent> Receiver;

	UPROPERTY(BlueprintReadOnly, Category="Component")
	TObjectPtr<UInventoryComponent> PlayerInventory;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget",meta=(BindWidget))
	TObjectPtr<UImage> Image_ItemIcon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget",meta=(BindWidget))
	TObjectPtr<UImage> Image_InputAccept;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget",meta=(BindWidget))
	TObjectPtr<UImage> Image_InputCancel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget",meta=(BindWidget))
	TObjectPtr<UButton> Button_Accept;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget",meta=(BindWidget))
	TObjectPtr<UButton> Button_Cancel;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget",meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	TSoftObjectPtr<UTexture2D> InputAcceptTexture;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	TSoftObjectPtr<UTexture2D> InputCancelTexture;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Variables")
	FText QuestionText;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Variables")
	FItemData ItemData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Variables")
	FItemData InSlotItemData;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Variables")
	EInputMethod InputMethod;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Variables")
	EItemDestination Initiator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Variables")
	EItemDestination Destination;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Variables")
	TObjectPtr<UUserWidget> SenderWidget;
};
