// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/PlayerInventorySlotWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Header/DebugHeader.h"
#include "Interface/WidgetManager.h"
#include "Interface/WidgetSlotListener.h"
#include "Inventory/Components/EquipmentComponent.h"
#include "Inventory/Components/InventoryCoreComponent.h"
#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"
#include "Inventory/UI/Items/DragDrop/ItemDataDragDropOperation.h"
#include "Inventory/UI/Items/DragDrop/ItemDragAndDropWidget.h"
#include "Inventory/UI/Items/ToolTip/ItemToolTipWidget.h"
#include "Kismet/KismetInputLibrary.h"


void UPlayerInventorySlotWidget::NativeConstruct()
{
	bIsFocusable = true;
	
	RefreshSlotDisplay();
	InitializeWidgetReferences();
	BindEventHandlers();
}

void UPlayerInventorySlotWidget::NativeDestruct()
{
	if (ImageLoadHandle.IsValid())
	{
		ImageLoadHandle->CancelHandle();
		ImageLoadHandle.Reset();
	}

	if (ItemToolTipWidget)
	{
		// ItemToolTipWidget->SetVisibility(ESlateVisibility::Hidden);
		ItemToolTipWidget->RemoveFromParent();
		ItemToolTipWidget = nullptr;
	}

	if (Button_Item)
	{
		Button_Item->OnClicked.RemoveAll(this);
		Button_Item->OnHovered.RemoveAll(this);
		Button_Item->OnUnhovered.RemoveAll(this);
		Button_Item->SetToolTip(nullptr);
	}

	ReceiverInventory = nullptr;

	Super::NativeDestruct();
}

void UPlayerInventorySlotWidget::SetItemImageAsync()
{
	if (!Image_Item || !ItemData.ItemClass)
	{
		Image_Item->SetColorAndOpacity(FLinearColor{1.f,1.f,1.f,0.f});
		return;
	}
	
    if (ImageLoadHandle.IsValid())
    {
        ImageLoadHandle->CancelHandle();
        ImageLoadHandle.Reset();
    }
    
    UItemBase* ItemBase = Cast<UItemBase>(ItemData.ItemClass->GetDefaultObject());
    if (!ItemBase)
    {
        return;
    }

    if (ItemBase->Image.IsValid())
    {
        if (UTexture2D* Texture = Cast<UTexture2D>(ItemBase->Image.Get()))
        {
            Image_Item->SetBrushFromTexture(Texture);
        	Image_Item->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
        	Image_Item->SetColorAndOpacity(FLinearColor::White);
            return;
        }
    }
    
    FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();

    TWeakObjectPtr<UPlayerInventorySlotWidget> WeakThis = this;
    TWeakObjectPtr<UImage> WeakImage = Image_Item;
    TSoftObjectPtr<UTexture2D> TexturePath = ItemBase->Image;
    
    ImageLoadHandle = StreamableManager.RequestAsyncLoad(
        TexturePath.ToSoftObjectPath(),
        FStreamableDelegate::CreateLambda([WeakThis, WeakImage, TexturePath]()
        {
            if (!WeakThis.IsValid() || !WeakImage.IsValid())
            {
                return;
            }
            
            if (UTexture2D* LoadedTexture = Cast<UTexture2D>(TexturePath.Get()))
            {
                WeakImage->SetBrushFromTexture(LoadedTexture);
                WeakImage->SetColorAndOpacity(FLinearColor::White);
            }
        })
    );
}

void UPlayerInventorySlotWidget::RefreshSlotDisplay()
{
	SetItemImageAsync();
	SetItemQuantity();
	RefreshToolTip();
	SetInTradeIcon();
}

void UPlayerInventorySlotWidget::CreateItemTooltip()
{
	HideItemComparison();

	if(ItemToolTipWidget)
	{
		ItemToolTipWidget->RemoveFromParent();
		ItemToolTipWidget = CreateWidget<UItemToolTipWidget>(this,ItemToolTipWidgetClass);
		Button_Item->SetToolTip(ItemToolTipWidget);
	}
}


