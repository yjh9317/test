// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/Items/DragDrop/ItemDragAndDropWidget.h"
#include "Components/Image.h"

void UItemDragAndDropWidget::NativeConstruct()
{
	FSlateBrush DraggableSlateBrush{};
	DraggableSlateBrush.ImageSize = DraggableImageSize;
	DraggableSlateBrush.SetResourceObject(DraggableImage);

	Image_Draggable->SetBrush(DraggableSlateBrush);
}
