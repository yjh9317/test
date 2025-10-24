// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/Common/WindowSwitcherWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

UWindowSwitcherWidget::UWindowSwitcherWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // It's unsafe to access BindWidget properties here, as they are not initialized yet.
    // This logic has been moved to NativePreConstruct.
}

void UWindowSwitcherWidget::NativePreConstruct()
{
    Super::NativePreConstruct();

    // This is the correct place to initialize widget properties for design-time visibility.
    if (TextBlock_Inventory)       TextBlock_Inventory->SetText(FText::FromString(TEXT("Inventory")));
    if (TextBlock_Quest)           TextBlock_Quest->SetText(FText::FromString(TEXT("Quest")));
    if (TextBlock_VendorEquipment) TextBlock_VendorEquipment->SetText(FText::FromString(TEXT("Equipment")));
    if (TextBlock_Abilities)       TextBlock_Abilities->SetText(FText::FromString(TEXT("Abilities")));
    if (TextBlock_Storage)         TextBlock_Storage->SetText(FText::FromString(TEXT("Storage")));
    if (TextBlock_StorageEquipment)TextBlock_StorageEquipment->SetText(FText::FromString(TEXT("Equipment")));
}

UButton* UWindowSwitcherWidget::GetButtonByWidgetType(EWidgetType Type) const
{
    EWidgetType ActiveWidgetType = GetActiveWidget();
    switch (Type)
    {
    case EWidgetType::INVENTORY:
        return Button_Inventory;
    case EWidgetType::EQUIPMENT:
        if (ActiveWidgetType == EWidgetType::VENDOR)
        {
            return Button_VendorEquipment;
        }
        else if (ActiveWidgetType == EWidgetType::STORAGE)
        {
            return Button_StorageEquipment;
        }
        break;
    case EWidgetType::CRAFTING:
        break;
    case EWidgetType::VENDOR:
        return Button_Vendor;
    case EWidgetType::STORAGE:
        return Button_Storage;
    case EWidgetType::LOADGAME:
        break;
    case EWidgetType::ABILITIES:
        return Button_Abilities;
    default: 
        break;
    }
    return nullptr;
}

UOverlay* UWindowSwitcherWidget::GetWidgetSwitcherPanel(EWidgetType Type) const
{
    switch (Type)
    {
    case EWidgetType::INVENTORY:
    case EWidgetType::CRAFTING:
    case EWidgetType::ABILITIES:
        return Overlay_Main;
    case EWidgetType::VENDOR:
        return Overlay_Vendor;
    case EWidgetType::STORAGE:
        return Overlay_Storage;
    default:
        break;
    }
    return nullptr;
}

void UWindowSwitcherWidget::OnHoverButton(EWidgetType Type)
{
    if (Type != GetActiveTab())
    {
        UButton* WidgetButton = GetButtonByWidgetType(Type);
        if (WidgetButton)
        {
            WidgetButton->SetColorAndOpacity(HoveredBorderColor);
        }
    }
}

void UWindowSwitcherWidget::OnUnHoverButton(EWidgetType Type)
{
    if (Type != GetActiveTab())
    {
        UButton* WidgetButton = GetButtonByWidgetType(Type);
        if (WidgetButton)
        {
            WidgetButton->SetColorAndOpacity(UnHoveredBorderColor);
        }
    }
}

EWidgetType UWindowSwitcherWidget::GetActiveTab() const
{
    if (IWidgetManager* WidgetManager = Cast<IWidgetManager>(GetOwningPlayer()))
    {
        return WidgetManager->GetActiveTab();
    }
    return EWidgetType::NONE;
}

EWidgetType UWindowSwitcherWidget::GetActiveWidget() const
{
    if (IWidgetManager* WidgetManager = Cast<IWidgetManager>(GetOwningPlayer()))
    {
        return WidgetManager->GetActiveWidget();
    }
    return EWidgetType::NONE;
}