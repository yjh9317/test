// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/Data/ItemDataStructs.h"
#include "EquipmentWidget.generated.h"

class UImage;
class UEquipmentSlotWidget;
class UInventoryComponent;
class UEquipmentComponent;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UEquipmentWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    void SetFocusToSlot(EItemSlot FocusSlot);
    void AssignCurrentlyFocusedSlot(EItemSlot FocusSlot);
    
protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;
    virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
    
    UPROPERTY(BlueprintReadOnly, Category="Slot", meta=(BindWidget))
    TObjectPtr<UEquipmentSlotWidget> EquipmentSlot_Head;

    UPROPERTY(BlueprintReadOnly, Category="Slot", meta=(BindWidget))
    TObjectPtr<UEquipmentSlotWidget> EquipmentSlot_Necklace;

    UPROPERTY(BlueprintReadOnly, Category="Slot", meta=(BindWidget))
    TObjectPtr<UEquipmentSlotWidget> EquipmentSlot_Chest;

    UPROPERTY(BlueprintReadOnly, Category="Slot", meta=(BindWidget))
    TObjectPtr<UEquipmentSlotWidget> EquipmentSlot_Legs;

    UPROPERTY(BlueprintReadOnly, Category="Slot", meta=(BindWidget))
    TObjectPtr<UEquipmentSlotWidget> EquipmentSlot_Gloves;

    UPROPERTY(BlueprintReadOnly, Category="Slot", meta=(BindWidget))
    TObjectPtr<UEquipmentSlotWidget> EquipmentSlot_Boots;

    UPROPERTY(BlueprintReadOnly, Category="Slot", meta=(BindWidget))
    TObjectPtr<UEquipmentSlotWidget> EquipmentSlot_PrimaryWeapon;

    UPROPERTY(BlueprintReadOnly, Category="Slot", meta=(BindWidget))
    TObjectPtr<UEquipmentSlotWidget> EquipmentSlot_SecondaryWeapon;

    UPROPERTY(BlueprintReadOnly, Category="Slot", meta=(BindWidget))
    TObjectPtr<UImage> Capture_Image;

private:
    void CreateEquipment();
    UEquipmentSlotWidget* GetWidgetSlotByItemSlot(EItemSlot FocusSlot);
    
    UFUNCTION()
    void EmptyEquipmentSlot(const FItemData& InItemData);
    UFUNCTION()
    void OverwriteEquipmentSlot(const FItemData& InItemData);

    UPROPERTY(meta=(AllowPrivateAccess = "true"))
    TMap<EItemSlot, UEquipmentSlotWidget*> EquipmentSlots;

    UPROPERTY(BlueprintReadOnly, Category="Component", meta=(AllowPrivateAccess = "true"))
    TObjectPtr<UEquipmentComponent> EquipmentComponent;

    UPROPERTY(BlueprintReadOnly, Category="Component", meta=(AllowPrivateAccess = "true"))
    TObjectPtr<UInventoryComponent> InventoryComponent;
    
    UPROPERTY(BlueprintReadOnly, Category="Variables", meta=(AllowPrivateAccess = "true"))
    EItemSlot FocusedSlot;
};
