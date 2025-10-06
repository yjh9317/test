// Fill out your copyright notice in the Description page of Project Settings.

#include "Quest/Channels/Quest_EnvironmentChannel.h"
#include "Quest/Data/Enum/QuestNotifyEventType.h"

const int DEFAULT_AMOUNT = 1;

void UQuest_EnvironmentChannel::OnEnvironmentEventNotify(EQuestEnvironmentEventType eventType, UObject* entity)
{
	switch (eventType)
	{
	case EQuestEnvironmentEventType::Interact:
		OnInteractEventDelegate.Broadcast(entity, EQuestNotifyEventType::Interact, DEFAULT_AMOUNT);
		break;

	case EQuestEnvironmentEventType::Travel:
		OnTravelEventDelegate.Broadcast(entity, EQuestNotifyEventType::Travel, DEFAULT_AMOUNT);
		break;

	case EQuestEnvironmentEventType::MiniGame:
		OnMiniGameEventDelegate.Broadcast(entity, EQuestNotifyEventType::MiniGame, DEFAULT_AMOUNT);
		break;
	}
}
