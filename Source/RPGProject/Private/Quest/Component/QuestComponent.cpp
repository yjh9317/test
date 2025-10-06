// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/Component/QuestComponent.h"

UQuestComponent::UQuestComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Owner = GetOwner();
}

UQuestComponent::~UQuestComponent()
{
}


void UQuestComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UQuestComponent::LateBeginPlay()
{
	
}

void UQuestComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

// void UQuestComponent::OnQuestStateChangedWrapper(URPGQuest* questUpdate, EAQ_QuestState QuestState)
// {
// }
//
// void UQuestComponent::OnQuestRequirementMet(URPGQuest* quest)
// {
// }
//
// void UQuestComponent::BindFunctionsToQuestDelegates()
// {
// }


void UQuestComponent::OnQuestStateChangedWrapper(URPGQuest* questUpdate, EQuestState QuestState)
{
}

void UQuestComponent::OnQuestRequirementMet(URPGQuest* quest)
{
}

void UQuestComponent::BindFunctionsToQuestDelegates()
{
}

void UQuestComponent::SetQuestMarker(bool isMarkerVisible, bool isQuestValid, EQuestType QuestState)
{
}

void UQuestComponent::UpdateQuestMarker()
{
}

void UQuestComponent::CheckQuestTypes(TArray<URPGQuest*>& ValidQuests, EQuestType& currentType)
{
}

