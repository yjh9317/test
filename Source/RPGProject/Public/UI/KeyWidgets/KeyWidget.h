// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KeyWidget.generated.h"


class UImage;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UKeyWidget : public UUserWidget
{
	GENERATED_BODY()

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetAnimation> UpdateAnimation;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetAnimation> PendingAnimation;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Key;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UTexture2D> QTE_Key;

	// UPROPERTY(EditDefaultsOnly)
	// EQTEStatus QTEStatus;
	//
	// FSlateColor Fail;
	// FSlateColor Success;
	// FSlateColor Pending;
	//
	// virtual void NativeConstruct() override;
	// void QTEUpdate(EQTEStatus Status);
};
