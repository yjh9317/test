// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/Channels/Quest_InventoryChannel.h"
#include "Quest/Data/Enum/QuestNotifyEventType.h"

void UQuest_InventoryChannel::OnInventoryEventNotify(EQuestInventoryEventType eventType, UObject* entity, float amount)
{
	switch (eventType)
	{
	case EQuestInventoryEventType::Collect:
		OnInventoryEventDelegate.Broadcast(entity, EQuestNotifyEventType::Collect, amount);
		break;
	case EQuestInventoryEventType::RemoveFromInventory:
		OnInventoryEventDelegate.Broadcast(entity, EQuestNotifyEventType::RemovedFromInventory, -amount);
		break;
	}
}
