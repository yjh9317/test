// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Quest_PlayerChannelsFacade.h"
#include "UObject/NoExportTypes.h"
#include "Quest/Data/QuestStruct.h"
#include "Quest/Data/Enum/QuestObjectivesType.h"
#include "Quest_PlayerChannels.generated.h"

class UQuest_InventoryChannel;
class UQuest_EnvironmentChannel;
class UQuest_CombatChannel;
class UQuestChannel;
class UQuest_StatsChannel;
class UQuest_AudioChannel;
class UQuestManager;
class URPGQuest;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UQuest_PlayerChannels : public UActorComponent, public IQuest_PlayerChannelsFacade
{
	GENERATED_BODY()

public:
	UQuest_PlayerChannels();
	~UQuest_PlayerChannels();
		/* Book Quest Template */
	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest | UI")
	// TSubclassOf<UUserWidget> QuestWidgetsClass = nullptr;
	// void InitQuestWidgets();
	//
	// UFUNCTION(BlueprintCallable, Category = "Quest | Events")
	// void ForceInitQuestWidget();
	//
	/* Observers Pattern */
	/** Add Observer to the corresponding channel */
	UFUNCTION(BlueprintCallable, Category = "Quest | Events")
	void AddObserver(URPGQuest* entity, EQuestObjectivesType eventType);
	
	/** Remove Observer to the corresponding channel */
	UFUNCTION(BlueprintCallable, Category = "Quest | Events")
	void RemoveObserver(URPGQuest* entity, EQuestObjectivesType eventType);
	
	
	/* Delegates */
	virtual void OnQuestStateChanged(URPGQuest* QuestUpdate, EQuestState QuestState) override;
	virtual void OnInteractQuestGiver(TArray<URPGQuest*> questsToDisplay) override;
	

	/* This function is only for debug purpose, as there isn't any Stats Component to store
	Player's level and others data. */
	// UFUNCTION(BlueprintCallable, Category = "Advanced Quest | Events")
	// void LevelUp();


	void OnQuestCreated(URPGQuest* quest);
	
	UFUNCTION(Category = "Advanced Quest | Events")
	void OnQuestEnable_Implementation(URPGQuest* quest);
	
	UFUNCTION(Category = "Advanced Quest | Events")
	void OnEnvironmentEventNotify_Implementation(EQuestEnvironmentEventType eventType, UObject* entity);
	UFUNCTION(Category = "Advanced Quest | Events")
	void OnCombatEventNotify_Implementation(EQuestCombatEventType eventType, UObject* entity, float amount = 1);
	UFUNCTION(Category = "Advanced Quest | Events")
	void OnStatsEventNotify_Implementation(EQuestStatsEventType eventType, UObject* entity, float InStatValue = 1);
	
	UFUNCTION(Category = "Advanced Quest | Events")
	void OnSpecialEventNotify_Implementation(UQuestSpecialEventData* specialEvent);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Advanced Quest | Events")
	void OnQuestEnded(URPGQuest* quest);
	
	
	// /* Controllers */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Advanced Quest | Inputs", meta = (AllowPrivateAccess = "true"))
	class UInputAction* OpenJournalAction = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Advanced Quest | Inputs", meta = (AllowPrivateAccess = "true"))
	class UInputAction* SaveQuestsAction = nullptr;
	
	void SetPlayerInputComponent();

	/* Checkers for Quests */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Advanced Quest | Quest")
	int CheckInventoryForItem(FQuestObjectives currentObjective);	// This is implemented in blueprint 
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Advanced Quest | Quest")
	int CheckForPlayerStats(FQuestObjectives currentObjective);	// This is implemented in blueprint 

	/* Sounds */
	// UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Advanced Quest | Sounds | Quest")
	// UAQ_QuestSounds* QuestSounds = nullptr;

protected:
	virtual void BeginPlay() override;

	/** Player Channels */
	UPROPERTY(BlueprintReadOnly, Category = "Quest | Channels")
	UQuest_InventoryChannel* InventoryChannel = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | Channels")
	UQuest_EnvironmentChannel* EnvironmentChannel = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | Channels")
	UQuest_CombatChannel* CombatChannel = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | Channels")
	UQuestChannel* QuestChannel = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | Channels")
	UQuest_StatsChannel* StatsChannel = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | Channels")
	UQuest_AudioChannel* AudioChannel = nullptr;

	/* Player Stats */
	// UPROPERTY(BlueprintReadOnly, Category = "Quest | Player")
	// int PlayerLevel = 1;
	//
	// UFUNCTION(BlueprintCallable, Category = "Quest | Player")
	// void SetPlayerLevel(int inPlayerLevel) { PlayerLevel = inPlayerLevel;}

	/* Saving system */
	void SaveGame();

	// Those functions should be implemented in the Inventory Component
	// UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Advanced Quest | Save-Load")
	// void SavePlayerData(); 
	// UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Advanced Quest | Save-Load")
	// void LoadPlayerData();

private:
	UQuestManager* QuestManager;
};
