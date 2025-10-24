// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/Channels/QuestChannel.h"
#include "Blueprint/UserWidget.h"
#include "Quest/Channels/Quest_AudioChannel.h"
#include "Quest/Data/RPGQuestData.h"
#include "Quest/Data/QuestSounds.h"
#include "Quest/Data/Enum/QuestRequiermentEventType.h"


void UQuestChannel::AddWidgetToViewport()
{
	/* Check if there is a QuestWidgets Widget class */
	if (!QuestWidgetsClass)
		return;
	
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (!PlayerController)
		return;
	
	/* Create the QuestWidgets widget and add it to the viewport */
	// QuestWidgets = CreateWidget<UAQ_BookQuest>(PlayerController, QuestWidgetsClass);
	// if (!QuestWidgets)
	// 	return;
	//
	// QuestWidgets->AddToViewport();
}

void UQuestChannel::OnQuestStateChanged(URPGQuest* QuestUpdate, EQuestState QuestState)
{
	switch (QuestState)
	{
	case EQuestState::Active:
	{
		/* Update the Book Quest */
		// if (QuestWidgets)
		// 	QuestWidgets->UpdateQuestWidgets(QuestUpdate);
		break;
	}
	case EQuestState::Valid:
	{
		AudioChannel->Play2DSound(this, SoundBank->QuestValid);

		/* Update the Book Quest */
		// if (QuestWidgets)
		// 	QuestWidgets->UpdateQuestWidgets(QuestUpdate);
		break;
	}
	case EQuestState::Failed:
	{
		AudioChannel->Play2DSound(this, SoundBank->QuestFailed);

		/* Update the Book Quest */
		// if (QuestWidgets)
		// 	QuestWidgets->UpdateQuestWidgets(QuestUpdate);
		break;
	}

	case EQuestState::Pending:
	{
		AudioChannel->Play2DSound(this, SoundBank->AbandonQuest);

		/* Remove the Quest from the Book Quest */
		// if (QuestWidgets)
		// 	QuestWidgets->RemoveQuest(QuestUpdate);

		/* Remove the Quest Channel to the Quest Update delegate*/
		QuestUpdate->PositiveObjectiveUpdateDelegate.RemoveDynamic(this, &UQuestChannel::OnQuestProgress);
		QuestUpdate->NegativeObjectiveUpdateDelegate.RemoveDynamic(this, &UQuestChannel::OnQuestRegression);
		QuestUpdate->QuestStateChangedDelegate.RemoveDynamic(this, &UQuestChannel::OnQuestStateChanged);
		break;
	}
	case EQuestState::Archive:
	{
		PlayQuestEndSound(QuestUpdate);

		/* Remove the Quest from the Book Quest */
		// if (QuestWidgets)
		// 	QuestWidgets->RemoveQuest(QuestUpdate);

		/* Remove the Quest Channel to the Quest Update delegate*/
		QuestUpdate->PositiveObjectiveUpdateDelegate.RemoveDynamic(this, &UQuestChannel::OnQuestProgress);
		QuestUpdate->NegativeObjectiveUpdateDelegate.RemoveDynamic(this, &UQuestChannel::OnQuestRegression);
		QuestUpdate->QuestStateChangedDelegate.RemoveDynamic(this, &UQuestChannel::OnQuestStateChanged);

		if(QuestCompletionDelegate.IsBound())
			QuestCompletionDelegate.Broadcast(QuestUpdate);

		break;
	}
	}

	if (QuestState == EQuestState::Pending)
	{
		FQuestRequirementData& requirements = QuestUpdate->QuestData->QuestRequirements;

		/* Unbind from delegates */
		if (requirements.SpecialEvents.Num() > 0)
			SpecialEventTriggerDelegate.RemoveDynamic(QuestUpdate, &URPGQuest::OnEventRequirementChange);

		if (requirements.PlayerLevel != 0)
			LevelRequirementChangedDelegate.RemoveDynamic(QuestUpdate, &URPGQuest::OnLevelRequirementChange);
	}
}

void UQuestChannel::OnPlayerLevelChange(int newLevel)
{
	
}

void UQuestChannel::OnSpecialEventTrigger(UQuestSpecialEventData* specialEvent)
{
}


void UQuestChannel::PlayQuestEndSound(URPGQuest* InQuest)
{
}

void UQuestChannel::PlayQuestStartSound(URPGQuest* InQuest)
{
}

void UQuestChannel::OnQuestProgress(URPGQuest* QuestUpdate)
{
}

void UQuestChannel::OnQuestRegression(URPGQuest* QuestUpdate)
{
}
