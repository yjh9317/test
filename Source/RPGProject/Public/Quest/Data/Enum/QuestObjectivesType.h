#pragma once
#include "QuestObjectivesType.generated.h"


UENUM(BlueprintType)
enum class EQuestObjectivesType : uint8
{
	Interact		UMETA(DisplayName = "Interact"),
	Collect			UMETA(DisplayName = "Collect"),
	Deliver         UMETA(DisplayName = "Deliver"),

	Kill			UMETA(DisplayName = "Kill"),
	Protect			UMETA(DisplayName = "Protect"),

	Location		UMETA(DisplayName = "Location"),
	MiniGame        UMETA(DisplayName = "Mini-Game"),

	PlayerLevelUp   UMETA(DisplayName = "Player Level Up"),
};

UENUM(BlueprintType)
enum class EQuestObjectivesSpecifier : uint8
{
	None			UMETA(DisplayName = "Default"),
	Unique			UMETA(DisplayName = "Unique")
};
