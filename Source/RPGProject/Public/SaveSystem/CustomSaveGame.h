// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "CustomSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UCustomSaveGame : public USaveGame
{
	GENERATED_BODY()
public:

	/** The actual name of the custom save file. If empty it will use the class name. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Files", meta = (DisplayName = "File Name"))
	FString SaveGameName;
	
	/** If you want to use the current save slot for the custom save file. */
	UPROPERTY(EditDefaultsOnly, Category = "Files")
	bool bUseSaveSlot;
};
