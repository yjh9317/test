// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "CoreMinimal.h"
#include "Misc/Paths.h"
#include "Engine/World.h"
#include "Engine/Level.h"
#include "Engine/LevelStreaming.h"
#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"
#include "Serialization/MemoryWriter.h"
#include "Engine/EngineTypes.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SaveLoadData.h"
#include "CustomSaveGame.h"
#include "InfoSaveGame.h"
#include "GameFramework/SaveGame.h"
#include "AsyncSaveGame.h"
#include "AsyncLoadGame.h"
#include "SaveLoadSettings.h"
#include "GameSaveSubsystem.generated.h"

/**
 * 
 */
DEFINE_LOG_CATEGORY_STATIC(LogGameSave, Log, All);

UCLASS(config=Engine, configdonotcheckdefaults, meta=(DisplayName = "MySaveLoad"))
class RPGPROJECT_API UGameSaveSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	static UGameSaveSubsystem* Get(UObject* WorldContextObject);
	
	UPROPERTY(config)
	FString CurrentSaveGameName;

	UPROPERTY(config)
	FString CurrentSaveUserName;

	UPROPERTY(Transient)
	TArray<FActorSaveData> SavedActors;

private:

	const uint32 PlayerIndex = 0;

	FCriticalSection ActorCriticalSection;

	uint32 PartitionLoadNum;
	uint32 PartitionSaveNum;
	float LastPartitionLoadTime;
	float LastPartitionSaveTime;

private:

	UPROPERTY(Transient)
	TArray<AActor*> ActorList;

	UPROPERTY(Transient)
	TArray<FLevelArchive> LevelArchiveList;	

	UPROPERTY(Transient)
	FMultiLevelStreamingData MultiLevelStreamData;

	UPROPERTY(Transient)
	FPlayerStackArchive PlayerStackData;

	UPROPERTY(Transient)
	TArray<FLevelScriptSaveData> SavedScripts;

	UPROPERTY(Transient)
	FGameObjectSaveData SavedGameMode;

	UPROPERTY(Transient)
	FGameObjectSaveData SavedGameState;

	UPROPERTY(Transient)
	FControllerSaveData SavedController;

	UPROPERTY(Transient)
	FPawnSaveData SavedPawn;

	UPROPERTY(Transient)
	FGameObjectSaveData SavedPlayerState;

	UPROPERTY(Transient)
	UInfoSaveGame* CachedSlotInfoSave;

	UPROPERTY(Transient)
	TMap<FString, UCustomSaveGame*> CachedCustomSaves;

	UPROPERTY(Transient)
	TArray<FActorSaveData> WorldPartitionActors;
	/** Blueprint Library function accessors */
	

	// Blueprint Functions
	
public:
	UCustomSaveGame* GetCustomSave(const TSubclassOf<UCustomSaveGame>& SaveGameClass);
	bool SaveCustom(UCustomSaveGame* SaveGame);
	bool SaveAllCustomObjects();
	void DeleteCustomSave(UCustomSaveGame* SaveGame);
	
	void SetCurrentSaveGameName(const FString & SaveGameName);
	void SetCurrentSaveUserName(const FString& UserName);
	
	TArray<FString> GetSortedSaveSlots() const;
	TArray<FString> GetAllSaveUsers() const;
	
	UTexture2D* ImportSaveThumbnail(const FString& SaveGameName);
	void ExportSaveThumbnail(UTextureRenderTarget2D* TextureRenderTarget, const FString& SaveGameName);
	
	void DeleteAllSaveDataForSlot(const FString& SaveGameName);
	void DeleteAllSaveDataForUser(const FString& UserName);

	/** Other public Functions  */

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
	virtual class UWorld* GetWorld() const override;
	
	FString GetCurrentSaveGameName() const;
	void SaveSlotInfoObject();
	UInfoSaveGame* GetSlotInfoObject(const FString& SaveGameName = FString());
	
	bool DoesSaveGameExist(const FString& SaveGameName) const;
	
	void PrepareLoadAndSaveActors(const uint32& Flags, const EAsyncCheckType& FunctionType, const bool& bFullReload);
	
	bool SavePlayerActors();
	void LoadPlayerActors(UAsyncLoadGame* LoadTask);
	
	bool SaveLevelActors();
	void LoadLevelActors(UAsyncLoadGame* LoadTask);
	
	void StartLoadLevelActors(UAsyncLoadGame* LoadTask);
	void LoadAllLevelActors(UAsyncLoadGame* LoadTask);
	
	bool SpawnOrUpdateLevelActor(const FActorSaveData& ActorArray);
	EUpdateActorResult UpdateLevelActor(const FActorSaveData& ActorArray);
	void SpawnLevelActor(const FActorSaveData& ActorArray);
	void ProcessLevelActor(AActor* Actor, const FActorSaveData& ActorArray);
	
	void LogFailSpawnLevelActor(const FActorSaveData& ActorArray);
	void LogFinishLoadingLevel();
	
	bool TryLoadPlayerFile();
	bool TryLoadLevelFile();
	
	APlayerController* GetPlayerController() const;
	APawn* GetPlayerPawn() const;
	
	FTimerManager& GetTimerManager() const;
	
	bool IsAsyncSaveOrLoadTaskActive(const ESaveGameMode& Mode = ESaveGameMode::MODE_All, const EAsyncCheckType& CheckType = EAsyncCheckType::CT_Both, const bool& bLogAndReturnError = true) const;
	
	bool HasValidGameMode() const;
	bool HasValidPlayer() const;

