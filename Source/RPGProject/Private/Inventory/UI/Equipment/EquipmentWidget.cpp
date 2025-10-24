// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/Equipment/EquipmentWidget.h"
#include "Header/DebugHeader.h"
#include "Interface/ComponentManager.h"
#include "Inventory/Components/EquipmentComponent.h"
#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"
#include "Inventory/UI/Equipment/EquipmentSlotWidget.h"

void UEquipmentWidget::NativeConstruct()
{
    Super::NativeConstruct();

    if (IComponentManager* ComponentManager = Cast<IComponentManager>(GetOwningPlayer()))
    {
        EquipmentComponent = ComponentManager->GetEquipmentComponent();
        InventoryComponent = ComponentManager->GetInventoryComponent();
    }
    
    // Populate the map for easy lookup. This fixes the original bug.
    EquipmentSlots.Add(EItemSlot::HEAD, EquipmentSlot_Head);
    EquipmentSlots.Add(EItemSlot::NECK, EquipmentSlot_Necklace);
    EquipmentSlots.Add(EItemSlot::CHESTPLATE, EquipmentSlot_Chest);
    EquipmentSlots.Add(EItemSlot::LEGS, EquipmentSlot_Legs);
    EquipmentSlots.Add(EItemSlot::GLOVES, EquipmentSlot_Gloves);
    EquipmentSlots.Add(EItemSlot::BOOTS, EquipmentSlot_Boots);
    EquipmentSlots.Add(EItemSlot::WEAPON, EquipmentSlot_PrimaryWeapon);
    EquipmentSlots.Add(EItemSlot::SHIELD, EquipmentSlot_SecondaryWeapon);

    CreateEquipment();

    if (EquipmentComponent)
    {
        EquipmentComponent->OnAddedToEquipment.AddUObject(this, &ThisClass::OverwriteEquipmentSlot);
        EquipmentComponent->OnRemovedFromEquipment.AddUObject(this, &ThisClass::EmptyEquipmentSlot);
    }
}

void UEquipmentWidget::NativeDestruct()
{
    Super::NativeDestruct();

    if (EquipmentComponent)
    {
        EquipmentComponent->OnAddedToEquipment.RemoveAll(this);
        EquipmentComponent->OnRemovedFromEquipment.RemoveAll(this);
    }
}

FReply UEquipmentWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
    return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UEquipmentWidget::CreateEquipment()
{
    if (nullptr == EquipmentComponent) return;

    TMap<EItemSlot, FItemData> EquipmentData = EquipmentComponent->GetEquipmentData();

    // Correctly iterate through the existing equipment data to populate slots.
    for (const TPair<EItemSlot, FItemData>& EquipmentPair : EquipmentData)
    {
        if (UEquipmentSlotWidget* CurrentEquipmentWidget = GetWidgetSlotByItemSlot(EquipmentPair.Key))
        {
            CurrentEquipmentWidget->OverwriteSlot(this, EquipmentPair.Value);
        }
    }
}

void UEquipmentWidget::SetFocusToSlot(EItemSlot FocusSlot)
{
    if (UEquipmentSlotWidget* CurrentEquipmentWidget = GetWidgetSlotByItemSlot(FocusSlot))
    {
        FocusedSlot = FocusSlot;
        CurrentEquipmentWidget->SetKeyboardFocus();
    }
}

UEquipmentSlotWidget* UEquipmentWidget::GetWidgetSlotByItemSlot(EItemSlot FocusSlot)
{
    // Simplified and safer lookup.
    return EquipmentSlots.FindRef(FocusSlot);
}

void UEquipmentWidget::EmptyEquipmentSlot(const FItemData& InItemData)
{
    if (!UInventoryUtilityLibrary::IsItemClassValid(InItemData))
    {
        DebugHeader::LogWarning("UEquipmentWidget::EmptyEquipmentSlot : InItemData is not valid");
        return;
    }
    if (UEquipmentSlotWidget* SlotWidget = GetWidgetSlotByItemSlot(InItemData.EquipmentSlot))
    {
        SlotWidget->EmptySlot();
    }
}

void UEquipmentWidget::OverwriteEquipmentSlot(const FItemData& InItemData)
{
    if (!UInventoryUtilityLibrary::IsItemClassValid(InItemData))
    {
        DebugHeader::LogWarning("UEquipmentWidget::OverwriteEquipmentSlot : InItemData is not valid");
        return;
    }
    if (UEquipmentSlotWidget* SlotWidget = GetWidgetSlotByItemSlot(InItemData.EquipmentSlot))
    {
        SlotWidget->OverwriteSlot(this, InItemData);
    }
}

void UEquipmentWidget::AssignCurrentlyFocusedSlot(EItemSlot FocusSlot)
{
    FocusedSlot = FocusSlot;
}