// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RPGUserWidget.h"

void URPGUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}

void URPGUserWidget::SetFunctionTimer(FTimerDelegate& DelegateToSet, float Delay)
{
	if (!GetWorld()) return;

	FTimerHandle LocalTimerHandle{};
	GetWorld()->GetTimerManager().SetTimer(
		LocalTimerHandle,
		DelegateToSet,
		Delay,
		false
	);
}

void URPGUserWidget::NativeConstruct()
{
	GetWorld()->GetTimerManager().SetTimer(
	InitTimerHandle,
	this,
	&URPGUserWidget::InitializeWidget,
	TimerRate,
	false
	);

	// FTimerHandle LocalTimerHandle{};
	// GetWorld()->GetTimerManager().SetTimer(
	// 	LocalTimerHandle,
	// 	FTimerDelegate::CreateLambda([this](){ InitializeWidget();}),
	// 	1.f,
	// 	false
	// );
}
