// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/RPGQuest.h"
#include "Quest/Data/RPGQuestData.h"

URPGQuest::URPGQuest()
{
	QuestData = CreateDefaultSubobject<URPGQuestData>(TEXT("QuestData"));
}

URPGQuest::~URPGQuest()
{
}

void URPGQuest::SetQuestData(URPGQuestData* _QuestData)
{
	QuestData = DuplicateObject<URPGQuestData>(_QuestData, this);

	/* Check if there is any requirements*/
	FQuestRequirementData& requirements = QuestData->QuestRequirements;
	
	if (requirements.PlayerLevel != 0)
		requirements.LevelMet = false;

	if (requirements.SpecialEvents.Num() > 0)
		requirements.AllEventsMet = false;

	if(!requirements.LevelMet
		|| !requirements.AllEventsMet
		|| requirements.QuestID.Num() > 0)
		bIsRequirementMet = false;
}

void URPGQuest::UpdateQuestState()
{
	if (PositiveObjectiveUpdateDelegate.IsBound())
		PositiveObjectiveUpdateDelegate.Broadcast(this);

	AllObjectivesCompleted = true;
	int objectivesCount = QuestData->Objectives.Num();
	for (int i = 0; i < objectivesCount; i++)
	{
		// if (!QuestData->objectives[i].isObjectiveComplete)
		// 	AllObjectivesCompleted = false;
	}

	/** Check if all the objectives are completed */
	if (AllObjectivesCompleted)
	{
		QuestState = EQuestState::Valid;
		if (QuestStateChangedDelegate.IsBound())
			QuestStateChangedDelegate.Broadcast(this, QuestState);
	}
}


void URPGQuest::EnableQuest()
{
}

void URPGQuest::DisableQuest()
{
}

void URPGQuest::ResetQuest()
{
}

void URPGQuest::ResetObjectives()
{
}

void URPGQuest::QuestFailed()
{
}

void URPGQuest::OnNotify(UObject* entity, EQuestNotifyEventType eventTypeP, float amount)
{
}

void URPGQuest::UpdateCurrentObjective(int i, float amount)
{
}

void URPGQuest::OnQuestRequirementChange(URPGQuest* questUpdateP, EQuestState questStateP)
{
}

void URPGQuest::OnEventRequirementChange(UQuestSpecialEventData* specialEvent)
{
}

void URPGQuest::OnLevelRequirementChange(int PlayerLevel)
{
}
