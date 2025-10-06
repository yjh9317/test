// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/LootBar/LootBarSlotWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Header/UEnemyTypes.h"
#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"
#include "Inventory/UI/LootBar/LootBarWidget.h"
#include "Item/ItemBase.h"
#include "Kismet/KismetInputLibrary.h"

void ULootBarSlotWidget::NativeConstruct()
{
	SetItemDetails();

	if(Button_Background)
	{
		Button_Background->OnClicked.AddDynamic(this,&ULootBarSlotWidget::OnRightClick);
		Button_Background->OnHovered.AddDynamic(this,&ULootBarSlotWidget::EventButtonBackgroundHover);
	}
}

FReply ULootBarSlotWidget::NativeOnPreviewMouseButtonDown(const FGeometry& InGeometry,const FPointerEvent& InMouseEvent)
{
	FKey EffectingKey = UKismetInputLibrary::PointerEvent_GetEffectingButton(InMouseEvent);
	if(EffectingKey == EKeys::RightMouseButton)
	{
		OnRightClick();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply ULootBarSlotWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FVector2D MouseCursorDelta = UKismetInputLibrary::PointerEvent_GetCursorDelta(InMouseEvent);
	if(MouseCursorDelta.X != 0 || MouseCursorDelta.Y !=0)
	{
		if(LootBarWidget)
		{
			LootBarWidget->bKeyboardFocus = false;
		}
		EventMouseMoved();
		return FReply::Handled();
	}
	return FReply::Unhandled();
}


void ULootBarSlotWidget::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	StopAnimationBorder();
	Image_Border->SetColorAndOpacity(FLinearColor{1.f,1.f,1.f,0.f});
}

void ULootBarSlotWidget::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	StopAnimationBorder();
	Image_Border->SetColorAndOpacity(FLinearColor{1.f,1.f,1.f,0.f});
}

void ULootBarSlotWidget::SetItemDetails()
{
	UInventoryUtilityLibrary::SetItemImageTextureAsync(Image_Item,ItemData);
	SetItemName();
	SetItemType();
	SetItemRarity();
	SetItemRarityColor();
	SetItemQuantity();
}

void ULootBarSlotWidget::SetItemName()
{
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		UItemBase* ItemBase = Cast<UItemBase>(ItemData.ItemClass->GetDefaultObject());
		if(ItemBase)
		{
			TextBlock_Name->SetText(ItemBase->Name);
		}
	}
}

void ULootBarSlotWidget::SetItemType()
{
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		UItemBase* ItemBase = Cast<UItemBase>(ItemData.ItemClass->GetDefaultObject());
		if(ItemBase)
		{
			FString EnumString = EnumToString(ItemBase->Type);
			TextBlock_Type->SetText(FText::FromString(EnumString));
		}
	}
}

void ULootBarSlotWidget::SetItemRarity()
{
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		UItemBase* ItemBase = Cast<UItemBase>(ItemData.ItemClass->GetDefaultObject());
		if(ItemBase)
		{
			FString EnumString = EnumToString(ItemBase->Rarity);
			TextBlock_Rarity->SetText(FText::FromString(EnumString));
		}
	}
}

void ULootBarSlotWidget::SetItemRarityColor()
{
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		UItemBase* ItemBase = Cast<UItemBase>(ItemData.ItemClass->GetDefaultObject());
		if(ItemBase)
		{
			FSlateColor RarityColor = UInventoryUtilityLibrary::GetRarityColor(ItemBase->Rarity);
			TextBlock_Rarity->SetColorAndOpacity(RarityColor);
		}
	}
}

void ULootBarSlotWidget::SetItemQuantity()
{
	if(UInventoryUtilityLibrary::IsItemClassValid(ItemData))
	{
		UItemBase* ItemBase = Cast<UItemBase>(ItemData.ItemClass->GetDefaultObject());
		if(ItemBase && ItemBase->Stackable && ItemData.Quantity >= 1)
		{
			TextBlock_Quantity->SetText(FText::AsNumber(ItemData.Quantity));			
		}
		else
		{
			TextBlock_Quantity->SetText(FText::GetEmpty());
		}
	}
}

void ULootBarSlotWidget::OnRightClick()
{
	if(LootBarWidget)
	{
		LootBarWidget->TakeItem();
	}
}
void ULootBarSlotWidget::EventMouseMoved()
{
	if(HasKeyboardFocus())
	{
		SetKeyboardFocus();
		Image_Border->SetColorAndOpacity(FLinearColor{1.f,1.f,1.f,1.f});
		if(LootBarWidget)
		{
			LootBarWidget->SetCurrentlyFocusedSlot(this);
			PlayAnimationBorder();
		}
	}
}

void ULootBarSlotWidget::EventButtonBackgroundHover()
{
	if(!LootBarWidget->bKeyboardFocus)
	{
		SetKeyboardFocus();
		Image_Border->SetColorAndOpacity(FLinearColor{1.f,1.f,1.f,1.f});
		if(LootBarWidget)
		{
			LootBarWidget->SetCurrentlyFocusedSlot(this);
			PlayAnimationBorder();
		}
	}
}

