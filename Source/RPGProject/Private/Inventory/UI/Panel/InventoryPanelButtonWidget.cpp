// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/Panel/InventoryPanelButtonWidget.h"
#include "Components/Border.h"
#include "Header/DebugHeader.h"

void UInventoryPanelButtonWidget::NativeConstruct()
{
	if(!Button->OnClicked.IsBound())
	{
		Button->OnClicked.AddDynamic(this,&UInventoryPanelButtonWidget::OnPanelButtonClicked);
	}
}

void UInventoryPanelButtonWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	Button->SetStyle(ButtonStyleOff);
}

void UInventoryPanelButtonWidget::RefreshPanelButton(EInventoryPanel ActivePanel)
{
	if(CurrentPanel == ActivePanel)
	{
		Border->SetBrushColor(FLinearColor{1.f,1.f,1.f,1.f});
		Button->SetStyle(ButtonStyleOn);
	}
	else
	{
		Border->SetBrushColor(FLinearColor{0.f,0.f,0.f,0.f});
		Button->SetStyle(ButtonStyleOff);
	}
}

void UInventoryPanelButtonWidget::OnPanelButtonClicked()
{
	OnClickedButtonPanel.Broadcast(CurrentPanel);
}
