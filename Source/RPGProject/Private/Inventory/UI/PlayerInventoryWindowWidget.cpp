// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/PlayerInventoryWindowWidget.h"

#include "Interface/ComponentManager.h"
#include "Inventory/Components/InventoryComponent.h"
#include "Inventory/UI/PlayerInventoryWidget.h"
#include "Kismet/KismetInputLibrary.h"
#include "Kismet/KismetMathLibrary.h"

void UPlayerInventoryWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ForceLayoutPrepass();

	IComponentManager* ComponentManager = Cast<IComponentManager>(GetOwningPlayer());
	if(ComponentManager)
	{
		PlayerInventory = ComponentManager->GetInventoryComponent();
	}
	PlayConstructAnim();
}

FReply UPlayerInventoryWindowWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if(EKeys::F == UKismetInputLibrary::GetKey(InKeyEvent))
	{
		// CancelDragDrop
		PlayerInventory->Server_SortInventory(PlayerInventory,ESortMethod::Quicksort,PlayerInventory->ActivePanel,false);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UPlayerInventoryWindowWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	PlayerInventoryWidget->SetKeyboardFocus();
	return FReply::Handled();
}

FReply UPlayerInventoryWindowWidget::NativeOnAnalogValueChanged(const FGeometry& InGeometry,
	const FAnalogInputEvent& InAnalogEvent)
{
	float LocalAnalogValue = UKismetInputLibrary::GetAnalogValue(InAnalogEvent);

	if(UKismetMathLibrary::NearlyEqual_FloatFloat(LocalAnalogValue,0.f,0.1f))
	{
		return FReply::Handled();
	}

	// GetOwningPlayer AddPlayerCaptureRotation
	return FReply::Handled();
}

FReply UPlayerInventoryWindowWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FVector2D LocalCursorData = UKismetInputLibrary::PointerEvent_GetCursorDelta(InMouseEvent);
	if(LocalCursorData.X != 0 || LocalCursorData.Y != 0)
	{
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply UPlayerInventoryWindowWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	return FReply::Handled();
}
