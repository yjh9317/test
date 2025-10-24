// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SaveLoadBlueprintLibrary.generated.h"

class UInfoSaveGame;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API USaveLoadBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// Define a global save game name. If empty, it will use the default name from the Plugin Settings.
	UFUNCTION(BlueprintCallable, Category = "SaveLoad", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Current Save Slot"))
	static void SetCurrentSaveGameName(UObject* WorldContextObject, const FString& SaveGameName);

	// Get the current save game slot defined by 'Set Current Save Slot'.
	UFUNCTION(BlueprintPure, Category = "Easy Multi Save | Slots", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Current Save Slot"))
	static UInfoSaveGame* GetSlotInfoSaveGame(UObject* WorldContextObject, FString& SaveGameName);

	// Get a save game slot by name.
	UFUNCTION(BlueprintPure, Category = "Easy Multi Save | Slots", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Named Save Slot"))
	static UInfoSaveGame* GetNamedSlotInfo(UObject* WorldContextObject, const FString& SaveGameName);

	// Loads the array of available save games / slots, sorted by their time of saving.
	UFUNCTION(BlueprintPure, Category = "Easy Multi Save | Slots", meta = (WorldContext = "WorldContextObject", DisplayName = "Get All Save Slots"))
	static TArray<FString> GetSortedSaveSlots(UObject* WorldContextObject);

	/*
	* Useful if you have a multi-user game. 
	* Puts all save game data into /UserSaveGames/UserName instead of /SaveGames/
	* If the name is none, it just uses the /SaveGames/ folder.
	*/
	UFUNCTION(BlueprintCallable, Category = "Easy Multi Save | Multi User", meta = (WorldContext = "WorldContextObject", DisplayName = "Set Current Save User"))
	static void SetCurrentSaveUserName(UObject* WorldContextObject, const FString& UserName);

	// Fully removes all save files of a specific user.
	UFUNCTION(BlueprintCallable, Category = "Easy Multi Save | Multi User", meta = (WorldContext = "WorldContextObject", DisplayName = "Delete Save User"))
	static void DeleteSaveUser(UObject* WorldContextObject, const FString& UserName);

	// Returns an array of all available save users that were added using 'Set Current Save User'. Sorted by name.
	UFUNCTION(BlueprintPure, Category = "Easy Multi Save | Multi User", meta = (WorldContext = "WorldContextObject", DisplayName = "Get Save Users"))
	static TArray<FString> GetAllSaveUsers(UObject* WorldContextObject);
	
	// Check if a specific save slot exists. 
	UFUNCTION(BlueprintPure, Category = "Easy Multi Save | Slots", meta = (WorldContext = "WorldContextObject", DisplayName = "Does Save Slot Exist"))
	static bool DoesSaveSlotExist(UObject* WorldContextObject, const FString& SaveGameName);
	
	// Deletes all data and directories for a save game.
	UFUNCTION(BlueprintCallable, Category = "Easy Multi Save | Slots", meta = (WorldContext = "WorldContextObject", DisplayName = "Delete Save Slot"))
	static void DeleteAllSaveDataForSlot(UObject* WorldContextObject, const FString& SaveGameName);
	
	/**
	* Deletes the cache for Multi-Level Saving. 
	* All save data from currently unloaded levels is removed from memory. 
	*/
	UFUNCTION(BlueprintCallable, Category = "Easy Multi Save | Multi Level", meta = (WorldContext = "WorldContextObject", DisplayName = "Clear Multi Level Save"))
	static void ClearMultiLevelSave(UObject* WorldContextObject);
	
	// Imports a thumbnail as .png from the save game folder.
	UFUNCTION(BlueprintCallable, Category = "Easy Multi Save | Thumbnail", meta = (WorldContext = "WorldContextObject"))
	static UTexture2D* ImportSaveThumbnail(UObject* WorldContextObject, const FString& SaveGameName);
	
	/**
	* Saves a thumbnail from a render target texture as .png to the save game folder.
	*
	* @param TextureRenderTarget - The texture target from the scene capture 2d source.
	* @param SaveGameName - The name of the Savegame/Slot that is tied to the thumbnail.
	*/
	UFUNCTION(BlueprintCallable, Category = "Easy Multi Save | Thumbnail", meta = (WorldContext = "WorldContextObject"))
	static void ExportSaveThumbnail(UObject* WorldContextObject, UTextureRenderTarget2D* TextureRenderTarget, const FString& SaveGameName);
	
	/**
	* Adds and Removes the common EMS property tags. A good place to call it is Begin Play. Should not be called from EMS Interface Events. 
	*
	* @param bSkipSave -  The Actor is not saved at all.
	* @param bPersistent - Loads this Actor independent of the level. Not valid for Player Actors. Does not save Transform.
	* @param bSkipTransform - Skips saving/loading of the Actors transform. Spawned Actors appear at (0,0,0).
	*/
	UFUNCTION(BlueprintCallable, Category = "Easy Multi Save | Actors", meta = (WorldContext = "WorldContextObject"))
	static void SetActorSaveProperties(UObject* WorldContextObject, bool bSkipSave, bool bPersistent, bool bSkipTransform);
	
	/**
	* Checks if SaveGameActors or LoadGameActors is currently active. 
	* @return - True if SaveGameActors or LoadGameActors is currently active.
	*/
	UFUNCTION(BlueprintPure, Category = "Easy Multi Save | Actors", meta = (WorldContext = "WorldContextObject"))
	static bool IsSavingOrLoading(UObject* WorldContextObject);
	
	// Saves all available custom save objects automatically. 
	UFUNCTION(BlueprintCallable, Category = "Easy Multi Save | Custom", meta = (WorldContext = "WorldContextObject", DisplayName = "Save Custom Objects", AdvancedDisplay="SaveGame"))
	static bool SaveCustom(UObject* WorldContextObject);
	
	/**
	* Gets the desired EmsCustomSaveGame object.
	*
	* @param SaveGameClass - The class of the save object.
	* @param SaveGameName - The name of the custom save.
	* @return - The custom save object.
	*/
	UFUNCTION(BlueprintPure, Category = "Easy Multi Save | Custom", meta = (WorldContext = "WorldContextObject", DeterminesOutputType = "SaveGameClass", DisplayName = "Get Custom Save Object"))
	static UCustomSaveGame* GetCustomSave(UObject* WorldContextObject, TSubclassOf<UCustomSaveGame> SaveGameClass);
	
	// Deletes the desired EmsCustomSaveGame object.
	UFUNCTION(BlueprintCallable, Category = "Easy Multi Save | Custom", meta = (WorldContext = "WorldContextObject", DisplayName = "Delete Custom Save Object"))
	static void DeleteCustomSave(UObject* WorldContextObject, UCustomSaveGame* SaveGame);
	
	/**
	* If the current Persistent Level is using World Partition with GIS_SaveLoad
	* @return - If the World uses World Partition.
	*/
	UFUNCTION(BlueprintPure, Category = "Easy Multi Save | World Partition", meta = (WorldContext = "WorldContextObject", DisplayName = "Is World Partition Save"))
	static bool IsWorldPartition(UObject* WorldContextObject);
};
