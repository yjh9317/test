#pragma once
#include "QuestNotifyEventType.generated.h"


UENUM(BlueprintType)
enum class EQuestNotifyEventType : uint8
{
	// Combat 
	Kill                    UMETA(DisplayName = "Kill"),
	Heal                    UMETA(DisplayName = "Heal"),
	Damage                  UMETA(DisplayName = "Damage"),
	Protect                 UMETA(DisplayName = "Protect"),

	// Inventory
	Collect                 UMETA(DisplayName = "Collect"),
	RemovedFromInventory    UMETA(DisplayName = "Remove From Inventory"),
	Deliver                 UMETA(DisplayName = "Deliver to"),

	// Environment
	Interact                UMETA(DisplayName = "Interact"),
	Travel                  UMETA(DisplayName = "Travel"),
	MiniGame                UMETA(DisplayName = "Mini-Game"),


	// Stats
	PlayerLevelUp           UMETA(DisplayName = "Player Level Up"),
};
