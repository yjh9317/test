// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/Common/ItemConfirmationPopupWidget.h"
#include "Engine/AssetManager.h"
#include "Interface/ComponentManager.h"
#include "Interface/WidgetManager.h"
#include "Inventory/Components/InventoryComponent.h"
#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"
#include "Item/ItemBase.h"
#include "Kismet/KismetInputLibrary.h"

void UItemConfirmationPopupWidget::NativeConstruct()
{
	Super::NativeConstruct();
	if(IComponentManager* ComponentManager = Cast<IComponentManager>(GetOwningPlayer()))
	{
		PlayerInventory = ComponentManager->GetInventoryComponent();
	}
	UInventoryUtilityLibrary::SetItemImageTextureAsync(Image_ItemIcon,ItemData);
	PlayConstructAnimation();
	
}

FReply UItemConfirmationPopupWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if(EKeys::E == UKismetInputLibrary::GetKey(InKeyEvent))
	{
		OnPressedAccept();
		return FReply::Handled();
	}

	if(EKeys::R == UKismetInputLibrary::GetKey(InKeyEvent))
	{
		OnPressedCancel();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UItemConfirmationPopupWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	return FReply::Handled();
}

FReply UItemConfirmationPopupWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	return FReply::Handled();
}

void UItemConfirmationPopupWidget::OnPressedAccept()
{
	if(SenderWidget)
	{
		SetKeyboardFocus();
	}

	if(UInventoryUtilityLibrary::IsStackableAndHaveStacks(ItemData,1))
	{
		if(IWidgetManager* WidgetManager = Cast<IWidgetManager>(GetOwningPlayer()))
		{
			WidgetManager->OpenSplitStackPopup(Sender,Receiver,ItemData,InSlotItemData,InputMethod,Initiator,Destination,SenderWidget);
		}
	}
	else
	{
		PlayerInventory->Server_ConfirmationPopupAccepted(Receiver,Sender,ItemData,InSlotItemData,InputMethod,Initiator,Destination,GetOwningPlayer());
	}
}

void UItemConfirmationPopupWidget::OnPressedCancel()
{
	if(SenderWidget)
	{
		SetKeyboardFocus();
	}
}

FText UItemConfirmationPopupWidget::GetName() const
{
	UItemBase* ItemBase = ItemData.ItemClass.GetDefaultObject();
	if(ItemBase)
	{
		return ItemBase->Name;
	}
	return {};
}

FText UItemConfirmationPopupWidget::GetQuestionText() const
{
	return QuestionText;
}

TSoftObjectPtr<UTexture2D> UItemConfirmationPopupWidget::GetInputIconAccept() const
{
	if(InputAcceptTexture.IsValid())
	{
		return InputAcceptTexture;
	}
	return nullptr;
}	

TSoftObjectPtr<UTexture2D> UItemConfirmationPopupWidget::GetInputIconCancel() const
{
	if(InputCancelTexture.IsValid())
	{
		return InputCancelTexture;
	}
	return nullptr;
}

FText UItemConfirmationPopupWidget::GetItemType() const
{
	UItemBase* ItemBase = ItemData.ItemClass.GetDefaultObject();
	FText StatusText;
	if(ItemBase)
	{
		FString StatusString;
		const UEnum* EnumPtr = FindObject<UEnum>(nullptr, TEXT("/Script/RPGProject.EItemType"));
		if (EnumPtr)
		{
			StatusString = EnumPtr->GetNameStringByValue(static_cast<int64>(ItemBase->Type));
		}
		StatusText = FText::FromString(StatusString);
	}
	return StatusText;
}

FText UItemConfirmationPopupWidget::GetItemRarity() const
{
	UItemBase* ItemBase = ItemData.ItemClass.GetDefaultObject();
	FText StatusText;
	if(ItemBase)
	{
		FString StatusString;
		const UEnum* EnumPtr = FindObject<UEnum>(nullptr, TEXT("/Script/RPGProject.EItemRarity"));
		if (EnumPtr)
		{
			StatusString = EnumPtr->GetNameStringByValue(static_cast<int64>(ItemBase->Rarity));
		}
		StatusText = FText::FromString(StatusString);
	}
	return StatusText;
}

FSlateColor UItemConfirmationPopupWidget::GetItemRarityColor()
{
	UItemBase* ItemBase = ItemData.ItemClass.GetDefaultObject();

	if(ItemBase)
	{
		return UInventoryUtilityLibrary::GetRarityColor(ItemBase->Rarity);
	}
	return FSlateColor();
}
