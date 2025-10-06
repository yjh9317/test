// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Quest_CombatChannel.generated.h"

enum class EQuestNotifyEventType : uint8;

UENUM(BlueprintType)
enum class EQuestCombatEventType : uint8
{
	Kill       UMETA(DisplayName = "Kill"),
	Heal       UMETA(DisplayName = "Heal"),
	Damage     UMETA(DisplayName = "Damage"),
	Protect    UMETA(DisplayName = "Protect"),
};

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnKillEventDelegate, UObject* entity, EQuestNotifyEventType eventType, float amount);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnProtectEventDelegate, UObject* entity, EQuestNotifyEventType eventType, float amount);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnHealEventDelegate, UObject* entity, EQuestNotifyEventType eventType, float amount);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnDamageEventDelegate, UObject* entity, EQuestNotifyEventType eventType, float amount);

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UQuest_CombatChannel : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Advanced Quest | Events")
	void OnCombatEventNotify(EQuestCombatEventType eventType, UObject* entity, float amount = 1);

	FOnKillEventDelegate OnKillEventDelegate;
	FOnProtectEventDelegate OnProtectEventDelegate;
	FOnHealEventDelegate OnHealEventDelegate;
	FOnDamageEventDelegate OnDamageEventDelegate;
};
