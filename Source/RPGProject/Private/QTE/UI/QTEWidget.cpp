// Fill out your copyright notice in the Description page of Project Settings.


#include "QTE/UI/QTEWidget.h"

#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "QTE/Components/QTEComponent.h"
#include "UI/KeyWidgets/KeyWidget.h"


// UKeyWidget* UQTEWidget::AddWidget(UTexture2D* KeyIcon, FScreenPosition ScreenPosition, EQTEStatus QteStatus)
// {
// 	KeyWidget = CreateWidget<UKeyWidget>(GetWorld(),KeyWidgetClass);
//
// 	if(KeyWidget && CanvasPanel)  
// 	{
// 		UCanvasPanelSlot* PanelSlot = CanvasPanel->AddChildToCanvas(KeyWidget);
// 		PanelSlot->SetAutoSize(true);
// 		PanelSlot->SetAnchors(ScreenPosition.Anchors);
// 		PanelSlot->SetAlignment(ScreenPosition.Alignment);
// 		PanelSlot->SetPosition(ScreenPosition.Offset);
// 		return KeyWidget;
// 	}
// 	return nullptr;
// }
