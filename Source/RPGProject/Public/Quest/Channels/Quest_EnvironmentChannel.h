// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Quest_EnvironmentChannel.generated.h"

enum class EQuestNotifyEventType : uint8;

UENUM(BlueprintType)
enum class EQuestEnvironmentEventType : uint8
{
	Interact                UMETA(DisplayName = "Interact"),
	Travel                  UMETA(DisplayName = "Travel"),
	MiniGame                UMETA(DisplayName = "Mini-Game")
};

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnInteractEventDelegate, UObject* entity, EQuestNotifyEventType eventType, float amount);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnTravelEventDelegate, UObject* entity, EQuestNotifyEventType eventType, float amount);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnMiniGameEventDelegate, UObject* entity, EQuestNotifyEventType eventType, float amount);


/**
 * 
 */
UCLASS()
class RPGPROJECT_API UQuest_EnvironmentChannel : public UObject
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Advanced Quest | Events")
	void OnEnvironmentEventNotify(EQuestEnvironmentEventType eventType, UObject* entity);

	/* Delegates */
	FOnInteractEventDelegate OnInteractEventDelegate;
	FOnTravelEventDelegate OnTravelEventDelegate;
	FOnMiniGameEventDelegate OnMiniGameEventDelegate;
};
