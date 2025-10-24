// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/Storage/StorageSlotWidget.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Header/DebugHeader.h"
#include "Interaction/Data/InteractableDefinitions.h"
#include "Interface/WidgetManager.h"
#include "Interface/WidgetSlotListener.h"
#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"
#include "Inventory/UI/Items/DragDrop/ItemDataDragDropOperation.h"
#include "Inventory/UI/Items/DragDrop/ItemDragAndDropWidget.h"
#include "Inventory/UI/Items/ToolTip/ItemToolTipWidget.h"
#include "Item/ItemBase.h"
#include "Kismet/KismetInputLibrary.h"

void UStorageSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(Button_Item)
	{
		Button_Item->OnClicked.AddDynamic(this,&UStorageSlotWidget::OnSlotRightClicked);
		Button_Item->OnHovered.AddDynamic(this,&UStorageSlotWidget::OnSlotHovered);
		Button_Item->OnUnhovered.AddDynamic(this,&UStorageSlotWidget::OnSlotUnhovered);
	}
	SetItemImageAsync();
	SetItemQuantity();
	RefreshToolTip();
}

void UStorageSlotWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if(ItemToolTipWidget)
	{
		// ItemToolTipWidget->RemoveFromParent();
		ItemToolTipWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}


FReply UStorageSlotWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	return UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, EKeys::LeftMouseButton).NativeReply;
}

FReply UStorageSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if(UKismetInputLibrary::PointerEvent_GetEffectingButton(InMouseEvent) == EKeys::RightMouseButton)
	{
		OnSlotRightClicked();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

void UStorageSlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	if(!UInventoryUtilityLibrary::IsItemClassValid(ItemData)  || !PlayerInventory)
	{
		return;
	}

	UItemBase* ItemBase = Cast<UItemBase>(ItemData.ItemClass->GetDefaultObject());
	if(!ItemBase->Image.Get())
	{
		return;
	}

	PlayerInventory->SwitchActivePanel(ItemBase->InventoryPanel);
	ItemDragAndDropWidgetRef = CreateWidget<UItemDragAndDropWidget>(GetOwningPlayer(),ItemDragAndDropWidgetClass);
	FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
	
	StreamableManager.RequestAsyncLoad(ItemBase->Image.ToSoftObjectPath(), [this,&ItemBase]()
	{
		TObjectPtr<UTexture2D> LoadedTexture = ItemBase->Image.Get();
		if(ItemDragAndDropWidgetRef && LoadedTexture)
		{
			ItemDragAndDropWidgetRef->DraggableImage = LoadedTexture;
		}
	});
	ItemDataDragAndDropOperationRef  = NewObject<UItemDataDragDropOperation>(
				this, 
				ItemDataDragAndDropOperationClass,
				NAME_None,
				RF_Transient);
	
	if(ItemDataDragAndDropOperationRef && ItemDragAndDropWidgetRef && ActorInventory)
	{
		ItemDataDragAndDropOperationRef->DefaultDragVisual = ItemDragAndDropWidgetRef;
		ItemDataDragAndDropOperationRef->Pivot = EDragPivot::CenterCenter;
		ItemDataDragAndDropOperationRef->DraggableWidget = ItemDragAndDropWidgetRef;
		ItemDataDragAndDropOperationRef->DraggedFrom = EItemDestination::StorageSlot;
		ItemDataDragAndDropOperationRef->InventoryComponent = ActorInventory;
	}
}

bool UStorageSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,UDragDropOperation* InOperation)
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
					Border->SetBrushColor(HoveredBorderColor);
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

bool UStorageSlotWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,UDragDropOperation* InOperation)
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

void UStorageSlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	UItemDataDragDropOperation* ItemDataDragDropOperation = Cast<UItemDataDragDropOperation>(InOperation);
	if(!ItemDataDragDropOperation)
	{
		return ;		
	}
	ItemDataDragDropOperation->ClearDraggableIcon();
	Border->SetBrushColor(UnHoveredBorderColor);
}

void UStorageSlotWidget::OnSlotRightClicked()
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

void UStorageSlotWidget::OnSlotHovered()
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

void UStorageSlotWidget::OnSlotUnhovered()
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

void UStorageSlotWidget::SetItemImageAsync()
{
	if(!Image_Item || !UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		return;
	}
	
	if(UInventoryUtilityLibrary::SetItemImageTextureAsync(Image_Item,ItemData) == EOperationStatus::Failed)
	{
		Image_Item->SetVisibility(ESlateVisibility::Collapsed);
	}
	Image_Item->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	if(ItemData.AlreadyUsed)
	{
		Image_Item->SetColorAndOpacity(ImageItemUsedColor);
	}
	else
	{
		Image_Item->SetColorAndOpacity(ImageItemUnusedColor);
	}
}

void UStorageSlotWidget::CreateItemTooltip()
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

void UStorageSlotWidget::ShowItemComparison()
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

void UStorageSlotWidget::HideItemComparison()
{
	if(ItemToolTipWidget)
	{
		ItemToolTipWidget->HideComparisonToolTip();
	}
}

void UStorageSlotWidget::SetItemQuantity()
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

void UStorageSlotWidget::EmptySlot()
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

void UStorageSlotWidget::HighlightSlot()
{
	APlayerController* LocalPC = GetOwningPlayer();
	if(LocalPC && !HasUserFocusedDescendants(LocalPC) && !IsHovered())
	{
		PlayHighlightAniamtion();
	}
}

void UStorageSlotWidget::RefreshToolTip()
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

bool UStorageSlotWidget::IsAnyPopupActive()
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
