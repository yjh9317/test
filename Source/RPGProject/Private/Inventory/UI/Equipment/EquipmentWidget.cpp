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

	IComponentManager* ComponentManager = Cast<IComponentManager>(GetOwningPlayer());
	if(ComponentManager)
	{
		EquipmentComponent = ComponentManager->GetEquipmentComponent();
		InventoryComponent = ComponentManager->GetInventoryComponent();
	}
	CreateEquipment();

	if(EquipmentComponent)
	{
		EquipmentComponent->OnAddedToEquipment.AddUObject(this,&ThisClass::OverwriteEquipmentSlot);
		EquipmentComponent->OnRemovedFromEquipment.AddUObject(this,&ThisClass::EmptyEquipmentSlot);
	}
}

void UEquipmentWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if(EquipmentComponent)
	{
		EquipmentComponent->OnAddedToEquipment.RemoveAll(this);
		EquipmentComponent->OnRemovedFromEquipment.RemoveAll(this);
		// EquipmentComponent->OnAddedToEquipment.RemoveDynamic(this,&ThisClass::OverwriteEquipmentSlot);
		// EquipmentComponent->OnRemovedFromEquipment.RemoveDynamic(this,&ThisClass::EmptyEquipmentSlot);
	}
}

FReply UEquipmentWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UEquipmentWidget::CreateEquipment()
{
	if(nullptr == EquipmentComponent) return;

	TMap<EItemSlot,FItemData> EquipmentData = EquipmentComponent->GetEquipmentData();
	TArray<EItemSlot> ItemSlots;
	ItemSlots.Reserve(EquipmentData.Num());
	
	for(const auto& EquipmentSlot: ItemSlots)
	{
		if(UEquipmentSlotWidget* CurrentEquipmentWidget = GetWidgetSlotByItemSlot(EquipmentSlot))
		{
			CurrentEquipmentWidget->OverwriteSlot(this,EquipmentData[EquipmentSlot]);		
		}
	}
}

void UEquipmentWidget::SetFocusToSlot(EItemSlot FocusSlot)
{
	if(UEquipmentSlotWidget* CurrentEquipmentWidget = GetWidgetSlotByItemSlot(FocusSlot))
	{
		FocusedSlot = FocusSlot;
		SetKeyboardFocus();
	}
}

UEquipmentSlotWidget* UEquipmentWidget::GetWidgetSlotByItemSlot(EItemSlot FocusSlot)
{
	switch (FocusSlot)
	{
	case EItemSlot::HEAD: return EquipmentSlots[EItemSlot::HEAD]; break;
	case EItemSlot::LEGS: return EquipmentSlots[EItemSlot::LEGS]; break;
	case EItemSlot::NECK: return EquipmentSlots[EItemSlot::NECK];break;
	case EItemSlot::BOOTS: return EquipmentSlots[EItemSlot::BOOTS];break;
	case EItemSlot::GLOVES: return EquipmentSlots[EItemSlot::GLOVES];break;
	case EItemSlot::SHIELD: return EquipmentSlots[EItemSlot::SHIELD];break;
	case EItemSlot::WEAPON: return EquipmentSlots[EItemSlot::WEAPON];break;
	case EItemSlot::CHESTPLATE: return EquipmentSlots[EItemSlot::CHESTPLATE];break;
	}
	return nullptr;
}

void UEquipmentWidget::EmptyEquipmentSlot(const FItemData& InItemData)
{
	if(!UInventoryUtilityLibrary::IsItemClassValid(InItemData))
	{
		DebugHeader::LogWarning("UEquipmentWidget::EmptyEquipmentSlot : InItemData is not valid");
		return;
	}
	UEquipmentSlotWidget* SlotWidget = GetWidgetSlotByItemSlot(InItemData.EquipmentSlot);
	if(SlotWidget) SlotWidget->EmptySlot();
}

void UEquipmentWidget::OverwriteEquipmentSlot(const FItemData& InItemData)
{
	if(!UInventoryUtilityLibrary::IsItemClassValid(InItemData))
	{
		DebugHeader::LogWarning("UEquipmentWidget::OverwriteEquipmentSlot : InItemData is not valid");
		return;
	}
	UEquipmentSlotWidget* SlotWidget = GetWidgetSlotByItemSlot(InItemData.EquipmentSlot);
	if(SlotWidget) SlotWidget->OverwriteSlot(this,InItemData);
}

void UEquipmentWidget::AssignCurrentlyFocusedSlot(EItemSlot FocusSlot)
{
	FocusedSlot = FocusSlot;
}
