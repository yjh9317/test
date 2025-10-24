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
    UFUNCTION(BlueprintPure, Category="UI Binding")
    FText GetName() const;

    UFUNCTION(BlueprintPure, Category="UI Binding")
    FText GetQuestionText() const;

    UFUNCTION(BlueprintPure, Category="UI Binding")
    TSoftObjectPtr<UTexture2D> GetInputIconAccept() const;

    UFUNCTION(BlueprintPure, Category="UI Binding")
    TSoftObjectPtr<UTexture2D> GetInputIconCancel() const;

    UFUNCTION(BlueprintPure, Category="UI Binding")
    FText GetItemType() const;

    UFUNCTION(BlueprintPure, Category="UI Binding")
    FText GetItemRarity() const;

    UFUNCTION(BlueprintPure, Category="UI Binding")
    FSlateColor GetItemRarityColor();

    UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="Animation")
    void PlayConstructAnimation();

protected:
    virtual void NativeConstruct() override;
    virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
    virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
    virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

    UFUNCTION()
    void OnPressedAccept();
    
    UFUNCTION()
    void OnPressedCancel();

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config")
    TSoftObjectPtr<UTexture2D> InputAcceptTexture;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Config")
    TSoftObjectPtr<UTexture2D> InputCancelTexture;

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UImage> Image_ItemIcon;

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UImage> Image_InputAccept;

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UImage> Image_InputCancel;

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UButton> Button_Accept;

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UButton> Button_Cancel;

    UPROPERTY(meta=(BindWidget))
    TObjectPtr<UTextBlock> TextBlock_ItemName;

private:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Data", meta=(AllowPrivateAccess = "true"))
    TObjectPtr<UInventoryCoreComponent> Sender;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Data", meta=(AllowPrivateAccess = "true"))
    TObjectPtr<UInventoryCoreComponent> Receiver;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Data", meta=(AllowPrivateAccess = "true"))
    TObjectPtr<UInventoryComponent> PlayerInventory;
       
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Data", meta=(AllowPrivateAccess = "true"))
    FText QuestionText;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Data", meta=(AllowPrivateAccess = "true"))
    FItemData ItemData;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Data", meta=(AllowPrivateAccess = "true"))
    FItemData InSlotItemData;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Data", meta=(AllowPrivateAccess = "true"))
    EInputMethod InputMethod;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Data", meta=(AllowPrivateAccess = "true"))
    EItemDestination Initiator;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Data", meta=(AllowPrivateAccess = "true"))
    EItemDestination Destination;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Data", meta=(AllowPrivateAccess = "true"))
    TObjectPtr<UUserWidget> SenderWidget;
};
