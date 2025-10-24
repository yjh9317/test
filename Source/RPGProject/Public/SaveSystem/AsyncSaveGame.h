// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadData.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncSaveGame.generated.h"

class UGameSaveSubsystem;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAsyncSaveOutputPin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAsyncSaveFailedPin);

UENUM()
enum class ENextStepType : uint8
{
	SaveLevel,
	FinishSave,
};
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UAsyncSaveGame : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:

	/*Executes when at least one save operation(Level or Player) completes.*/
	UPROPERTY(BlueprintAssignable)
	FAsyncSaveOutputPin OnCompleted;

	/*Executes when all save operations have failed.*/
	UPROPERTY(BlueprintAssignable)
	FAsyncSaveFailedPin OnFailed;

	uint32 Data;
	ESaveGameMode Mode;

private:

	uint8 bIsActive : 1;
	uint8 bFinishedStep : 1;
	uint8 bHasFailed : 1;
	
	UPROPERTY()
	UObject* WorldContext;

	UPROPERTY()
	UGameSaveSubsystem* SaveLoadObject;

public:

	/**
	* Main function for Saving the Game. Use the Data checkboxes to define what you want to save. 
	*
	* @param Data - Check here what data you want to save. 
	*/
	UFUNCTION(BlueprintCallable, Category = "Easy Multi Save | Actors", meta = (DisplayName = "Save Game Actors", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static UAsyncSaveGame* AsyncSaveActors(UObject* WorldContextObject, UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/RPGProject.ESaveTypeFlags")) int32 Data);

	UFUNCTION()
	static bool NativeSpawnSaveTask(UGameSaveSubsystem* Object, const int32& Data);

	UFUNCTION()
	static bool NativeSaveLevelActors(UGameSaveSubsystem* Object);

	virtual void Activate() override;

	FORCEINLINE bool IsActive() const { return bIsActive; }

private:

	void StartSaving();

	void SavePlayer();
	void InternalSavePlayer();

	void SaveLevel();
	void InternalSaveLevel();

	void FinishSaving();
	void CompleteSavingTask();

	void TryMoveToNextStep(ENextStepType Step);

	static ESaveGameMode GetMode(int32 Data);
};
