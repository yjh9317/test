
#include "Inventory/UI/Items/DragDrop/ItemDataDragDropOperation.h"
#include "Components/Image.h"
#include "Inventory/UI/Items/DragDrop/ItemDragAndDropWidget.h"


void UItemDataDragDropOperation::ClearDraggableIcon()
{
	if(DraggableWidget)
	{
		DraggableWidget->Image_WrongSlot->SetVisibility(ESlateVisibility::Hidden);
		DraggableWidget->Image_SwapSlot->SetVisibility(ESlateVisibility::Hidden);
		DraggableWidget->Image_DropSlot->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UItemDataDragDropOperation::ShowIconDrop()
{
	ClearDraggableIcon();

	if(DraggableWidget)
	{
		DraggableWidget->Image_DropSlot->SetVisibility(ESlateVisibility::Visible);
	}
}

void UItemDataDragDropOperation::ShowIconSwap()
{
	ClearDraggableIcon();

	if(DraggableWidget)
	{
		DraggableWidget->Image_SwapSlot->SetVisibility(ESlateVisibility::Visible);
	}
}

void UItemDataDragDropOperation::ShowIconWrongSlot()
{
	ClearDraggableIcon();

	if(DraggableWidget)
	{
		DraggableWidget->Image_WrongSlot->SetVisibility(ESlateVisibility::Visible);
	}
}
