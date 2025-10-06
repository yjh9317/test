// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/Equipment/EquipmentSlotWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "Header/DebugHeader.h"
#include "Interaction/Data/InteractableDefinitions.h"
#include "Interface/ComponentManager.h"
#include "Interface/FocusWidgetHandler.h"
#include "Interface/WidgetManager.h"
#include "Inventory/Components/EquipmentComponent.h"
#include "Inventory/Components/InventoryComponent.h"
#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"
#include "Inventory/UI/Items/DragDrop/ItemDataDragDropOperation.h"
#include "Inventory/UI/Items/DragDrop/ItemDragAndDropWidget.h"
#include "Inventory/UI/Items/ToolTip/ItemToolTipWidget.h"
#include "Player/Controller/MainPlayerController.h"
#include "Shared/Util/AsyncLoadUtility.h"

void UEquipmentSlotWidget::NativePreConstruct()
{
	if (Image_Item)
	{
		Image_Item->SetBrush(EmptySlotStyle);
	}
	if (Overlay_Label)
	{
		Overlay_Label->SetVisibility(SetLabelVisibility());
	}
	if (TextBlock_Label)
	{
		TextBlock_Label->SetText(SetLabelCharacter());
	}
	if(TextBlock_SlotName)
	{
		TextBlock_SlotName->SetText(EquipmentSlotName);
	}
}

void UEquipmentSlotWidget::NativeConstruct()
{
	IComponentManager* ComponentManager = Cast<IComponentManager>(GetOwningPlayer());
	if (ComponentManager)
	{
		PlayerEquipment = ComponentManager->GetEquipmentComponent();
		PlayerInventory = ComponentManager->GetInventoryComponent();
	}
	if(Button_Item)
	{
		Button_Item->OnClicked.AddDynamic(this, &UEquipmentSlotWidget::OnRightClick);
	}
}

void UEquipmentSlotWidget::NativeDestruct()
{
	if (ItemToolTipWidget)
	{
		ItemToolTipWidget->RemoveFromParent();
	}
}

void UEquipmentSlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (UItemDataDragDropOperation* DropOperation = Cast<UItemDataDragDropOperation>(InOperation))
	{
		DropOperation->ClearDraggableIcon();
		if (Image_Border)
		{
			Image_Border->SetColorAndOpacity(HoveredBorderColor);
		}
	}
}

FReply UEquipmentSlotWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry,
                                                            const FPointerEvent& InMouseEvent)
{
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

FReply UEquipmentSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		OnRightClick();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void UEquipmentSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                                UDragDropOperation*& OutOperation)
{
	APlayerController* LocalPC = GetOwningPlayer();
	if (UInventoryUtilityLibrary::IsItemClassValid(ItemData) && LocalPC)
	{
		UItemBase* ItemBase = Cast<UItemBase>(ItemData.ItemClass->GetDefaultObject());
		
		if (nullptr == ItemBase || !ItemBase->Image)
		{
			return;
		}

		PlayerInventory->SwitchActivePanel(ItemBase->InventoryPanel);

		if (ItemDragAndDropWidgetClass && ItemDataDragAndDropOperationClass)
		{
			UItemDragAndDropWidget* DragAndDropWidget = CreateWidget<UItemDragAndDropWidget>(LocalPC, ItemDragAndDropWidgetClass);
			
			if(!DragAndDropWidget)
			{
				return;
			}
			DragAndDropWidget->DraggableImage = ItemBase->Image.Get();

			UItemDataDragDropOperation* DragOperation = NewObject<UItemDataDragDropOperation>(
				this,
				ItemDataDragAndDropOperationClass,
				NAME_None,
				RF_Transient);

			DragOperation->Payload = nullptr;
			DragOperation->Pivot = EDragPivot::CenterCenter;
			DragOperation->DefaultDragVisual = DragAndDropWidget;
			DragOperation->DraggableWidget = DragAndDropWidget;
			DragOperation->ItemData = ItemData;
			DragOperation->DraggedFrom = EItemDestination::EquipmentSlot;
			OutOperation = DragOperation;
		}
	}
}

bool UEquipmentSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                        UDragDropOperation* InOperation)
{
	if (const UItemDataDragDropOperation* DragAndDropOperation = Cast<UItemDataDragDropOperation>(InOperation))
	{
		FItemData DragItemData = DragAndDropOperation->ItemData;
		if (UInventoryUtilityLibrary::AreItemSlotsEqual(DragItemData, ItemData) ||
			(UInventoryUtilityLibrary::CanWeaponsBeSwapped(DragItemData, ItemData) && PlayerEquipment->EnableOffHand))
		{
			switch (DragAndDropOperation->DraggedFrom)
			{
			case EItemDestination::InventorySlot:
				{
					PlayerEquipment->ServerTransferItemFromInventory(DragAndDropOperation->InventoryComponent,
					                                                 DragItemData, ItemData, EInputMethod::DragAndDrop);
					Image_Border->SetColorAndOpacity(UnHoveredBorderColor);
				}
				break;
			case EItemDestination::EquipmentSlot:
				{
					if (!UInventoryUtilityLibrary::IsItemClassValid(ItemData) ||
						UInventoryUtilityLibrary::AreWeaponTypesEqual(DragItemData, ItemData))
					{
						PlayerEquipment->ServerTransferItemFromEquipment(DragItemData, ItemData);
					}
				}
				break;
			default:
				return false;
				break;
			}
			return true;
		}
	}
	return false;
}

