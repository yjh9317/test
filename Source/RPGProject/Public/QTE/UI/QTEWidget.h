// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QTE/Components/QTEComponent.h"
#include "UI/KeyWidgets/KeyWidget.h"
#include "QTEWidget.generated.h"

class UCanvasPanel;
struct FScreenPosition;

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UQTEWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// UKeyWidget* AddWidget(UTexture2D* KeyIcon, FScreenPosition ScreenPosition, EQTEStatus QteStatus);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UKeyWidget> KeyWidgetClass;

	UPROPERTY()
	TObjectPtr<UKeyWidget> KeyWidget;

	UPROPERTY(meta=(BindWidget))
	UCanvasPanel* CanvasPanel;
};
