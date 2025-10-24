// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest/RPGQuest.h"
#include "UObject/NoExportTypes.h"
#include "Quest_PlayerChannelsFacade.generated.h"

enum class EQuestEnvironmentEventType : uint8;
enum class EQuestCombatEventType : uint8;
enum class EQuestStatsEventType : uint8;
/**
 * 
 */
UINTERFACE(Blueprintable)
class RPGPROJECT_API UQuest_PlayerChannelsFacade : public UInterface
{
	GENERATED_BODY()
	
};


class RPGPROJECT_API IQuest_PlayerChannelsFacade
{
	GENERATED_BODY()
public:
	UFUNCTION()
	virtual void OnQuestStateChanged(URPGQuest* QuestUpdate, EQuestState QuestState) {}

	virtual void OnInteractQuestGiver(TArray<URPGQuest*> questsToDisplay) {}


	/* Events */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Quest | Events")
	void OnQuestEnable(URPGQuest* quest);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Quest | Events")
	void OnEnvironmentEventNotify(EQuestEnvironmentEventType eventType, UObject* entity);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Quest | Events")
	void OnCombatEventNotify(EQuestCombatEventType eventType, UObject* entity, float amount = 1);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Quest | Events")
	void OnStatsEventNotify(EQuestStatsEventType eventType, UObject* entity, float InStatValue = 1);


	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Advanced Quest | Events")
	// void OnSpecialEventNotify(UAQ_SpecialEventData* specialEvent);


	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Quest | Inventory")
	void AddItemToInvetory(URPGQuestData* item, int amount);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Quest | Inventory")
	void RemoveItemFromInvetory(URPGQuestData* item, int amount, bool bRemoveCompletely = false);
};