FReply UPlayerInventorySlotWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
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

FReply UPlayerInventorySlotWidget::NativeOnKeyUp(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
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

FReply UPlayerInventorySlotWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	FEventReply Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent,this,EKeys::LeftMouseButton);
	return Reply.NativeReply;
}

FReply UPlayerInventorySlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	if(EKeys::RightMouseButton == UKismetInputLibrary::PointerEvent_GetEffectingButton(InMouseEvent))
	{
		OnRightClick();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UPlayerInventorySlotWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	// UE_LOG(LogTemp, Warning, TEXT("Mouse Move detected on slot %d"), SlotIndex);
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

void UPlayerInventorySlotWidget::OnRightClick()
{
	IWidgetSlotListener* SlotListener = Cast<IWidgetSlotListener>(ParentWidget);
	
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData) && SlotListener)
	{
		SlotListener->OnSlotClicked(this);
	}
}

void UPlayerInventorySlotWidget::InitializeWidgetReferences()
{
	if (!Button_Item || !Border)
	{
		return;
	}

	if(ButtonBackgroundImage)
	{
		FButtonStyle ButtonStyle;

		ButtonStyle.Normal.SetResourceObject(ButtonBackgroundImage);
		ButtonStyle.Hovered.SetResourceObject(ButtonBackgroundImage);
		ButtonStyle.Pressed.SetResourceObject(ButtonBackgroundImage);
		Button_Item->SetStyle(ButtonStyle);
	}
    
	Border->SetBrushColor(UnHoveredBorderColor);
}

void UPlayerInventorySlotWidget::BindEventHandlers()
{
	if (!Button_Item) return;
	
	Button_Item->OnClicked.RemoveAll(this);
	Button_Item->OnClicked.AddDynamic(this, &UPlayerInventorySlotWidget::OnItemButtonClicked);
    
	Button_Item->OnHovered.RemoveAll(this);
	Button_Item->OnHovered.AddDynamic(this, &UPlayerInventorySlotWidget::OnItemButtonHovered);
    
	Button_Item->OnUnhovered.RemoveAll(this);
	Button_Item->OnUnhovered.AddDynamic(this, &UPlayerInventorySlotWidget::OnItemButtonUnhovered);
}


void UPlayerInventorySlotWidget::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
                                                      UDragDropOperation*& OutOperation)
{
	OutOperation = nullptr;
    
	if (!UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		return;
	}
    
	if (!PlayerInventory)
	{
		DebugHeader::LogError(TEXT("NativeOnDragDetected: PlayerInventory is null"));
		return;
	}

	if (!ItemDragAndDropWidgetClass || !ItemDataDragDropOperationClass)
	{
		DebugHeader::LogError(TEXT("NativeOnDragDetected: Required drag classes are null"));
		return;
	}

	UItemBase* ItemBase = Cast<UItemBase>(ItemData.ItemClass->GetDefaultObject());
	if (!ItemBase)
	{
		DebugHeader::LogWarning(TEXT("NativeOnDragDetected: ItemBase is null"));
		return;
	}

	UItemDragAndDropWidget* DragWidget = CreateWidget<UItemDragAndDropWidget>(
		GetOwningPlayer(), 
		ItemDragAndDropWidgetClass);
    
	if (!DragWidget)
	{
		return;
	}

	if (ItemBase->Image.IsValid())
	{
		DragWidget->DraggableImage = ItemBase->Image.Get();
	}
	DragWidget->DraggableImageSize = DraggedImageSize;

	UItemDataDragDropOperation* DragOperation = NewObject<UItemDataDragDropOperation>(
		this, 
		ItemDataDragDropOperationClass,
		NAME_None,
		RF_Transient);
    
	if (!DragOperation)
	{
		DragWidget->RemoveFromParent();
		return;
	}
  
	DragOperation->DefaultDragVisual = DragWidget;
	DragOperation->DraggableWidget = DragWidget;
	DragOperation->ItemData = ItemData;
	DragOperation->InventoryComponent = PlayerInventory;
	DragOperation->DraggedFrom = EItemDestination::InventorySlot;
    
	OutOperation = DragOperation;
}

