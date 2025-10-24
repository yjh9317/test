// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/UI/StatsViewModel.h"

UStatsViewModel::UStatsViewModel()
{
	bIsInitialized = false;
}

void UStatsViewModel::BeginDestroy()
{
	Shutdown();
	Super::BeginDestroy();
}

void UStatsViewModel::Initialize(UStatsComponent* InStatsComponent)
{
	if (!InStatsComponent)
	{
		return;
	}
    
	if (StatsComponent == InStatsComponent && bIsInitialized)
	{
		return;
	}
    
	if (StatsComponent && bIsInitialized)
	{
		Shutdown();
	}
    
	StatsComponent = InStatsComponent;
    
	if (StatsComponent)
	{
		BindToStatsComponent();
        
		// 초기값 계산
		TArray<FGameplayTag> InitialTags = {
			FGameplayTag::RequestGameplayTag("Character.Stat.Health"),
			FGameplayTag::RequestGameplayTag("Character.Stat.Stamina"),
			FGameplayTag::RequestGameplayTag("Character.Stat.Experience"),
			FGameplayTag::RequestGameplayTag("Character.Stat.Level")
		};
        
		for (const FGameplayTag& Tag : InitialTags)
		{
			UpdateUIValues(Tag);
		}
        
		bIsInitialized = true;
	}
}

void UStatsViewModel::Shutdown()
{
}

void UStatsViewModel::BindToStatsComponent()
{
}

void UStatsViewModel::UnbindFromStatsComponent()
{
}

void UStatsViewModel::OnStatChangedInComponent(FGameplayTag StatTag, float NewValue)
{
}

void UStatsViewModel::UpdateUIValues(FGameplayTag StatTag)
{
	UpdatePercent(StatTag);
	UpdateText(StatTag);
	UpdateColor(StatTag);
}

void UStatsViewModel::UpdatePercent(FGameplayTag StatTag)
{
}

void UStatsViewModel::UpdateText(FGameplayTag StatTag)
{
}

void UStatsViewModel::UpdateColor(FGameplayTag StatTag)
{
}

// float UStatsViewModel::CalculatePercent(FGameplayTag CurrentTag, FGameplayTag MaxTag) const
// {
// }

