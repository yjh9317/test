// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Quest_StatsChannel.generated.h"

enum class EQuestNotifyEventType : uint8;

UENUM(BlueprintType)
enum class EQuestStatsEventType : uint8
{
	PlayerLevelUp       UMETA(DisplayName = "Player Level Up"),
};

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnStatsEventDelegate, UObject* entity, EQuestNotifyEventType eventType, float InStatValue);

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UQuest_StatsChannel : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Advanced Quest | Events")
	void OnStatsEventNotify(EQuestStatsEventType eventType, UObject* entity, float amount = 1);

	FOnStatsEventDelegate OnStatsEventDelegate;
};
