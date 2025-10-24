// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/MultiPlayerTrading/TradingSplitStackWidget.h"

void UTradingSplitStackWidget::NativeConstruct()
{
	
	if(Slider)
	{
		// Slider->OnValueChanged.AddDynamic()
	}
}

void UTradingSplitStackWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

FReply UTradingSplitStackWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

FReply UTradingSplitStackWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	return Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);
}

FReply UTradingSplitStackWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UTradingSplitStackWidget::OnPressAccept()
{
}

void UTradingSplitStackWidget::OnPressCancel()
{
}

void UTradingSplitStackWidget::OnSliderValueChanged()
{
}

void UTradingSplitStackWidget::SetInputIconAsync()
{
}

void UTradingSplitStackWidget::ShowSplitStackWidget(const FItemData& InItemData)
{
}

float UTradingSplitStackWidget::GetSliderValue() const
{
	return {};
}

FText UTradingSplitStackWidget::GetItemQuantity() const
{
	return {};
}

FString UTradingSplitStackWidget::SetCurrentQuantityText()
{
	return {};
}

void UTradingSplitStackWidget::SetCurrentWeight()
{
}

void UTradingSplitStackWidget::SetCurrentValue()
{
}

TSoftObjectPtr<UTexture2D> UTradingSplitStackWidget::GetInputIconAccept()
{
	return {};
}

TSoftObjectPtr<UTexture2D> UTradingSplitStackWidget::GetInputIconCancel()
{
	return {};
}

int32 UTradingSplitStackWidget::GetCurrentQuantity() const
{
	return {};
}

void UTradingSplitStackWidget::ModifyCurrentQuantity(int32 ValueToAdd)
{
}
