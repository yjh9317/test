// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SaveLoadData.h"
#include "UObject/ObjectMacros.h"
#include "TimerManager.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AsyncLoadGame.generated.h"

class UGameSaveSubsystem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAsyncLoadOutputPin);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAsyncLoadFailedPin);

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UAsyncLoadGame : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
	
public:

	/*Executes when at least one load operation(Level or Player) completes.*/
	UPROPERTY(BlueprintAssignable)
	FAsyncLoadOutputPin OnCompleted;

	/*Executes when all load operations have failed.*/
	UPROPERTY(BlueprintAssignable)
	FAsyncLoadFailedPin OnFailed;
	
	uint32 Data;
	ESaveGameMode Mode;

private:

	uint8 bIsActive : 1;
	uint8 bLoadFailed : 1;
	uint8 bFullReload : 1;
	uint8 bDeferredLoadSuccess : 1;

	TArray<FActorSaveData> SavedActors;
	FTimerHandle FailedTimerHandle;
	int32 LoadedActorNum;

	UPROPERTY()
	UObject* WorldContext;

	UPROPERTY()
	UGameSaveSubsystem* SLObject;

public:
	
	/**
	* Main function for Loading the Game. Use the Data checkboxes to define what you want to load.
	*
	* @param Data - Check here what data you want to load.
	* @param bFullReload - If false, load only Actors that have not been loaded. Set to true if you want to reload all saved Actor Data.
	*/
	UFUNCTION(BlueprintCallable, Category = "Easy Multi Save | Actors", meta = (DisplayName = "Load Game Actors", BlueprintInternalUseOnly = "true", WorldContext = "WorldContextObject"))
	static UAsyncLoadGame* AsyncLoadActors(UObject* WorldContextObject, UPARAM(meta = (Bitmask, BitmaskEnum = "/Script/RPGProject.ELoadTypeFlags")) int32 Data, bool bFullReload);

	UFUNCTION()
	static bool NativeSpawnLoadTask(UGameSaveSubsystem* Object, const int32& Data, const bool& bFullReload);
	
	UFUNCTION()
	static bool NativeLoadLevelActors(UGameSaveSubsystem* Object);

	virtual void Activate() override;

	FORCEINLINE bool IsActive() const { return bIsActive; }

	void FinishLoading();
	void StartDeferredLoad();

private:

	void PreLoading();
	void StartLoading();

	void LoadPlayer();
	void LoadLevel();

	void CompleteLoadingTask();
	void FailLoadingTask();

	void DeferredLoadActors();
	void ClearFailTimer();

	static ESaveGameMode GetMode(int32 Data);

	FORCEINLINE void SetLoadNotFailed()
	{
		ClearFailTimer();
		bLoadFailed = false;
	}
};
