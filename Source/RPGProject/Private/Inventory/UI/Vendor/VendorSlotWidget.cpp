// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/Vendor/VendorSlotWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Interaction/Data/InteractableDefinitions.h"
#include "Interface/WidgetManager.h"
#include "Interface/WidgetSlotListener.h"
#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"
#include "Inventory/UI/Items/DragDrop/ItemDataDragDropOperation.h"
#include "Inventory/UI/Items/ToolTip/ItemToolTipWidget.h"
#include "Inventory/UI/Items/DragDrop/ItemDragAndDropWidget.h"
#include "Item/ItemBase.h"
#include "Kismet/KismetInputLibrary.h"

void UVendorSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if(Button_Item)
	{
		Button_Item->OnClicked.AddDynamic(this,&UVendorSlotWidget::OnSlotRightClicked);
		Button_Item->OnHovered.AddDynamic(this,&UVendorSlotWidget::OnSlotHovered);
		Button_Item->OnUnhovered.AddDynamic(this,&UVendorSlotWidget::OnSlotUnhovered);
	}
	SetItemImageAsync();
	SetItemQuantity();
	RefreshToolTip();
}

void UVendorSlotWidget::NativeDestruct()
{
	if(ItemToolTipWidget)
	{
		ItemToolTipWidget->RemoveFromParent();
	}
}

FReply UVendorSlotWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

FReply UVendorSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FKey EffectingKey = UKismetInputLibrary::PointerEvent_GetEffectingButton(InMouseEvent);
	if(EffectingKey == EKeys::RightMouseButton)
	{
		OnSlotRightClicked();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void UVendorSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	if(!UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		return;
	}
	
	UItemBase* ItemBase = Cast<UItemBase>(ItemData.ItemClass->GetDefaultObject());
	if(!ItemBase || !ItemBase->Image)
	{
		return;
	}


	// Switch Inventory Panel
	PlayerInventory->SwitchActivePanel(ItemBase->InventoryPanel);

	// Create Drag&Drop Operation
	UItemDragAndDropWidget* DragAndDropWidget = CreateWidget<UItemDragAndDropWidget>(
		GetOwningPlayer(), ItemDragAndDropWidgetClass);

	if (!DragAndDropWidget)
	{
		return;
	}

	DragAndDropWidget->DraggableImage = ItemBase->Image.Get();

	UItemDataDragDropOperation* DragOperation = NewObject<UItemDataDragDropOperation>(
		this,
		ItemDataDragDropOperationClass,
		NAME_None,
		RF_Transient);

	DragOperation->Payload = nullptr;
	DragOperation->Pivot = EDragPivot::CenterCenter;
	DragOperation->DefaultDragVisual = DragAndDropWidget;
	DragOperation->DraggableWidget = DragAndDropWidget;
	DragOperation->ItemData = ItemData;
	DragOperation->DraggedFrom = EItemDestination::VendorSlot;
	OutOperation = DragOperation;
}

