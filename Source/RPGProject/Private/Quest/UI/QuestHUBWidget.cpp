// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/UI/QuestHUBWidget.h"

void UQuestHUBWidget::OpenQuestLog()
{
	/* Open/Close the Journal */
	bool bIsJournalVisible = QuestLogWidget->IsVisible();

	CloseAll();

	if (bIsJournalVisible)
	{
		PlayerController->SetInputMode(FInputModeGameOnly{});
		return;
	}

	if (!PlayerController)
		return;

	PlayerController->SetInputMode(FInputModeUIOnly{});
	PlayerController->SetShowMouseCursor(true);

	QuestLogWidget->SetVisibility(ESlateVisibility::Visible);
	QuestLogWidget->SetKeyboardFocus();
}

void UQuestHUBWidget::OpenQuestGiverSummary()
{
	CloseAll();

	if (!PlayerController)
		return;

	PlayerController->SetInputMode(FInputModeUIOnly{});
	PlayerController->SetShowMouseCursor(true);
	PlayerController->SetIgnoreMoveInput(true);

	/* Open the Quest Giver Summary */
	QuestGiverWidget->SetVisibility(ESlateVisibility::Visible);
	QuestGiverWidget->SetKeyboardFocus();	
}

void UQuestHUBWidget::CloseAll()
{
	/* Close all the widgets */
	QuestLogWidget->SetVisibility(ESlateVisibility::Collapsed);
	QuestGiverWidget->SetVisibility(ESlateVisibility::Collapsed);

	if (!PlayerController)
		return;

	PlayerController->SetShowMouseCursor(false);
	PlayerController->SetIgnoreMoveInput(false);

}

void UQuestHUBWidget::OnQuestEnableBroadcast(URPGQuest* quest)
{
	if (OnQuestEnableDelegate.IsBound())
		OnQuestEnableDelegate.Broadcast(quest);
}