/** Internal Functions  */

private:

	bool VerifyOrCreateDirectory(const FString& NewDir);
 
	bool SaveBinaryArchive(FBufferArchive& BinaryData, const FString& FullSavePath);
	bool LoadBinaryArchive(const EDataLoadType& LoadType, const FString& FullSavePath, UObject* Object = nullptr);
	bool UnpackBinaryArchive(const EDataLoadType& LoadType, FMemoryReader FromBinary, UObject* Object = nullptr);
	bool UnpackLevel(const FLevelArchive& LevelArchive);
	bool UnpackPlayer(const FPlayerArchive& PlayerArchive);
	
	void DirectSetPlayerPosition(const FPlayerPositionArchive& PosArchive);
	void ClearPlayerPosition();
	
	USaveGame* CreateSaveObject(const TSubclassOf<USaveGame>& SaveGameClass);
	bool SaveObject(const FString& FullSavePath, USaveGame* SaveGameObject);
	USaveGame* LoadObject(const FString& FullSavePath, TSubclassOf<USaveGame> SaveGameClass);
 
	FGameObjectSaveData ParseGameModeObjectForSaving(AActor* Actor, const bool& bWorldPartition);
	FActorSaveData ParseLevelActorForSaving(AActor* Actor, const EActorType& Type);
 
	bool SaveBinaryData(const TArray<uint8>& SavedData, const FString& FullSavePath) const;
 
	void SaveActorToBinary(AActor* Actor, FGameObjectSaveData& OutData, const bool& bKeepLoadedTag = false);
	void LoadActorFromBinary(AActor* Actor, const FGameObjectSaveData& InData);
 
	void SerializeToBinary(UObject* Object, TArray<uint8>& OutData);
	void SerializeFromBinary(UObject* Object, const TArray<uint8>& InData);
 
	void SerializeActorStructProperties(AActor* Actor);
	void SerializeStructProperties(UObject* Object);
	void SerializeScriptStruct(UStruct* ScriptStruct);
	void SerializeArrayStruct(FArrayProperty* ArrayProp);
	void SerializeMap(FMapProperty* MapProp);
 
	bool HasComponentSaveInterface(const UActorComponent* Comp) const;
	TArray<UActorComponent*> GetSaveComponents(AActor* Actor) const;
 
	void SaveActorComponents(AActor* Actor, TArray<FComponentSaveData>& OutComponents);
	void LoadActorComponents(AActor* Actor, const TArray<FComponentSaveData>& InComponents);
 
	bool HasSaveArchiveError(const FBufferArchive& CheckArchive, const ESaveErrorType& ErrorType) const;
 
	bool HasSaveInterface(const AActor* Actor) const;
	bool IsValidActor(const AActor* Actor) const;
	bool IsValidForSaving(const AActor* Actor) const;
	bool IsValidForLoading(const AActor* Actor) const;
 
	bool HasValidTransform(const FTransform& CheckTransform) const;
	bool SaveAsLevelActor(const APawn* Pawn) const;
 
	bool CheckForExistingActor(const FActorSaveData& ActorArray);
 
	EActorType GetActorType(const AActor* Actor) const;
    bool IsMovable(const USceneComponent* SceneComp) const;
	bool CanProcessActorTransform(const AActor* Actor) const;
 
	FName GetLevelName() const;
 
	TArray<FString> GetAllSaveGames() const;
	TArray<FString> GetSaveSlotsDesktop() const;
	TArray<FString> GetSaveSlotsConsole() const;
 
	template<class T>
	bool CheckLoadIterator(const T& It, const ESaveGameMode& Mode, const bool& bLog, const FString& DebugString) const;
 //
	template <class TSaveGame>
	TSaveGame* GetDesiredSaveObject(const FString& FullSavePath, const FSoftClassPath& InClassName, TSaveGame*& SaveGameObject);
 
	FLevelStackArchive AddMultiLevelStackData(const FLevelArchive& LevelArchive, const FGameObjectSaveData& InGameMode, const FGameObjectSaveData& InGameState);
	FLevelArchive AddMultiLevelStreamData(const FLevelArchive& LevelArchive);
	void UpdateMultiLevelStreamData(const FLevelArchive& LevelArchive);
 
	bool HasRenderTargetResource(UTextureRenderTarget2D* TextureRenderTarget) const;
	bool CompressRenderTarget(UTextureRenderTarget2D* TexRT, FArchive& Ar);
	bool ExportRenderTarget(UTextureRenderTarget2D* TexRT, const FString& FileName);
 
	TArray<uint8> BytesFromString(const FString& String) const { return FSaveHelpers::BytesFromString(String); };
	FString StringFromBytes(const TArray<uint8>& Bytes) const { return FSaveHelpers::StringFromBytes(Bytes); };

	/** Settings Helpers  */

