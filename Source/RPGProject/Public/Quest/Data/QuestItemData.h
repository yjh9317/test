// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Enum/QuestObjectivesType.h"
#include "Enum/QuestRequiermentEventType.h"
#include "QuestItemData.generated.h"

UENUM(BlueprintType)
enum class ERPGQuestType : uint8
{
	MainQuest				UMETA(DisplayName = "Main Quest"),
	SideQuest				UMETA(DisplayName = "Side Quest"),
};

class UQuestItemData;

USTRUCT(Blueprintable, BlueprintType)
struct FObjectives
{
	GENERATED_BODY()

	/** Objectives specificities */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest | Data")
	EQuestObjectivesType ObjectiveType = EQuestObjectivesType::Interact;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest | Data", meta = (EditCondition = "objectiveType != EAQ_ObjectivesType::Collect && objectiveType != EAQ_ObjectivesType::Deliver", EditConditionHides))
	UClass* ObjectTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest | Data", meta = (EditCondition = "objectiveType == EAQ_ObjectivesType::Collect || objectiveType == EAQ_ObjectivesType::Deliver", EditConditionHides))
	UQuestItemData* ItemTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Advanced Quest | Quest | Data")
	FString ObjectiveSummary = FString();


	/** If the goal is a unique target*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest | Data", meta = (EditCondition = "objectiveType != EAQ_ObjectivesType::Collect && objectiveType != EAQ_ObjectivesType::Deliver", EditConditionHides))
	bool IsUnique = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest | Data", meta = (EditCondition = "objectiveType != EAQ_ObjectivesType::Collect && objectiveType != EAQ_ObjectivesType::Deliver", EditConditionHides))
	int UniqueObjectID = 0;


	/** Amount needed to complete the objective */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest | Data")
	int AmountNeeded = 1;

	UPROPERTY(BlueprintReadWrite, Category = "Advanced Quest | Quest | Data")
	int CurrentAmount = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Advanced Quest | Quest | Data")
	bool IsObjectiveComplete = false;
};


UCLASS()
class RPGPROJECT_API UQuestItemData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	/* Quest properties */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest Overview")
	ERPGQuestType QuestType = ERPGQuestType::MainQuest;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest Overview")
	FString QuestLocation = FString();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest Overview")
	FString questTitle = FString();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest Overview")
	FString questDescription = FString();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest Overview")
	FString questSummary = FString();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest Overview")
	int QuestID = 0;

	/* Requirements */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest Overview")
	FQuestRequirementData questRequirements;


	/* Objectives */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest Overview")
	TArray<EQuestObjectivesType> objectives;


	/** Rewards */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest Overview")
	int xpReward = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest Overview")
	int goldReward = 0;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest Overview")
	// TMap<class UAQ_ItemData*, int> itemsReward;
};
