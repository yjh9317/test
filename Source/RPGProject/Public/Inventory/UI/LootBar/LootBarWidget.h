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
    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
    void PlayConstructAnim();

    UFUNCTION()
    void TakeItem();

    UFUNCTION()
    void TakeAllItems();

    void ActivateLootBar();
    void DeactivateLootBar();
    void SetCurrentlyFocusedSlot(ULootBarSlotWidget* InSlotWidget);

    UFUNCTION(BlueprintPure, Category="UI")
    TSoftObjectPtr<UTexture2D> GetInputIconTakeItem() const;
    
    UFUNCTION(BlueprintPure, Category="UI")
    TSoftObjectPtr<UTexture2D> GetInputIconTakeAllItems() const;

    UFUNCTION(BlueprintPure, Category="UI")
    TSoftObjectPtr<UTexture2D> GetInputIconCloseBar() const;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "State")
    bool bKeyboardFocus;

protected:
    virtual void NativeOnInitialized() override;
    virtual void NativeConstruct() override;
    virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
    virtual FReply NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;


    UFUNCTION()
    void CloseButton();

    UFUNCTION()
    void RefreshLootBar(EInventoryPanel Panel);

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UButton> Button_Close;
    
    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UButton> Button_Take;

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UButton> Button_TakeAll;

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UImage> Image_InputClose;

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UImage> Image_InputTake;

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UScrollBox> ScrollBox;

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UVerticalBox> VerticalBox;

private:
    void EventToggleInputListening();
    void CreateLootBar();
    void ScrollWidget(UUserWidget* FocusedWidget);
    void SetOwnerInventory();
    void SetFocusToSlot(int32 SlotIndex);
    bool ValidateOwnerInventory();
    
    UFUNCTION()
    void OnTakeItemTriggered(const FInputActionValue& Value);

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Texture", meta=(AllowPrivateAccess="true"))
    TSoftObjectPtr<UTexture2D> TakeItemTexture;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Texture", meta=(AllowPrivateAccess="true"))
    TSoftObjectPtr<UTexture2D> TakeAllItemTexture;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Texture", meta=(AllowPrivateAccess="true"))
    TSoftObjectPtr<UTexture2D> CloseBarTexture;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget", meta=(AllowPrivateAccess="true"))
    TSubclassOf<ULootBarSlotWidget> LootBarSlotWidgetClass;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Widget", meta=(AllowPrivateAccess="true"))
    TObjectPtr<ULootBarSlotWidget> LootBarSlotWidget;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UInventoryCoreComponent> OwnerInventory;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component", meta=(AllowPrivateAccess="true"))
    TObjectPtr<UInventoryComponent> PlayerInventory;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta=(AllowPrivateAccess="true"))
    int32 FocusedIndex;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State", meta=(AllowPrivateAccess="true"))
    bool bCanListenForInput;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Config", meta=(AllowPrivateAccess="true"))
    float CanListenForInputTime = 0.2f;
    
    FTimerHandle CanListenForInputHandle;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Enhanced Input", meta = (AllowPrivateAccess = "true"))
    TObjectPtr<UInputAction> TakeItemAction;

    FDelegateHandle TakeItemHandle;
    FDelegateHandle TakeAllItemHandle;
    FDelegateHandle RefreshLootBarHandle;
};