bool UPlayerInventorySlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	UItemDataDragDropOperation* ItemDataDragDrop = Cast<UItemDataDragDropOperation>(InOperation);
	if(bCanDrop && ItemDataDragDrop)
	{
		if(UKismetInputLibrary::InputEvent_IsShiftDown(InDragDropEvent))
		{
			if(AttemptSplitting(ItemDataDragDrop))
			{
				Border->SetBrushColor(UnHoveredBorderColor);
				return true;
			}
		}
		else
		{
			EItemDestination Destination = ItemDataDragDrop->DraggedFrom;
			switch (Destination)
			{
			case EItemDestination::InventorySlot:
				DraggedFromInventory(ItemDataDragDrop); break;
			case EItemDestination::EquipmentSlot:
				DraggedFromEquipment(ItemDataDragDrop); break;
			case EItemDestination::VendorSlot:
			case EItemDestination::StorageSlot:
				DraggedFromOtherInventory(ItemDataDragDrop);
				break;
			case EItemDestination::None:
			case EItemDestination::DropBar:
				default:
				break;
			}
		}
	}
	return false;
}

bool UPlayerInventorySlotWidget::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	UItemDataDragDropOperation* ItemDragDropOperation = Cast<UItemDataDragDropOperation>(InOperation);
	
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData) && ItemDragDropOperation)
	{
		EItemDestination Destination = ItemDragDropOperation->DraggedFrom;
		const FItemData& DragAndDropItemData = ItemDragDropOperation->ItemData;
		
		switch (Destination)
		{
		case EItemDestination::InventorySlot:
			break;
		case EItemDestination::EquipmentSlot:
			{
				// Dragged from - Equipment
				bool bCanSwap = false;
				
				if (UInventoryUtilityLibrary::AreItemSlotsEqual(DragAndDropItemData, ItemData))
				{
					// 조건 A: 같은 아이템 슬롯일 경우
					bCanSwap = PlayerEquipment->CanItemBeEquipped(ItemData);
				}
				else
				{
					// 조건 B: 다른 아이템 슬롯일 경우 (무기 스왑 등)
					bCanSwap = PlayerEquipment->EnableOffHand &&
							   UInventoryUtilityLibrary::CanWeaponsBeSwapped(DragAndDropItemData, ItemData) &&
							   PlayerEquipment->CanItemBeEquipped(ItemData);
				}
				
				if (bCanSwap)
				{
					ItemDragDropOperation->ShowIconSwap();
					Border->SetBrushColor(BorderGreenColor);
				}
				else
				{
					ItemDragDropOperation->ShowIconWrongSlot();
					Border->SetBrushColor(BorderRedColor);
				}
				return true;
				break;
			}
		case EItemDestination::VendorSlot:
		case EItemDestination::StorageSlot:
			// Dragged from - Vendor/Storage
			if(UInventoryUtilityLibrary::AreItemsTheSame(DragAndDropItemData,ItemData) &&
					UInventoryUtilityLibrary::AreItemsStackable(DragAndDropItemData,ItemData))
			{
				// Show Drop Slot and Green border color
				ItemDragDropOperation->ShowIconDrop();
				Border->SetBrushColor(BorderGreenColor);
				return true;	
			}
			else
			{
				// Show Wrong Slot and Red border color
				ItemDragDropOperation->ShowIconWrongSlot();
				Border->SetBrushColor(BorderRedColor);
				return true;	
			}
			break;
		case EItemDestination::None:
		case EItemDestination::DropBar:
			default:
			break;
		}
	}
	
	return false;
}

void UPlayerInventorySlotWidget::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	UItemDataDragDropOperation* ItemDragDropOperation = Cast<UItemDataDragDropOperation>(InOperation);
	if(ItemDragDropOperation)
	{
		ItemDragDropOperation->ClearDraggableIcon();
		Border->SetBrushColor(UnHoveredBorderColor);
	}
}


