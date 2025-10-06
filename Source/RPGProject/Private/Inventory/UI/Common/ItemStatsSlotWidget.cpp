// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/Common/ItemStatsSlotWidget.h"

#include "Components/TextBlock.h"
#include "Components/WidgetSwitcher.h"

void UItemStatsSlotWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if(IsDesignTime())
	{
		TextBlock_StatName->SetText(StatName);
		TextBlock_StatValue->SetText(FText::AsNumber(StatValue));
		TextBlock_ComparisonGreaterValue->SetText(FText::AsNumber(ComparisonValue));
	}
}

void UItemStatsSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(bShowComparison)
	{
		WidgetSwitcher_CompValue->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SetAppropriateComparisonBox();
	}
	else
	{
		WidgetSwitcher_CompValue->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UItemStatsSlotWidget::ShowComparisonToolTip(float InComparisonValue)
{
	ComparisonValue = InComparisonValue;
	bShowComparison = true;
	
	WidgetSwitcher_CompValue->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SetAppropriateComparisonBox();
}

void UItemStatsSlotWidget::HideComparisonToolTip()
{
	bShowComparison = false;
	
	WidgetSwitcher_CompValue->SetVisibility(ESlateVisibility::Collapsed);
}

FText UItemStatsSlotWidget::GetStatValue() const
{
	return FText::AsNumber(StatValue);
}

FText UItemStatsSlotWidget::GetComparisonValue() const
{
	return FText::AsNumber(StatValue - ComparisonValue);
}

FSlateColor UItemStatsSlotWidget::GetComparisonColor() const
{
	if((StatValue - ComparisonValue) == 0)
	{
		return FSlateColor{FLinearColor{0.442708f,0.442708f,0.442708f,1.f}};
	}
	return FSlateColor{};
}

void UItemStatsSlotWidget::SetAppropriateComparisonBox() const
{
	if((StatValue - ComparisonValue) > 0)
	{
		WidgetSwitcher_CompValue->SetActiveWidgetIndex(0);
	}
	else if((StatValue - ComparisonValue) == 0)
	{
		WidgetSwitcher_CompValue->SetActiveWidgetIndex(2);
		
	}
	else if((StatValue - ComparisonValue) < 0)
	{
		WidgetSwitcher_CompValue->SetActiveWidgetIndex(1);
	}
}
