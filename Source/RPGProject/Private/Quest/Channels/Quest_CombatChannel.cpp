// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/Channels/Quest_CombatChannel.h"

#include "Quest/Data/Enum/QuestNotifyEventType.h"

void UQuest_CombatChannel::OnCombatEventNotify(EQuestCombatEventType eventType, UObject* entity, float amount)
{
	switch (eventType)
	{
	case EQuestCombatEventType::Kill:
		OnKillEventDelegate.Broadcast(entity, EQuestNotifyEventType::Kill, amount);
		break;

	case EQuestCombatEventType::Protect:
		OnProtectEventDelegate.Broadcast(entity, EQuestNotifyEventType::Protect, amount);
		break;

	case EQuestCombatEventType::Heal:
		OnHealEventDelegate.Broadcast(entity, EQuestNotifyEventType::Heal, amount);
		break;

	case EQuestCombatEventType::Damage:
		OnDamageEventDelegate.Broadcast(entity, EQuestNotifyEventType::Damage, amount);
		break;
	}
}
