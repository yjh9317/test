#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "QuestStruct.generated.h"

UENUM()
enum class EQuestType : uint8
{
	EQuest_Daily					UMETA(DisplayName = "Daily Quest"),
	EQuest_Weekly					UMETA(DisplayName = "Weekly Quest"),
	EQuest_MainQuest UMETA( DisplayName = "MainQuest"),
	EQuest_SubQuest UMETA( DisplayName = "SubQuest")
};

UENUM(BlueprintType)
enum class EQuestStatus : uint8
{
	NotStarted UMETA(DisplayName = "Not Started"),
	InProgress UMETA(DisplayName = "In Progress"),
	Completed UMETA(DisplayName = "Completed"),
	Failed UMETA(DisplayName = "Failed")
};

USTRUCT()
struct FQuestTime
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category = "QuestTime")
	bool bIsLimit;
	
	UPROPERTY(EditAnywhere, Category = "QuestTime")
	float LimitTime;
};

USTRUCT()
struct FQuestNPC
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category= "QuestNPC")
	FString QuestGiverName;

	UPROPERTY(EditAnywhere, Category= "QuestNPC")
	FString QuestReceiverName;
};


// USTRUCT()
// struct FQuestData : public FTableRowBase
// {
// 	GENERATED_USTRUCT_BODY()
//
// 	UPROPERTY(EditAnywhere, Category= "Quest Data")
// 	uint32 ID;
//
// 	UPROPERTY(EditAnywhere, Category= "Quest Data")
// 	FText QuestName;
//
// 	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Quest Data")
// 	FText QuestDescription;
//
// 	UPROPERTY(EditAnywhere, Category= "Quest Data")
// 	EQuestType QuestType;
//
// 	UPROPERTY(EditAnywhere, Category= "Quest Data")
// 	EQuestStatus QuestStatus;
//
// 	UPROPERTY(EditAnywhere, Category= "Quest Data")
// 	FQuestNPC QuestNPC;
//
// 	UPROPERTY(EditAnywhere, Category= "Quest Data")
// 	FQuestTime QuestTime;
//
// 	UPROPERTY(EditAnywhere, Category= "Quest Data")
// 	bool bRepeatable = false;
// };
