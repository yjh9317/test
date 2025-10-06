#pragma once
#include "QuestRequiermentEventType.generated.h"

class UQuestSpecialEventData;

USTRUCT(Blueprintable, BlueprintType)
struct FQuestRequirementData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest | Requirement")
	int PlayerLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest | Requirement")
	bool LevelMet = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest | Requirement")
	TMap<int, bool> QuestID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest | Requirement")
	TMap<UQuestSpecialEventData*, bool> SpecialEvents;
	
	bool AllEventsMet = true;
};