bool UEquipmentSlotWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
                                            UDragDropOperation* InOperation)
{
	UItemDataDragDropOperation* DragAndDropOperation = Cast<UItemDataDragDropOperation>(InOperation);
	if (!DragAndDropOperation)
	{
		return false;
	}
    
	if (!PlayerEquipment)
	{
		return false; 
	}
	
	if (!Image_Border)
	{
		return false;
	}

	FItemData DragItemData = DragAndDropOperation->ItemData;

	if ((UInventoryUtilityLibrary::AreItemSlotsEqual(DragItemData, ItemData) ||
		  (PlayerEquipment->EnableOffHand &&
			 UInventoryUtilityLibrary::CanWeaponsBeSwapped(DragItemData, ItemData))) &&
	   PlayerEquipment->CanItemBeEquipped(DragItemData))
	{
		DragAndDropOperation->ShowIconSwap();
		Image_Border->SetColorAndOpacity(FLinearColor::Green);
	}
	else
	{
		DragAndDropOperation->ShowIconWrongSlot();
		Image_Border->SetColorAndOpacity(FLinearColor::Red);
	}

	return true;
}


void UEquipmentSlotWidget::OnRightClick()
{
	if (UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		PlayerInventory->Server_TransferItemFromEquipment(PlayerEquipment, ItemData, FItemData{},
		                                                 EInputMethod::RightClick);
	}
}

void UEquipmentSlotWidget::SetItemQuantity()
{
	// if (!UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	// {
	// 	TextBlock_Quantity->SetVisibility(ESlateVisibility::Collapsed);
	// 	return;
	// }
	//
	// UItemBase* ItemBase = Cast<UItemBase>(ItemData.ItemClass->GetDefaultObject());
	// if(nullptr == ItemBase) return;
	//
	// if (ItemBase && ItemBase->Stackable && ItemData.Quantity > 1)
	// {
	// 	TextBlock_Quantity->SetText(FText::AsNumber(ItemData.Quantity));
	// 	TextBlock_Quantity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	// }
	// else
	// {
	// 	TextBlock_Quantity->SetVisibility(ESlateVisibility::Collapsed);
	// }
}

void UEquipmentSlotWidget::EmptySlot()
{
	FItemData LocalItemData{};
	LocalItemData.EquipmentSlot = EquipmentSlot;
	
	ItemData = LocalItemData;
	SetItemImageAsync(ItemData);
	SetItemQuantity();
	Button_Item->SetToolTip(nullptr);
	if (ItemToolTipWidget)
	{
		// ItemToolTipWidget->RemoveFromParent();
		ItemToolTipWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

ESlateVisibility UEquipmentSlotWidget::SetLabelVisibility()
{
	if (LabelCharText.IsEmpty())
	{
		return ESlateVisibility::Collapsed;
	}
	return ESlateVisibility::HitTestInvisible;
}

FText UEquipmentSlotWidget::SetLabelCharacter() const
{
	return LabelCharText;
}

void UEquipmentSlotWidget::OverwriteSlot(UEquipmentWidget* InEquipmentWidget,const FItemData& InItemData)
{
	EquipmentWidget = InEquipmentWidget;
	ItemData = InItemData;

	SetItemImageAsync(InItemData);
	SetItemQuantity();
	RefreshToolTip();
}

void UEquipmentSlotWidget::RefreshToolTip()
{
	if (IsHovered())
	{
		Button_Item->SetToolTip(nullptr);
		if (ItemToolTipWidget)
		{
			// ItemToolTipWidget->RemoveFromParent();
			ItemToolTipWidget->SetVisibility(ESlateVisibility::Hidden);
		}
		
		ItemToolTipWidget = CreateWidget<UItemToolTipWidget>(GetWorld(), ItemToolTipWidgetClass);
		ItemToolTipWidget->ItemData = ItemData;
		Button_Item->SetToolTip(ItemToolTipWidget);
	}
}

EWidgetType UEquipmentSlotWidget::GetActiveWidget()
{
	IWidgetManager* WidgetManager = Cast<IWidgetManager>(GetOwningPlayer());
	if (WidgetManager)
	{
		return WidgetManager->GetActiveWidget();
	}
	return EWidgetType::NONE;
}

void UEquipmentSlotWidget::SetItemImageAsync(const FItemData& InItemData)
{
	if(!InItemData.ItemClass) return;
	
	if (!UInventoryUtilityLibrary::IsItemClassValid(InItemData))
	{
		Image_Item->SetBrush(EmptySlotStyle);
		SetItemQuantity();
		return;
	}

	TSoftObjectPtr<UTexture2D> ImageSource  = InItemData.ItemClass.GetDefaultObject()->Image;
	TSharedPtr<FStreamableHandle> Handle = RPGAsyncLoadUtil::RequestAsyncLoadObject<UTexture2D>(
		ImageSource,
		[this](UTexture2D* LoadedTexture)
		{
			if (LoadedTexture)
			{
				Image_Item->SetBrushFromTexture(LoadedTexture);
			}
		}
	);
}

 