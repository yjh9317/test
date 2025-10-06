// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/Channels/Quest_PlayerChannels.h"
#include "Quest/RPGQuest.h"
#include "Quest/Channels/QuestChannel.h"
#include "Quest/Channels/Quest_AudioChannel.h"
#include "Quest/Channels/Quest_CombatChannel.h"
#include "Quest/Channels/Quest_EnvironmentChannel.h"
#include "Quest/Channels/Quest_InventoryChannel.h"
#include "Quest/Channels/Quest_StatsChannel.h"


UQuest_PlayerChannels::UQuest_PlayerChannels()
{
	InventoryChannel = CreateDefaultSubobject<UQuest_InventoryChannel>(TEXT("Inventory Channel"));
	EnvironmentChannel = CreateDefaultSubobject<UQuest_EnvironmentChannel>(TEXT("Environment Channel"));
	CombatChannel = CreateDefaultSubobject<UQuest_CombatChannel>(TEXT("Combat Channel"));
	StatsChannel = CreateDefaultSubobject<UQuest_StatsChannel>(TEXT("Stats Channel"));
	AudioChannel = CreateDefaultSubobject<UQuest_AudioChannel>(TEXT("Audio Channel"));
	QuestChannel = CreateDefaultSubobject<UQuestChannel>(TEXT("Quest Channel"));

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

UQuest_PlayerChannels::~UQuest_PlayerChannels()
{
}

void UQuest_PlayerChannels::BeginPlay()
{
	Super::BeginPlay();
}

void UQuest_PlayerChannels::AddObserver(URPGQuest* entity, EQuestObjectivesType eventType)
{
}

void UQuest_PlayerChannels::RemoveObserver(URPGQuest* entity, EQuestObjectivesType eventType)
{
}

void UQuest_PlayerChannels::OnQuestStateChanged(URPGQuest* QuestUpdate, EQuestState QuestState)
{
	IQuest_PlayerChannelsFacade::OnQuestStateChanged(QuestUpdate, QuestState);
}

void UQuest_PlayerChannels::OnInteractQuestGiver(TArray<URPGQuest*> questsToDisplay)
{
	IQuest_PlayerChannelsFacade::OnInteractQuestGiver(questsToDisplay);
}

void UQuest_PlayerChannels::OnQuestCreated(URPGQuest* quest)
{
}

void UQuest_PlayerChannels::OnQuestEnable_Implementation(URPGQuest* quest)
{
	IQuest_PlayerChannelsFacade::OnQuestEnable_Implementation(quest);
}

void UQuest_PlayerChannels::OnEnvironmentEventNotify_Implementation(EQuestEnvironmentEventType eventType,
	UObject* entity)
{
	IQuest_PlayerChannelsFacade::OnEnvironmentEventNotify_Implementation(eventType, entity);
}

void UQuest_PlayerChannels::OnCombatEventNotify_Implementation(EQuestCombatEventType eventType, UObject* entity,
	float amount)
{
	IQuest_PlayerChannelsFacade::OnCombatEventNotify_Implementation(eventType, entity, amount);
}

void UQuest_PlayerChannels::OnStatsEventNotify_Implementation(EQuestStatsEventType eventType, UObject* entity,
	float InStatValue)
{
	IQuest_PlayerChannelsFacade::OnStatsEventNotify_Implementation(eventType, entity, InStatValue);
}

void UQuest_PlayerChannels::OnSpecialEventNotify_Implementation(UQuestSpecialEventData* specialEvent)
{
}

