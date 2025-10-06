// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Public/SaveSystem/SaveLoadData.h"
#include "InfoSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UInfoSaveGame : public USaveGame
{
	GENERATED_BODY()
public:

	UPROPERTY(SaveGame, BlueprintReadOnly, Category = "Slots")
	FSaveSlotInfo SlotInfo;
};