void UPlayerInventorySlotWidget::DraggedFromEquipment(const UItemDataDragDropOperation* DragDrop)
{
	const FItemData& DragAndDropItemData = DragDrop->ItemData;
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		// Swap Items if both have similar Equipment Slot
		if(UInventoryUtilityLibrary::AreItemSlotsEqual(DragAndDropItemData, ItemData) ||
			(PlayerEquipment->EnableOffHand && UInventoryUtilityLibrary::CanWeaponsBeSwapped(DragAndDropItemData, ItemData)))
		{
			PlayerEquipment->ServerTransferItemFromInventory(PlayerInventory,ItemData,DragAndDropItemData,EInputMethod::DragAndDrop);
		}
		// UInventoryUtilityLibrary::PlaySoundOnItemDropped
	}
	else
	{
		// SERVER: Transfer Item from Equipment to Inventory
		PlayerEquipment->ServerTransferItemFromInventory(PlayerInventory,ItemData,DragAndDropItemData,EInputMethod::DragAndDrop);
	}
}

void UPlayerInventorySlotWidget::DraggedFromInventory(const UItemDataDragDropOperation* DragDrop)
{
	const FItemData& DragAndDropItemData = DragDrop->ItemData;
	if(DragAndDropItemData.Index != ItemData.Index)
	{
		// SERVER: Move Item to Slot
		EInventoryPanel DragDropPanel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(DragAndDropItemData);
		int32 DragDropIndex = DragAndDropItemData.Index;
		PlayerInventory->Server_MoveItemToSlot(PlayerInventory,DragDropPanel,DragDropIndex,ItemData.Index);
		
	}
}

void UPlayerInventorySlotWidget::DraggedFromOtherInventory(const UItemDataDragDropOperation* DragDrop)
{
	const FItemData& DragAndDropItemData = DragDrop->ItemData;
	// !IsItemClassValid == Empty
	if (!UInventoryUtilityLibrary::IsItemClassValid(ItemData) ||
		(UInventoryUtilityLibrary::AreItemsTheSame(DragAndDropItemData, ItemData) &&
		 UInventoryUtilityLibrary::AreItemsStackable(DragAndDropItemData, ItemData)))
	{
		UInventoryCoreComponent* Sender = DragDrop->InventoryComponent;
		if(UInventoryUtilityLibrary::IsStackableAndHaveStacks(DragAndDropItemData,1))
		{
			
			// Show Split Stack Widget
			if(IWidgetManager* WidgetManager = Cast<IWidgetManager>(GetOwningPlayer()))
			{
				WidgetManager->OpenSplitStackPopup(Sender,PlayerInventory,DragAndDropItemData,
					ItemData,EInputMethod::DragAndDrop,
					DragDrop->DraggedFrom,EItemDestination::InventorySlot,nullptr);

				Border->SetBrushColor(UnHoveredBorderColor);
			}
		}
		else
		{
			// SERVER:Transfer Item to Player Inventory

			PlayerInventory->Server_TransferItemFromInventory(PlayerInventory,
				Sender,DragAndDropItemData,ItemData,
				EInputMethod::DragAndDrop,GetOwningPlayer());
		}
	}
}

void UPlayerInventorySlotWidget::ShowItemComparison()
{
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		UItemBase* ItemBase = Cast<UItemBase>(ItemData.ItemClass->GetDefaultObject());
		if(nullptr == ItemBase) return;

		if(ItemBase->Rarity != EItemRarity::Consumable && ItemToolTipWidget)
		{
			ItemToolTipWidget->ShowComparisonToolTip();
		}
	}
}

void UPlayerInventorySlotWidget::HideItemComparison()
{
	if(ItemToolTipWidget)
	{
		ItemToolTipWidget->HideComparisonToolTip();
	}
}

void UPlayerInventorySlotWidget::EmptySlot()
{
	ItemData.Index = SlotIndex;
	SetItemImageAsync();
	SetItemQuantity();
	Button_Item->SetToolTip(nullptr);
	if(ItemToolTipWidget)
	{
		ItemToolTipWidget->RemoveFromParent();
	}
}