bool UVendorSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
		UItemDataDragDropOperation* ItemDataDragDropOperation = Cast<UItemDataDragDropOperation>(InOperation);
	APlayerController* LocalPC = GetOwningPlayer();
	if(!ItemDataDragDropOperation || !PlayerInventory || !LocalPC || !ActorInventory)
	{
		return false;		
	}
	
	FItemData DragAndDropItemData = ItemDataDragDropOperation->ItemData;

	if(ItemDataDragDropOperation->DraggedFrom == EItemDestination::InventorySlot)
	{
		if(UInventoryUtilityLibrary::IsItemClassValid(ItemData))
		{
			if(!UInventoryUtilityLibrary::AreItemsTheSame(DragAndDropItemData,ItemData) ||
				!UInventoryUtilityLibrary::AreItemsStackable(DragAndDropItemData,ItemData))
			{
				return false;
			}
		}

		EItemRemoveType RemoveType = PlayerInventory->GetItemRemoveType(DragAndDropItemData);
		IWidgetManager* WidgetManager = Cast<IWidgetManager>(LocalPC);
		switch (RemoveType)
		{
		case EItemRemoveType::Default:
			if(UInventoryUtilityLibrary::IsStackableAndHaveStacks(ItemDataDragDropOperation->ItemData,1))
			{
				// Show Split Stack Widget [On Accept - 'On Splitting Accepted' will be called]
				if(WidgetManager)
				{
					WidgetManager->OpenSplitStackPopup(
					PlayerInventory,
					ActorInventory,
					DragAndDropItemData,
					ItemData,
					EInputMethod::DragAndDrop,
					ItemDataDragDropOperation->DraggedFrom,
					EItemDestination::StorageSlot,
					nullptr
					);
					Border->SetBrushColor(UnHoveredBorderColor);
				}
			}
			else
			{
				// SERVER: Transfer Item from Player to Storage Inventory
				PlayerInventory->Server_TransferItemFromInventory(
					ActorInventory,
					PlayerInventory,
					DragAndDropItemData,
					ItemData,
					EInputMethod::DragAndDrop,
					LocalPC);
			}
			break;
		case EItemRemoveType::OnConfirmation:
			// Show Confirmation Popup - 'On Confirmation Popup Accepted' is called inside AC_InventoryCore(Receiver)
			if (WidgetManager)
			{
				WidgetManager->OpenConfirmationPopup(
					FText::FromString(TEXT("Are you sure you want to remove?")),
					PlayerInventory,
					ActorInventory,
					DragAndDropItemData,
					ItemData,
					EInputMethod::DragAndDrop,
					ItemDataDragDropOperation->DraggedFrom,
					EItemDestination::StorageSlot,
					nullptr
					);
				Border->SetBrushColor(UnHoveredBorderColor);
			}
			break;
		case EItemRemoveType::CannotBeRemoved:
			if(WidgetManager)
			{
				WidgetManager->DisplayMessageNotify(FText::FromString(TEXT("Item cannot be Removed.")));
			}
			break;
		}
	}
	else if (ItemDataDragDropOperation->DraggedFrom == EItemDestination::VendorSlot ||
		ItemDataDragDropOperation->DraggedFrom == EItemDestination::StorageSlot)
	{
		// Is not the same Slot
		if(DragAndDropItemData.Index != ItemData.Index)
		{
			PlayerInventory->Server_MoveItemToSlot(
				ActorInventory,
				EInventoryPanel::P1,
				DragAndDropItemData.Index,
				ItemData.Index);
		}
	}
	
	return true;
}

bool UVendorSlotWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	UItemDataDragDropOperation* ItemDragDropOperation = Cast<UItemDataDragDropOperation>(InOperation);
	if(!ItemDragDropOperation)
	{
		return false;
	}
	FItemData DragAndDropItemData = ItemDragDropOperation->ItemData;

	if(!UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		ItemDragDropOperation->ShowIconDrop();
		Border->SetBrushColor(BorderGreenColor);
	}

	if(ItemDragDropOperation->DraggedFrom == EItemDestination::InventorySlot)
	{
		if(UInventoryUtilityLibrary::AreItemsTheSame(DragAndDropItemData,ItemData) &&
				UInventoryUtilityLibrary::AreItemsStackable(DragAndDropItemData,ItemData))
		{
			ItemDragDropOperation->ShowIconDrop();
			Border->SetBrushColor(BorderGreenColor);
		}
		else
		{
			ItemDragDropOperation->ShowIconWrongSlot();
			Border->SetBrushColor(BorderRedColor);
		}
	}
	else if (ItemDragDropOperation->DraggedFrom == EItemDestination::VendorSlot ||
		ItemDragDropOperation->DraggedFrom == EItemDestination::StorageSlot)
	{
		ItemDragDropOperation->ShowIconSwap();
		Border->SetBrushColor(BorderGreenColor);
	}

	return true;
}

FReply UVendorSlotWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey InputKey = InKeyEvent.GetKey();
	if(InputKey == EKeys::LeftShift) 
	{
		ShowItemComparison();
		UInventoryUtilityLibrary::SetItemImageTextureAsync(Image_Item,ItemData);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UVendorSlotWidget::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey InputKey = InKeyEvent.GetKey();
	if(InputKey == EKeys::LeftShift) 
	{
		HideItemComparison();
		UInventoryUtilityLibrary::SetItemImageTextureAsync(Image_Item,ItemData);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void UVendorSlotWidget::OnSlotRightClicked()
{
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData) && ParentWidgetRef)
	{
		IWidgetSlotListener* WidgetSlotListener = Cast<IWidgetSlotListener>(ParentWidgetRef);
		if(WidgetSlotListener)
		{
			WidgetSlotListener->OnSlotClicked(this);
		}
	}
}

void UVendorSlotWidget::ShowItemComparison()
{
	if(!UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		return;
	}

	UItemBase* ItemBase = Cast<UItemBase>(ItemData.ItemClass->GetDefaultObject());
	if(ItemBase->Rarity != EItemRarity::Consumable && ItemToolTipWidget)
	{
		ItemToolTipWidget->ShowComparisonToolTip();
	}
}

void UVendorSlotWidget::HideItemComparison()
{
	if(ItemToolTipWidget)
	{
		ItemToolTipWidget->HideComparisonToolTip();
	}
}