public:

	FORCEINLINE bool IsNormalMultiLevelSave() const
	{
		return USaveLoadSettings::Get()->MultiLevelSaving == EMultiLevelSaveMethod::ML_Normal;
	}

	FORCEINLINE bool IsStreamMultiLevelSave() const
	{
		return USaveLoadSettings::Get()->MultiLevelSaving == EMultiLevelSaveMethod::ML_Stream;
	}

	FORCEINLINE bool IsFullMultiLevelSave() const
	{
		return USaveLoadSettings::Get()->MultiLevelSaving == EMultiLevelSaveMethod::ML_Full;
	}

	FORCEINLINE bool IsStackBasedMultiLevelSave() const
	{
		return IsFullMultiLevelSave() || IsNormalMultiLevelSave();
	}

	FORCEINLINE bool IsContainingStreamMultiLevelSave() const
	{
		return IsFullMultiLevelSave() || IsStreamMultiLevelSave();
	}

	FORCEINLINE bool IsConsoleFileSystem() const
	{
		return USaveLoadSettings::Get()->FileSaveMethod == EFileSaveMethod::FM_Console;
	}

	FORCEINLINE bool IsPersistentGameMode() const
	{
		return USaveLoadSettings::Get()->bPersistentGameMode;
	}

	FORCEINLINE bool IsPersistentPlayer() const
	{
		return USaveLoadSettings::Get()->bPersistentPlayer
			&& USaveLoadSettings::Get()->MultiLevelSaving == EMultiLevelSaveMethod::ML_Disabled;
	}

	FORCEINLINE FName GetCompressionMethod()
	{
		return USaveLoadSettings::Get()->CompressionMethod == ECompressionMethod::Zlib ? NAME_Zlib : NAME_Oodle;
	}

	FORCEINLINE FString GetThumbnailFormat() const
	{
		if (USaveLoadSettings::Get()->ThumbnailFormat == EThumbnailImageFormat::Png)
		{
			return SaveLoadConstants::ImgFormatPNG;
		}

		return SaveLoadConstants::ImgFormatJPG;
	}

	FORCEINLINE FString GetThumbnailFileExtension() const
	{
		if (IsConsoleFileSystem())
		{
			return SaveLoadConstants::SaveType;
		}

		return TEXT(".") + GetThumbnailFormat();
	}

