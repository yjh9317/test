// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/Components/LevelingComponent.h"

#include "Net/UnrealNetwork.h"

ULevelingComponent::ULevelingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

}

void ULevelingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{ 
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ULevelingComponent, CurrentLevel);
	DOREPLIFETIME(ULevelingComponent, CurrentEXP);
	DOREPLIFETIME(ULevelingComponent, LevelMaxEXP);
}

void ULevelingComponent::BeginDestroy()
{
	Super::BeginDestroy();

	// OnGainedExperience.Clear();
	// OnLevelUp.Clear();
	// OnLevelChanged.Clear();
	// OnCurrentExpChanged.Clear();
}

void ULevelingComponent::OnRep_CurrentLevel(int32 OldLevel)
{
}

void ULevelingComponent::OnRep_MaxLevel(int32 OldMaxLevel)
{
}

void ULevelingComponent::OnRep_CurrentEXP(float OldEXP)
{
}

void ULevelingComponent::OnRep_LevelMaxEXP(float OldEXP)
{
}




