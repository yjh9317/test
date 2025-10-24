// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Enum/QuestObjectivesType.h"
#include "Enum/QuestRequiermentEventType.h"
#include "QuestStruct.h"
#include "RPGQuestData.generated.h"

class UItemBase;

USTRUCT(Blueprintable, BlueprintType)
struct FQuestObjectives
{
	GENERATED_BODY()

	/** Objectives specificities */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest | Quest | Data")
	EQuestObjectivesType ObjectiveType = EQuestObjectivesType::Interact;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest | Quest | Data", meta = (EditCondition = "objectiveType != EAQ_ObjectivesType::Collect && objectiveType != EAQ_ObjectivesType::Deliver", EditConditionHides))
	UClass* ObjectTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest | Quest | Data", meta = (EditCondition = "objectiveType == EAQ_ObjectivesType::Collect || objectiveType == EAQ_ObjectivesType::Deliver", EditConditionHides))
	URPGQuestData* ItemTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Quest | Quest | Data")
	FString ObjectiveSummary = FString();


	/** If the goal is a unique target*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest | Quest | Data", meta = (EditCondition = "objectiveType != EAQ_ObjectivesType::Collect && objectiveType != EAQ_ObjectivesType::Deliver", EditConditionHides))
	bool IsUnique = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest | Quest | Data", meta = (EditCondition = "objectiveType != EAQ_ObjectivesType::Collect && objectiveType != EAQ_ObjectivesType::Deliver", EditConditionHides))
	int UniqueObjectID = 0;


	/** Amount needed to complete the objective */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest | Quest | Data")
	int AmountNeeded = 1;

	UPROPERTY(BlueprintReadWrite, Category = "Quest | Quest | Data")
	int CurrentAmount = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Quest | Quest | Data")
	bool IsObjectiveComplete = false;
};


UCLASS()
class RPGPROJECT_API URPGQuestData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/* Quest properties */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest | Quest Overview")
	EQuestType QuestType = EQuestType::EQuest_MainQuest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest | Quest Overview")
	FString QuestLocation = FString();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest | Quest Overview")
	FString QuestTitle = FString();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest | Quest Overview")
	FString QuestDescription = FString();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest | Quest Overview")
	FString QuestSummary = FString();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest | Quest Overview")
	int QuestID = 0;

	/* Requirements */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest | Quest Overview")
	FQuestRequirementData QuestRequirements;


	/* Objectives */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest | Quest Overview")
	TArray<FQuestObjectives> Objectives;


	/** Rewards */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest | Quest Overview")
	int XpReward = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest | Quest Overview")
	int GoldReward = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest | Quest Overview")
	TMap<UItemBase*, int> ItemsReward;
};
