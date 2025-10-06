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
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	void CreateEquipment();
	
	void SetFocusToSlot(EItemSlot FocusSlot);
	UEquipmentSlotWidget* GetWidgetSlotByItemSlot(EItemSlot FocusSlot);
	void AssignCurrentlyFocusedSlot(EItemSlot FocusSlot);
	
	UFUNCTION()
	void EmptyEquipmentSlot(const FItemData& InItemData);
	UFUNCTION()
	void OverwriteEquipmentSlot(const FItemData& InItemData);

	UPROPERTY()
	TMap<EItemSlot,UEquipmentSlotWidget*> EquipmentSlots;

	UPROPERTY(BlueprintReadOnly, Category="Component")
	TObjectPtr<UEquipmentComponent> EquipmentComponent;

	UPROPERTY(BlueprintReadOnly, Category="Component")
	TObjectPtr<UInventoryComponent> InventoryComponent;
	
	UPROPERTY(BlueprintReadOnly, Category="Variables")
	EItemSlot FocusedSlot;

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
};