/** File Access and Path Names  */

public:

	FORCEINLINE FString SaveUserDir() const
	{
		return FPaths::ProjectSavedDir() + TEXT("SaveGames/Users/");
	}

	FORCEINLINE FString UserSubDir() const
	{
		//Takes into account the already defined path from ISaveGame
		return TEXT("Users/") + CurrentSaveUserName + SaveLoadConstants::Slash;
	}

	FORCEINLINE FString BaseSaveDir() const
	{
		if (!CurrentSaveUserName.IsEmpty())
		{
			return SaveUserDir() + CurrentSaveUserName + SaveLoadConstants::Slash;
		}

		return FPaths::ProjectSavedDir() + SaveLoadConstants::SaveGamesFolder;
	}

	FORCEINLINE static FString ConsoleSaveDir()
	{
		return FPaths::ProjectSavedDir() + SaveLoadConstants::SaveGamesFolder;
	}

	FORCEINLINE FString AllThumbnailFiles() const
	{
		return BaseSaveDir() + TEXT("*") + GetThumbnailFileExtension();
	}

	FORCEINLINE FString GetFolderOrFile() const
	{
		//Console uses file names and not folders, "/" will automatically create a new folder.
		const bool bFile = IsConsoleFileSystem();
		const FString FolderOrFile = bFile ? SaveLoadConstants::Underscore : SaveLoadConstants::Slash;

		return FolderOrFile;
	}

	FORCEINLINE FString FullSaveDir(const FString& DataType, FString SaveGameName = FString()) const
	{
		if (SaveGameName.IsEmpty())
		{
			SaveGameName = GetCurrentSaveGameName();
		}

		// *.sav is added by ISaveInterface
		const FString FullName = SaveGameName + GetFolderOrFile() + DataType;

		if (!CurrentSaveUserName.IsEmpty())
		{
			return UserSubDir() + FullName;
		}

		return FullName;
	}

	FORCEINLINE FString CustomSaveFile(const FString& CustomSaveName, const bool bUseSlot = false) const
	{
		//Bound to a save slot, use default dir.
		if (bUseSlot)
		{
			return FullSaveDir(CustomSaveName);
		}

		//Not bound to slot, so we just save in the base folder. with user if desired.
		if (!CurrentSaveUserName.IsEmpty())
		{
			return UserSubDir() + CustomSaveName;
		}

		return CustomSaveName;
	}

	FORCEINLINE FString SlotInfoSaveFile(const FString& SaveGameName = FString()) const
	{
		return FullSaveDir(SaveLoadConstants::SlotSuffix, SaveGameName);
	}

	FORCEINLINE FString ActorSaveFile(const FString& SaveGameName = FString()) const
	{
		return FullSaveDir(SaveLoadConstants::ActorSuffix, SaveGameName);
	}

	FORCEINLINE FString PlayerSaveFile(const FString& SaveGameName = FString())  const
	{
		return FullSaveDir(SaveLoadConstants::PlayerSuffix, SaveGameName);
	}

	FORCEINLINE FString ThumbnailSaveFile(const FString& SaveGameName) const
	{
		const FString ThumbnailPath = BaseSaveDir() + SaveGameName + GetFolderOrFile();
		return ThumbnailPath + TEXT("Thumb") + GetThumbnailFileExtension();
	}

	FORCEINLINE FString SlotFilePath(const FString& SaveGameName = FString()) const
	{
		//This is only used for sorting.
		return BaseSaveDir() + SlotInfoSaveFile(SaveGameName) + SaveLoadConstants::SaveType;
	}

	FORCEINLINE FName LevelScriptSaveName(const AActor* Actor) const
	{
		return FName(*Actor->GetLevel()->GetOuter()->GetName());
	}

	FORCEINLINE FString GetFullActorName(const AActor* Actor) const
	{
		const FString ActorName = Actor->GetName();

		//World Partition has own unique Actor Ids
		if (CanSupportWorldPartition())
		{
			return ActorName;
		}

		//Stream and Full always require complex Actor names.
		if (!IsContainingStreamMultiLevelSave())
		{
			// if (USaveLoadPluginSettings::Get()->bSimpleActorNames)
			// {
			// 	return ActorName;
			// }
		}

		//This is only valid for placed Actors. Runtime Actors are always in the persistent.
		//Can't use GetActorType here, since it would crash Multi-Thread loading.
		if (IsPlacedActor(Actor))
		{
			const FString LevelName = Actor->GetLevel()->GetOuter()->GetName();
			const bool bAlreadyHas = ActorName.Contains(LevelName);

			if (bAlreadyHas)
			{
				return ActorName;
			}
			else
			{
				return LevelName + SaveLoadConstants::Underscore + ActorName;
			}
		}
	
		return ActorName;
	}

	FORCEINLINE FString ValidateSaveName(const FString& SaveGameName) const
	{
		FString CurrentSave = SaveGameName;
		CurrentSave = CurrentSave.Replace(TEXT(" "), *SaveLoadConstants::Underscore);
		CurrentSave = CurrentSave.Replace(TEXT("."), *SaveLoadConstants::Underscore);

		return FPaths::MakeValidFileName(*CurrentSave);
	}

	FORCEINLINE bool IsTimeSince(const float& LastTime, const float& CheckTime)
	{
		return GetWorld()->TimeSince(LastTime) >= CheckTime;
	}