bool UPlayerInventorySlotWidget::AttemptSplitting(UItemDataDragDropOperation* InDragDropOperation)
{
	APlayerController* LocalPC = GetOwningPlayer();
	if( !InDragDropOperation && !LocalPC && 	!PlayerInventory)
	{
		return false;
	}

	if(InDragDropOperation->DraggedFrom != EItemDestination::InventorySlot &&
	   InDragDropOperation->DraggedFrom != EItemDestination::StorageSlot)
	{
		return false;
	}
	
	const FItemData& DragAndDropItemData = InDragDropOperation->ItemData;

	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData) &&
		!UInventoryUtilityLibrary::AreItemsTheSame(DragAndDropItemData,ItemData))
	{
		return false;
	}

	if(!UInventoryUtilityLibrary::IsStackableAndHaveStacks(DragAndDropItemData,1))
	{
		return false;
	}
	
	IWidgetManager* WidgetManager = Cast<IWidgetManager>(LocalPC);
	if(WidgetManager)
	{
		WidgetManager->OpenSplitStackPopup(
			nullptr,
			PlayerInventory,
			DragAndDropItemData,
			ItemData,
			EInputMethod::DragAndDrop,
			InDragDropOperation->DraggedFrom,
			EItemDestination::InventorySlot,
			nullptr);
	}
	
	return true;
}

void UPlayerInventorySlotWidget::HighlightSlot()
{
	if(!HasUserFocusedDescendants(GetOwningPlayer()) && !IsHovered())
	{
		PlayHighlightAnimation();
	}
}

void UPlayerInventorySlotWidget::RefreshToolTip()
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

void UPlayerInventorySlotWidget::SetItemQuantity()
{
	if (!TextBlock_Quantity)
	{
		DebugHeader::LogWarning(TEXT("SetItemQuantity: Quantity widget is null"));
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

bool UPlayerInventorySlotWidget::IsAnyPopupActive()
{
	IWidgetManager* WidgetManager = Cast<IWidgetManager>(GetOwningPlayer());
	if(WidgetManager && WidgetManager->GetActivePopup() != EWidgetPopup::None)
	{
		return true;
	}
	return false;
}

void UPlayerInventorySlotWidget::SetInTradeIcon()
{
	Image_ItemInTrade->SetVisibility(bItemInTrade ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
}

void UPlayerInventorySlotWidget::OnItemButtonClicked()
{
	if(nullptr == ParentWidget) return;
	IWidgetSlotListener* SlotListener = Cast<IWidgetSlotListener>(ParentWidget);
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData) && SlotListener)
	{
		SlotListener->OnSlotClicked(this);
	}
}

void UPlayerInventorySlotWidget::OnItemButtonHovered()
{
	if(nullptr == ParentWidget || nullptr == Border) return;
	
	if(Border)
	{
		Border->SetBrushColor(HoveredBorderColor);
	}
	IWidgetSlotListener* SlotListener = Cast<IWidgetSlotListener>(ParentWidget);
	
	if(!IsAnyPopupActive())
	{
		StopHighlightAnimation();
		if(SlotListener)
		{
			SlotListener->OnSlotHovered(this);
		}
	}
}

void UPlayerInventorySlotWidget::OnItemButtonUnhovered()
{
	if(nullptr == ParentWidget) return;
	
	if(Button_Item)
	{
		Button_Item->SetToolTip(nullptr);
	}

	if(Border)
	{
		Border->SetBrushColor(UnHoveredBorderColor);
	}
	
	StopHighlightAnimation();
	HideItemComparison();

	IWidgetSlotListener* SlotListener = Cast<IWidgetSlotListener>(ParentWidget);
	if(ItemToolTipWidget)
	{
		ItemToolTipWidget->RemoveFromParent();
		if(SlotListener)
		{
			SlotListener->OnSlotUnhovered(this);
		}
	}
}
