// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/Common/SortSlotWidget.h"

#include "Components/Border.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"


void USortSlotWidget::NativePreConstruct()
{
	TextBlock_TitleText->SetText(Title);
}

void USortSlotWidget::NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent)
{
	Border_Background->SetBrushColor(FLinearColor{1.f,1.f,1.f,1.f});
}

void USortSlotWidget::NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent)
{
	Border_Background->SetBrushColor(FLinearColor{1.f,1.f,1.f,0.f});
}

FReply USortSlotWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	Button_Background->SetKeyboardFocus();
	return FReply::Handled();
}

void USortSlotWidget::EventOnSortButtonClick()
{
	OnSortButtonClicked.Broadcast(SortMethod);
}

void USortSlotWidget::EventOnSortButtonHover()
{
	Border_Background->SetBrushColor(FLinearColor{1.f,1.f,1.f,1.f});
}

void USortSlotWidget::EventOnSortButtonUnHover()
{
	Border_Background->SetBrushColor(FLinearColor{1.f,1.f,1.f,0.f});
}