/** Helper Functions  */

public:

	FORCEINLINE bool IsPlacedActor(const AActor* Actor) const
	{
		return Actor->IsNetStartupActor();
	}

	FORCEINLINE void ClearSavedLevelActors()
	{
		SavedActors.Empty();
		SavedScripts.Empty();
	}

	FORCEINLINE void ClearCachedSlot()
	{
		CachedSlotInfoSave = nullptr;
	}

	FORCEINLINE void ClearMultiLevelSave()
	{
		LevelArchiveList.Empty();
		MultiLevelStreamData = FMultiLevelStreamingData();
		PlayerStackData = FPlayerStackArchive();
	}

	FORCEINLINE void ClearCachedCustomSaves()
	{
		CachedCustomSaves.Empty();
	}

	FORCEINLINE UClass* StaticLoadSpawnClass(const FString& Class) const
	{
		return Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, *Class, nullptr, LOAD_None, nullptr));
	}

	FORCEINLINE bool CanSupportWorldPartition() const
	{
		const UWorld* InWorld = GetWorld();
		if (InWorld)
		{
			if (IsContainingStreamMultiLevelSave() && InWorld && InWorld->GetWorldPartition())
			{
				return true;
			}
		}

		return false;
	}
};

template <class T>
bool UGameSaveSubsystem::CheckLoadIterator(const T& It, const ESaveGameMode& Mode, const bool& bLog,
	const FString& DebugString) const
{
	if (It && It->IsActive() && (It->Mode == Mode || Mode == ESaveGameMode::MODE_All))
	{
		if (bLog)
		{
			UE_LOG(LogGameSave, Warning, TEXT("%s is active while trying to save or load."), *DebugString);
		}

		return true;
	}

	return false;
}

template <class TSaveGame>
TSaveGame* UGameSaveSubsystem::GetDesiredSaveObject(const FString& FullSavePath, const FSoftClassPath& InClassName,
	TSaveGame*& SaveGameObject)
{
	if (FullSavePath.IsEmpty())
	{
		return nullptr;
	}

	//If we don't have a cached one, we load it and set the cached one
	if (!SaveGameObject)
	{
		const FSoftClassPath LocalClassName = InClassName;
		TSubclassOf<TSaveGame> Class = LocalClassName.TryLoadClass<TSaveGame>();

		if (!Class)
		{
			UE_LOG(LogGameSave, Error, TEXT("Invalid Save Game Object Class: %s"), *LocalClassName.ToString());
			return nullptr;
		}

		USaveGame* SaveGame = LoadObject(FullSavePath, Class);
		SaveGameObject = Cast<TSaveGame>(SaveGame);
	}

	if (!SaveGameObject)
	{
		UE_LOG(LogGameSave, Error, TEXT("Invalid Save Game Object: %s"), *FullSavePath);
		return nullptr;
	}

	return SaveGameObject;
}