void UVendorSlotWidget::OnSlotHovered()
{
	if(!IsAnyPopupActive())
	{
		StopHighlightAniamtion();
		Border->SetBrushColor(HoveredBorderColor);

		if(ParentWidgetRef)
		{
			if(IWidgetSlotListener* WidgetSlotListener = Cast<IWidgetSlotListener>(ParentWidgetRef))
			{
				WidgetSlotListener->OnSlotHovered(this);
			}
		}
	}
}

void UVendorSlotWidget::OnSlotUnhovered()
{
	Button_Item->SetToolTip(nullptr);
	Border->SetBrushColor(UnHoveredBorderColor);
	HideItemComparison();
	if(ItemToolTipWidget)
	{
		ItemToolTipWidget->RemoveFromParent();
	}
	if(ParentWidgetRef)
	{
		if(IWidgetSlotListener* WidgetSlotListener = Cast<IWidgetSlotListener>(ParentWidgetRef))
		{
			WidgetSlotListener->OnSlotUnhovered(this);
		}
	}
}


void UVendorSlotWidget::CreateItemTooltip()
{
	HideItemComparison();

	if(ItemToolTipWidget)
	{
		ItemToolTipWidget->RemoveFromParent();
		ItemToolTipWidget = CreateWidget<UItemToolTipWidget>(GetOwningPlayer(),ItemToolTipWidgetClass);
		ItemToolTipWidget->ItemData = ItemData;
		ItemToolTipWidget->bIsCompareItem = false;
	}

	if(Button_Item)
	{
		Button_Item->SetToolTip(ItemToolTipWidget);
	}
}

void UVendorSlotWidget::SetItemImageAsync()
{
	if(UInventoryUtilityLibrary::SetItemImageTextureAsync(Image_Item,ItemData) == EOperationStatus::Failed)
	{
		Image_Item->SetVisibility(ESlateVisibility::Collapsed);
	}
	else
	{
		Image_Item->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		if(ItemData.AlreadyUsed)
		{
			Image_Item->SetColorAndOpacity(AlreadyUsedColor);
		}
		else
		{
			Image_Item->SetColorAndOpacity(NotUsedColor);
		}
	}
}

void UVendorSlotWidget::SetItemQuantity()
{
	if (!TextBlock_Quantity)
	{
		return;
	}

	if (!UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		TextBlock_Quantity->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	UItemBase* ItemBase = Cast<UItemBase>(ItemData.ItemClass->GetDefaultObject());
	if (!ItemBase)
	{
		TextBlock_Quantity->SetVisibility(ESlateVisibility::Collapsed);
		return;
	}

	bool bShouldShowQuantity = ItemBase->Stackable && ItemData.Quantity > 1;
    
	if (bShouldShowQuantity)
	{
		TextBlock_Quantity->SetText(FText::AsNumber(ItemData.Quantity));
		TextBlock_Quantity->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	}
	else
	{
		TextBlock_Quantity->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UVendorSlotWidget::EmptySlot()
{
	FItemData LocalItemData{};
	LocalItemData.Index = SlotIndex;
	
	ItemData = LocalItemData;
	SetItemImageAsync();
	SetItemQuantity();
	Button_Item->SetToolTip(nullptr);
	if (ItemToolTipWidget)
	{
		ItemToolTipWidget->RemoveFromParent();
	}
}

void UVendorSlotWidget::HighlightSlot()
{
	APlayerController* LocalPC = GetOwningPlayer();
	if(LocalPC && !HasUserFocusedDescendants(LocalPC) && !IsHovered())
	{
		PlayHighlightAniamtion();
	}
}

void UVendorSlotWidget::RefreshToolTip()
{
	if(IsHovered())
	{
		Button_Item->SetToolTip(nullptr);
		HideItemComparison();
		if(ItemToolTipWidget)
		{
			ItemToolTipWidget->RemoveFromParent();
		}
		ItemToolTipWidget = CreateWidget<UItemToolTipWidget>(GetOwningPlayer(),ItemToolTipWidgetClass);
		ItemToolTipWidget->ItemData = ItemData;
		ItemToolTipWidget->bIsCompareItem = false;
		Button_Item->SetToolTip(ItemToolTipWidget);
	}
}

bool UVendorSlotWidget::IsAnyPopupActive()
{
	if(IWidgetManager* WidgetManager = Cast<IWidgetManager>(GetOwningPlayer()))
	{
		if(WidgetManager->GetActivePopup() != EWidgetPopup::None)
		{
			return true;
		}
	}
	return false;
}
