// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Blueprint/UserWidget.h"
#include "LootBarWidget.generated.h"

class UInputMappingContext;
class UVerticalBox;
class UScrollBox;
class UImage;
class UButton;
class UInventoryComponent;
class UInventoryCoreComponent;
class UInputAction;
class ULootBarSlotWidget;
enum class EInventoryPanel;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API ULootBarWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable)
	void PlayConstructAnim();

	UFUNCTION()
	void TakeItem();

	UFUNCTION()
	void TakeAllItems();

	UFUNCTION()
	void CloseButton();

	UFUNCTION()
	void RefreshLootBar(EInventoryPanel Panel);
	
	void EventToggleInputListening();
	
	void CreateLootBar();
	void ScrollWidget(UUserWidget* FocusedWidget);
	void SetOwnerInventory();
	void ActivateLootBar();
	void DeactivateLootBar();

	void SetFocusToSlot(int32 SlotIndex);
	void SetCurrentlyFocusedSlot(ULootBarSlotWidget* InSlotWidget);
	bool ValidateOwnerInventory();
	
	TSoftObjectPtr<UTexture2D> GetInputIconTakeItem() const;
    TSoftObjectPtr<UTexture2D> GetInputIconTakeAllItems() const;
	TSoftObjectPtr<UTexture2D> GetInputIconCloseBar() const;
	

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Texture")
	TSoftObjectPtr<UTexture2D> TakeItemTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Texture")
	TSoftObjectPtr<UTexture2D> TakeAllItemTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Texture")
	TSoftObjectPtr<UTexture2D> CloseBarTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<ULootBarSlotWidget> LootBarSlotWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget")
	TObjectPtr<ULootBarSlotWidget> LootBarSlotWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UInventoryCoreComponent> OwnerInventory;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UInventoryComponent> PlayerInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default",meta=(BindWidget))
	TObjectPtr<UButton> Button_Close;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default",meta=(BindWidget))
	TObjectPtr<UButton> Button_Take;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default",meta=(BindWidget))
	TObjectPtr<UButton> Button_TakeAll;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default",meta=(BindWidget))
	TObjectPtr<UImage> Image_InputClose;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default",meta=(BindWidget))
	TObjectPtr<UImage> Image_InputTake;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default",meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default",meta=(BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	bool bKeyboardFocus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	int32 FocusedIndex;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Default")
	bool bCanListenForInput;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Defauls")
	float CanListenForInputTime = 0.2f;
	FTimerHandle CanListenForInputHandle;
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> TakeItemAction;
	
	UFUNCTION()
	void OnTakeItemTriggered(const FInputActionValue& Value);

	FDelegateHandle TakeItemHandle;
	FDelegateHandle TakeAllItemHandle;
	FDelegateHandle RefreshLootBarHandle;
};
