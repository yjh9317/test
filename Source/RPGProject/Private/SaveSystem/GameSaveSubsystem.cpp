// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveSystem/GameSaveSubsystem.h"

#include "EngineUtils.h"
#include "ImageUtils.h"
#include "PlatformFeatures.h"
#include "SaveGameSystem.h"
#include "Async/Async.h"
#include "Engine/LevelScriptActor.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "SaveSystem/RPGActorSaveInterface.h"
#include "SaveSystem/RPGCompSaveInterface.h"
#include "Serialization/ArchiveLoadCompressedProxy.h"
#include "Serialization/ArchiveSaveCompressedProxy.h"


UGameSaveSubsystem* UGameSaveSubsystem::Get(UObject* WorldContextObject)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World)
	{
		const UGameInstance* GameInst = World->GetGameInstance();
		if (GameInst)
		{
			UGameSaveSubsystem* SubSystem = GameInst->GetSubsystem<UGameSaveSubsystem>();
			if (SubSystem)
			{
				return SubSystem;
			}
		}
	}

	return nullptr;
}

bool UGameSaveSubsystem::SaveCustom(UCustomSaveGame* SaveGame)
{
	if (!IsValid(SaveGame))
	{
		return false;
	}

	const bool bUseSlot = SaveGame->bUseSaveSlot;
	const FString CustomSaveName = SaveGame->SaveGameName;

	if (SaveObject(*CustomSaveFile(CustomSaveName, bUseSlot), SaveGame))
	{
		UE_LOG(LogGameSave, Log, TEXT("Custom Save Game saved: %s"), *CustomSaveName);
		return true;
	}

	return false;
}

bool UGameSaveSubsystem::SaveAllCustomObjects()
{
	bool bSuccess = false;
	bool bUseSlot = false;

	for (auto& CustomObjectPair : CachedCustomSaves)
	{
		UCustomSaveGame* SaveGame = CustomObjectPair.Value;
		if (SaveGame)
		{
			if (SaveGame->bUseSaveSlot)
			{
				bUseSlot = true;
			}

			bSuccess = SaveCustom(SaveGame);
		}
	}

	//With at least one object using slots, we also save the slot info.
	if (bUseSlot)
	{
		SaveSlotInfoObject();
	}

	return bSuccess;
}

void UGameSaveSubsystem::DeleteCustomSave(UCustomSaveGame* SaveGame)
{
	if (!SaveGame)
	{
		return;
	}

	const bool bUseSlot = SaveGame->bUseSaveSlot;
	const FString CustomSaveName = SaveGame->SaveGameName;
	const FString SaveFile = CustomSaveFile(CustomSaveName, bUseSlot);
	
	bool bSuccess = false;

	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	if(SaveSystem->DoesSaveGameExist(*SaveFile, PlayerIndex))
	{
		bSuccess = SaveSystem->DeleteGame(false, *SaveFile, PlayerIndex);
	}

	if (bSuccess)
	{
		const FString CurrentSave = GetCurrentSaveGameName();
		const FString CachedRefName = bUseSlot ? CustomSaveName + CurrentSave : CustomSaveName;

		const UCustomSaveGame* CachedObject = CachedCustomSaves.FindRef(CachedRefName);
		if (CachedObject)
		{
			CachedCustomSaves.Remove(CachedRefName);
		}

		UE_LOG(LogGameSave, Log, TEXT("Custom Save Game Deleted: %s"), *CustomSaveName);
	}
}

void UGameSaveSubsystem::SetCurrentSaveGameName(const FString& SaveGameName)
{
	if (CurrentSaveGameName != SaveGameName)
	{
		ClearCachedSlot();

		CurrentSaveGameName = ValidateSaveName(SaveGameName);
		SaveConfig();

		UE_LOG(LogGameSave, Log, TEXT("New Current Save Game Slot is: %s"), *CurrentSaveGameName);
	}
}

void UGameSaveSubsystem::SetCurrentSaveUserName(const FString& UserName)
{
	if (IsConsoleFileSystem())
	{
		UE_LOG(LogGameSave, Warning, TEXT("Save Users are not supported when using the console file system."));
		return;
	}

	if (CurrentSaveUserName != UserName)
	{
		ClearCachedSlot();
		ClearCachedCustomSaves();

		CurrentSaveUserName = UserName;
		SaveConfig();

		UE_LOG(LogGameSave, Log, TEXT("New Current Save User Name is: %s"), *UserName);
	}
}

TArray<FString> UGameSaveSubsystem::GetSortedSaveSlots() const
{
	if (IsConsoleFileSystem())
	{
		return GetSaveSlotsConsole();
	}

	return GetSaveSlotsDesktop();
}

TArray<FString> UGameSaveSubsystem::GetAllSaveUsers() const
{
	TArray<FString> SaveUserNames;
	IFileManager::Get().FindFiles(SaveUserNames, *FPaths::Combine(SaveUserDir(), TEXT("*")), false, true);

	return SaveUserNames;
}

UTexture2D* UGameSaveSubsystem::ImportSaveThumbnail(const FString& SaveGameName)
{
	const FString SaveThumbnailName = ThumbnailSaveFile(SaveGameName);

	//Suppress warning messages when we dont have a thumb yet.
	if (FPaths::FileExists(SaveThumbnailName))
	{
		return FImageUtils::ImportFileAsTexture2D(SaveThumbnailName);
	}
	
	return nullptr;
}

void UGameSaveSubsystem::ExportSaveThumbnail(UTextureRenderTarget2D* TextureRenderTarget, const FString& SaveGameName)
{
	if (!TextureRenderTarget)
	{
		UE_LOG(LogGameSave, Warning, TEXT("ExportSaveThumbnailRT: TextureRenderTarget must be non-null"));
	}
	else if (!HasRenderTargetResource(TextureRenderTarget))
	{
		UE_LOG(LogGameSave, Warning, TEXT("ExportSaveThumbnailRT: Render target has been released"));
	}
	else if (SaveGameName.IsEmpty())
	{
		UE_LOG(LogGameSave, Warning, TEXT("ExportSaveThumbnailRT: FileName must be non-empty"));
	}
	else
	{
		const FString SaveThumbnailName = ThumbnailSaveFile(SaveGameName);
		const bool bSuccess = ExportRenderTarget(TextureRenderTarget, SaveThumbnailName);

		if (!bSuccess)
		{
			UE_LOG(LogGameSave, Warning, TEXT("ExportSaveThumbnailRT: FileWrite failed to create"));
		}
	}
}

void UGameSaveSubsystem::DeleteAllSaveDataForSlot(const FString& SaveGameName)
{
	ClearCachedSlot();

	bool bSuccess = false;

	//Console uses files and not folders
	if (IsConsoleFileSystem())
	{
		TArray<FString> AllFiles;

		//Gather all save files
		const TArray<FString> SaveGameNames = GetAllSaveGames();

		for (const FString& FileName : SaveGameNames)
		{
			if (FileName.Contains(SaveGameName))
			{
				AllFiles.Add(FileName);
			}
		}

		//Gather all thumbnail files
		TArray<FString> SaveThumbFiles;
		IFileManager::Get().FindFiles(SaveThumbFiles, *AllThumbnailFiles(), true, false);
		for (const FString& FileName : SaveThumbFiles)
		{
			if (FileName.Contains(SaveGameName))
			{
				AllFiles.Add(FileName);
			}
		}

		//Use native delete 
		ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
		for (const FString& FileName : AllFiles)
		{
			if (*FileName)
			{
				SaveSystem->DeleteGame(false, *FileName, PlayerIndex);
				bSuccess = true;
			}
		}

		if (bSuccess)
		{
			UE_LOG(LogGameSave, Log, TEXT("Save Game Data removed for: %s"), *SaveGameName);
		}
	}
	else
	{
		const FString SaveFile = FPaths::Combine(UGameSaveSubsystem::BaseSaveDir(), SaveGameName);

		bSuccess = IFileManager::Get().DeleteDirectory(*SaveFile, true, true);
		if (bSuccess)
		{
			UE_LOG(LogGameSave, Log, TEXT("Save Game Data removed for: %s"), *SaveGameName);
		}
	}

	//Delete the cached custom save objects
	TMap<FString, UCustomSaveGame*> TempCustomSaves = CachedCustomSaves;
	for (auto It = TempCustomSaves.CreateIterator(); It; ++It)
	{
		if (It)
		{
			if (It->Key.Contains(SaveGameName))
			{
				CachedCustomSaves.Remove(It->Key);
			}
		}
	}

}

void UGameSaveSubsystem::DeleteAllSaveDataForUser(const FString& UserName)
{
	ClearCachedSlot();
	ClearCachedCustomSaves();

	const FString UserSaveFile = SaveUserDir() + UserName;
	bool bSuccess = false;

	//Try removing folder	
	bSuccess = IFileManager::Get().DeleteDirectory(*UserSaveFile, true, true);
	if (bSuccess)
	{
		UE_LOG(LogGameSave, Log, TEXT("Save Game User Data removed for: %s"), *UserName);
	}
}

FString UGameSaveSubsystem::GetCurrentSaveGameName() const
{
	const FString DefaultName = USaveLoadSettings::Get()->DefaultSaveGameName;

	if (CurrentSaveGameName.IsEmpty())
	{
		return DefaultName;
	}

	return CurrentSaveGameName;
}

void UGameSaveSubsystem::SaveSlotInfoObject()
{
	const FString SaveGameName = GetCurrentSaveGameName();
	if (VerifyOrCreateDirectory(SaveGameName))
	{
		UInfoSaveGame* SaveGame = GetSlotInfoObject();
		if (SaveGame)
		{
			//GetSortedSaveSlots() only uses the file data, this uses actual saved data.
			SaveGame->SlotInfo.Name = SaveGameName;
			SaveGame->SlotInfo.TimeStamp = FDateTime::Now();
			SaveGame->SlotInfo.Level = GetLevelName();

			TArray<FString> PlayerNames;
			if (const AGameStateBase* GameState = GetWorld()->GetGameState())
			{
				const TArray<APlayerState*> Players = GameState->PlayerArray;
				if (!SaveLoadConstants::ArrayEmpty(Players))
				{
					for (const APlayerState* PlayerState : Players)
					{
						PlayerNames.Add(PlayerState->GetPlayerName());
					}

					SaveGame->SlotInfo.Players = PlayerNames;
				}
			}

			SaveObject(*SlotInfoSaveFile(), SaveGame);

			UE_LOG(LogGameSave, Log, TEXT("Slot Info saved: %s"), *SaveGameName);
		}
	}
}

UInfoSaveGame* UGameSaveSubsystem::GetSlotInfoObject(const FString& SaveGameName)
{
	//We need to clear the cached one for Get Named Slot Info
	if (!SaveGameName.IsEmpty())
	{
		CachedSlotInfoSave = nullptr;
	}

	return GetDesiredSaveObject<UInfoSaveGame>(
		SlotInfoSaveFile(SaveGameName),
		USaveLoadSettings::Get()->SlotInfoSaveGameClass,
		CachedSlotInfoSave);
}

bool UGameSaveSubsystem::DoesSaveGameExist(const FString& SaveGameName) const
{
	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	return SaveSystem->DoesSaveGameExist(*SlotInfoSaveFile(SaveGameName), PlayerIndex);
}

void UGameSaveSubsystem::PrepareLoadAndSaveActors(const uint32& Flags, const EAsyncCheckType& FunctionType,
	const bool& bFullReload)
{
	TArray<AActor*> Actors;
	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor && IsValidActor(Actor))
		{	
			const EActorType Type = GetActorType(Actor);

			//Load exclusive checks
			if (FunctionType == EAsyncCheckType::CT_Load)
			{
				if (bFullReload)
				{
					bool bIsPlayer = false;
					if (GetPlayerController())
					{
						bIsPlayer = (Actor == GetPlayerController()) || (Actor == GetPlayerPawn()) || (Actor == GetPlayerController()->PlayerState);
					}

					if (Flags & ENUM_TO_FLAG(ELoadTypeFlags::LF_Player))
					{
						if (bIsPlayer)
						{
							Actor->Tags.Remove(SaveLoadConstants::HasLoadedTag);
						}
					}

					if (Flags & ENUM_TO_FLAG(ELoadTypeFlags::LF_Level))
					{
						if (!bIsPlayer)
						{
							Actor->Tags.Remove(SaveLoadConstants::HasLoadedTag);
						}
					}
				}
				else
				{
					//Don't consider loaded placed Actors at all. Runtime Actors require another check, otherwise they would duplicate.
					if (Type == EActorType::AT_Placed && Actor->ActorHasTag(SaveLoadConstants::HasLoadedTag))
					{
						continue;
					}
				}
			}

			//For all, including player, a tick before actual save/load
			if (USaveLoadSettings::Get()->bAutoSaveStructs)
			{
				SerializeActorStructProperties(Actor);
			}

			if (Type == EActorType::AT_Runtime || Type == EActorType::AT_Placed || Type == EActorType::AT_LevelScript || Type == EActorType::AT_Persistent)
			{
				Actors.Add(Actor);
			}
		}
	}

	ActorList.Empty();
	ActorList = Actors;
}

bool UGameSaveSubsystem::SavePlayerActors()
{
	bool bHasPlayerSaveData = false;

	//Controller
	FControllerSaveData InController;
	APlayerController* Controller = GetPlayerController();
	if (Controller && IsValidForSaving(Controller))
	{
		if (!Controller->ActorHasTag(SaveLoadConstants::SkipTransformTag))
		{
			InController.Rotation = Controller->GetControlRotation();
		}

		SaveActorToBinary(Controller, InController.SaveData);
		bHasPlayerSaveData = true;
	}

	//Pawn
	FPawnSaveData InPawn;
	APawn* Pawn = GetPlayerPawn();
	if (Pawn && IsValidForSaving(Pawn) && !SaveAsLevelActor(Pawn))
	{
		if (!Pawn->ActorHasTag(SaveLoadConstants::SkipTransformTag))
		{
			InPawn.Position = Pawn->GetActorLocation();
			InPawn.Rotation = Pawn->GetActorRotation();
		}

		SaveActorToBinary(Pawn, InPawn.SaveData);
		bHasPlayerSaveData = true;
	}

	//Player State
	FGameObjectSaveData InPlayerState;
	APlayerState* PlayerState = GetPlayerController()->PlayerState;
	if (PlayerState && IsValidForSaving(PlayerState))
	{
		SaveActorToBinary(PlayerState, InPlayerState);
		bHasPlayerSaveData = true;
	}

	//Without any data, we can just return here
	if (!bHasPlayerSaveData)
	{
		return true;
	}

	FPlayerArchive PlayerArchive;
	{
		PlayerArchive.SavedController = InController;
		PlayerArchive.SavedPawn = InPawn;
		PlayerArchive.SavedPlayerState = InPlayerState;
		PlayerArchive.Level = GetLevelName();
	}

	FBufferArchive PlayerData;

	//Check for multi level saving.
	if (IsStackBasedMultiLevelSave())
	{
		PlayerStackData.ReplaceOrAdd(PlayerArchive);
		FPlayerStackArchive PlayerStack = PlayerStackData;
		PlayerData << PlayerStack;
	}
	else
	{
		PlayerData << PlayerArchive;
	}

	if (!HasSaveArchiveError(PlayerData, ESaveErrorType::ER_Player) && SaveBinaryArchive(PlayerData, PlayerSaveFile()))
	{
		UE_LOG(LogGameSave, Log, TEXT("Player Actors have been saved"));
		return true;
	}
	else
	{
		UE_LOG(LogGameSave, Warning, TEXT("Failed to save Player"));
	}

	return false;
}

void UGameSaveSubsystem::LoadPlayerActors(UAsyncLoadGame* LoadTask)
{
	//Controller
	APlayerController* Controller = GetPlayerController();
	if (Controller && IsValidForLoading(Controller))
	{
		const FControllerSaveData ControllerData = SavedController;

		if (!IsPersistentPlayer() && !ControllerData.Rotation.IsNearlyZero())
		{
			Controller->SetControlRotation(ControllerData.Rotation);
		}

		LoadActorFromBinary(Controller, ControllerData.SaveData);

		UE_LOG(LogGameSave, Log, TEXT("Player Controller loaded"));
	}

	//Pawn
	APawn* Pawn = GetPlayerPawn();
	if (Pawn && IsValidForLoading(Pawn) && !SaveAsLevelActor(Pawn))
	{
		const FPawnSaveData PawnData = SavedPawn;

		if (!IsPersistentPlayer() && !PawnData.Position.IsNearlyZero())
		{
			Pawn->SetActorLocation(PawnData.Position, false, nullptr, ETeleportType::TeleportPhysics);
			Pawn->SetActorRotation(PawnData.Rotation, ETeleportType::TeleportPhysics);
		}

		LoadActorFromBinary(Pawn, PawnData.SaveData);

		UE_LOG(LogGameSave, Log, TEXT("Player Pawn loaded"));
	}

	//Player State
	if (!SaveLoadConstants::ArrayEmpty(SavedPlayerState.Data))
	{
		APlayerState* PlayerState = GetPlayerController()->PlayerState;
		if (PlayerState && IsValidForLoading(PlayerState))
		{
			LoadActorFromBinary(PlayerState, SavedPlayerState);

			UE_LOG(LogGameSave, Log, TEXT("Player State loaded"))
		}
	}
}

bool UGameSaveSubsystem::SaveLevelActors()
{
	TArray<FActorSaveData> InActors;
	TArray<FLevelScriptSaveData> InScripts;
	FGameObjectSaveData InGameMode;
	FGameObjectSaveData InGameState;

	FScopeLock Lock(&ActorCriticalSection);

	for (AActor* Actor : ActorList)
	{
		if (Actor && IsValidForSaving(Actor))
		{
			const EActorType Type = GetActorType(Actor);

			//Add Level Actor and Component Data
			if (Type == EActorType::AT_Runtime || Type == EActorType::AT_Placed || Type == EActorType::AT_Persistent)
			{
				const FActorSaveData ActorArray = ParseLevelActorForSaving(Actor, Type);
				InActors.Add(ActorArray);
			}
			//Add Level Script Data
			else if (Type == EActorType::AT_LevelScript)
			{
				FLevelScriptSaveData ScriptArray;
				ScriptArray.Name = LevelScriptSaveName(Actor);

				SaveActorToBinary(Actor, ScriptArray.SaveData);
				InScripts.Add(ScriptArray);
			}
		}
	}

	//World Partition Actors
	bool bIsWorldPartition = false;
	if (!SaveLoadConstants::ArrayEmpty(WorldPartitionActors))
	{
		InActors.Append(WorldPartitionActors);
		WorldPartitionActors.Empty();
		bIsWorldPartition = true;
	}

	//Game Mode/State Actors
	InGameMode = ParseGameModeObjectForSaving(GetWorld()->GetAuthGameMode(), bIsWorldPartition);
	InGameState = ParseGameModeObjectForSaving(GetWorld()->GetGameState(), bIsWorldPartition);

	FBufferArchive LevelData;

	FLevelArchive LevelArchive;
	{
		LevelArchive.SavedActors = InActors;
		LevelArchive.SavedScripts = InScripts;

		//Stack based only has one set of data for mode and state
		if (!IsStackBasedMultiLevelSave())
		{
			LevelArchive.SavedGameMode = InGameMode;
			LevelArchive.SavedGameState = InGameState;
		}

		LevelArchive.Level = GetLevelName();
	}

	//Check for multi level saving.
	if (IsNormalMultiLevelSave())
	{
		FLevelStackArchive LevelStack = AddMultiLevelStackData(LevelArchive, InGameMode, InGameState);
		LevelData << LevelStack;
	}
	else if (IsStreamMultiLevelSave())
	{
		FLevelArchive StreamArchive = AddMultiLevelStreamData(LevelArchive);
		LevelData << StreamArchive;
	}
	else if (IsFullMultiLevelSave())
	{
		const FLevelArchive StreamArchive = AddMultiLevelStreamData(LevelArchive);
		FLevelStackArchive MultiLevelStack = AddMultiLevelStackData(StreamArchive, InGameMode, InGameState);
		LevelData << MultiLevelStack;
	}
	else
	{
		LevelData << LevelArchive;
	}

	//Save and log
	if (!HasSaveArchiveError(LevelData, ESaveErrorType::ER_Level) && SaveBinaryArchive(LevelData, ActorSaveFile()))
	{
		UE_LOG(LogGameSave, Log, TEXT("Level and Game Actors have been saved"));
		return true;
	}
	else
	{
		UE_LOG(LogGameSave, Warning, TEXT("Failed to save Level Actors"));
	}

	return false;
}

void UGameSaveSubsystem::LoadLevelActors(UAsyncLoadGame* LoadTask)
{
	//Level Scripts
	if (!SaveLoadConstants::ArrayEmpty(SavedScripts))
	{
		for (AActor* Actor : ActorList)
		{
			if (Actor && IsValidForLoading(Actor) && GetActorType(Actor) == EActorType::AT_LevelScript)
			{
				for (const FLevelScriptSaveData& ScriptArray : SavedScripts)
				{
					//Compare by level name, since the engine creates multiple script actors.
					const FName ScriptName = LevelScriptSaveName(Actor);

					if (ScriptArray.Name == ScriptName)
					{
						UE_LOG(LogGameSave, Log, TEXT("%s Blueprint Loaded"), *ScriptName.ToString());

						LoadActorFromBinary(Actor, ScriptArray.SaveData);
					}
				}
			}
		}
	}

	//Game Mode Actor
	if (!SaveLoadConstants::ArrayEmpty(SavedGameMode.Data))
	{
		AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
		if (GameMode && IsValidForLoading(GameMode))
		{
			LoadActorFromBinary(GameMode, SavedGameMode);

			UE_LOG(LogGameSave, Log, TEXT("Game Mode loaded"));
		}
	}

	//Game State Actor
	if (!SaveLoadConstants::ArrayEmpty(SavedGameState.Data))
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		if (GameState && IsValidForLoading(GameState))
		{
			LoadActorFromBinary(GameState, SavedGameState);

			UE_LOG(LogGameSave, Log, TEXT("Game State loaded"));
		}
	}

	//Level Actors
	StartLoadLevelActors(LoadTask);
}

void UGameSaveSubsystem::StartLoadLevelActors(UAsyncLoadGame* LoadTask)
{
	if (!LoadTask)
	{
		return;
	}

	if (SaveLoadConstants::ArrayEmpty(SavedActors))
	{
		LoadTask->FinishLoading();
		return;
	}

	//If authority, we use distance based loading
	if (GetWorld()->GetNetMode() != ENetMode::NM_Client)
	{
		const APlayerController* PC = GetPlayerController();
		if (PC && PC->PlayerCameraManager)
		{
			const FVector CameraLoc = PC->PlayerCameraManager->GetCameraLocation();
			SavedActors.Sort([CameraLoc](const FActorSaveData& A, const FActorSaveData& B)
			{
				const float DistA = FVector::Dist(A.Transform.GetLocation(), CameraLoc);
				const float DistB = FVector::Dist(B.Transform.GetLocation(), CameraLoc);

				return DistA < DistB;
			});
		}
	}

	if (USaveLoadSettings::Get()->LoadMethod == ELoadMethod::LM_Thread)
	{
		if (FPlatformProcess::SupportsMultithreading())
		{
			AsyncTask(ENamedThreads::AnyNormalThreadNormalTask, [this, LoadTask]()
			{
				LoadAllLevelActors(LoadTask);
			});
		}
		else
		{
			LoadTask->StartDeferredLoad();
		}
	}
	else if (USaveLoadSettings::Get()->LoadMethod == ELoadMethod::LM_Deferred)
	{
		LoadTask->StartDeferredLoad();
	}
	else
	{
		LoadAllLevelActors(LoadTask);
	}
}

void UGameSaveSubsystem::LoadAllLevelActors(UAsyncLoadGame* LoadTask)
{
	bool bSuccess = false;

	for (const FActorSaveData& ActorArray : SavedActors)
	{
		bSuccess = SpawnOrUpdateLevelActor(ActorArray);
	}

	if (bSuccess)
	{
		LogFinishLoadingLevel();
	}

	if (!IsInGameThread())
	{
		//This fixes a potential crash
		const TWeakObjectPtr<UAsyncLoadGame> LoadTaskRef = MakeWeakObjectPtr(LoadTask);
		if (LoadTaskRef.IsValid())
		{
			AsyncTask(ENamedThreads::GameThread, [LoadTaskRef]()
			{
				UAsyncLoadGame* NewLoadTask = LoadTaskRef.Get();
				if (IsValid(NewLoadTask))
				{
					NewLoadTask->FinishLoading();
				}
			});
		}
	}
	else
	{
		LoadTask->FinishLoading();
	}
}

bool UGameSaveSubsystem::SpawnOrUpdateLevelActor(const FActorSaveData& ActorArray)
{
	const EActorType Type = EActorType(ActorArray.Type);
	const bool bRightType = Type == EActorType::AT_Placed || Type == EActorType::AT_Runtime || Type == EActorType::AT_Persistent;

	if (!bRightType)
	{
		return false;
	}

	const EUpdateActorResult UpdateResult = UpdateLevelActor(ActorArray);
	if (UpdateResult == EUpdateActorResult::RES_ShouldSpawnNewActor)
	{
		SpawnLevelActor(ActorArray);
	}

	return UpdateResult != EUpdateActorResult::RES_Skip;
}

EUpdateActorResult UGameSaveSubsystem::UpdateLevelActor(const FActorSaveData& ActorArray)
{
	FScopeLock Lock(&ActorCriticalSection);

	for (AActor* Actor : ActorList)
	{
		if (Actor && IsValidActor(Actor))
		{
			//Update existing actors
			if (ActorArray.Name == BytesFromString(GetFullActorName(Actor)))
			{
				//Skips respawn
				if (Actor->ActorHasTag(SaveLoadConstants::HasLoadedTag))
				{
					return EUpdateActorResult::RES_Skip;
				}

				if (!IsInGameThread())
				{
					AsyncTask(ENamedThreads::GameThread, [this, Actor, ActorArray]()
					{
						ProcessLevelActor(Actor, ActorArray);
						return EUpdateActorResult::RES_Success;
					});
				}
				else
				{
					ProcessLevelActor(Actor, ActorArray);
				}

				return EUpdateActorResult::RES_Success;
			}
		}
	}

	return EUpdateActorResult::RES_ShouldSpawnNewActor;
}

void UGameSaveSubsystem::SpawnLevelActor(const FActorSaveData& ActorArray)
{
	const FString Class = StringFromBytes(ActorArray.Class);
	if (Class.IsEmpty())
	{
		return;
	}

	UClass* SpawnClass = FindObject<UClass>(nullptr, *Class);

	//Wait for valid class when not running in the game thread 
	if (!SpawnClass)
	{
		FGraphEventRef GraphEvent;

		if (!IsInGameThread())
		{
			GraphEvent = FFunctionGraphTask::CreateAndDispatchWhenReady([this, &SpawnClass, Class]()
			{
				SpawnClass = StaticLoadSpawnClass(Class);

			}, TStatId(), nullptr, ENamedThreads::GameThread);
		}
		else
		{
			SpawnClass = StaticLoadSpawnClass(Class);
		}

		if (GraphEvent.IsValid())
		{
			GraphEvent.GetReference()->Wait();
		}
	}

	if (!SpawnClass)
	{
		const FString ActorName = StringFromBytes(ActorArray.Name);
		UE_LOG(LogGameSave, Warning, TEXT("Spawn Class '%s' could not be loaded for Actor: %s"), *Class, *ActorName);
		return;
	}

	if (SpawnClass && SpawnClass->ImplementsInterface(URPGActorSaveInterface::StaticClass()))
	{
		const FActorSpawnParameters SpawnParams = FSaveHelpers::GetSpawnParams(ActorArray.Name);

		if (!IsInGameThread())
		{
			AsyncTask(ENamedThreads::GameThread, [this, ActorArray, SpawnClass, SpawnParams]()
			{
				if (!CheckForExistingActor(ActorArray))
				{
					AActor* NewActor = GetWorld()->SpawnActor(SpawnClass, &ActorArray.Transform, SpawnParams);
					if (NewActor)
					{
						ProcessLevelActor(NewActor, ActorArray);
					}
					else
					{
						LogFailSpawnLevelActor(ActorArray);
					}
				}
			});
		}
		else
		{
			if (!CheckForExistingActor(ActorArray))
			{
				AActor* NewActor = GetWorld()->SpawnActor(SpawnClass, &ActorArray.Transform, SpawnParams);
				if (NewActor)
				{
					ProcessLevelActor(NewActor, ActorArray);
				}
				else
				{
					LogFailSpawnLevelActor(ActorArray);
				}
			}
		}
	}
}

void UGameSaveSubsystem::ProcessLevelActor(AActor* Actor, const FActorSaveData& ActorArray)
{
	//Only process matching type
	if (EActorType(ActorArray.Type) == GetActorType(Actor))
	{
		if (CanProcessActorTransform(Actor) && HasValidTransform(ActorArray.Transform))
		{
			Actor->SetActorTransform(ActorArray.Transform, false, nullptr, ETeleportType::TeleportPhysics);
		}

		LoadActorFromBinary(Actor, ActorArray.SaveData);
	}
}

void UGameSaveSubsystem::LogFailSpawnLevelActor(const FActorSaveData& ActorArray)
{
	const FString ActorName = StringFromBytes(ActorArray.Name);
	UE_LOG(LogGameSave, Warning, TEXT("Failed to spawn Level Actor: %s"), *ActorName);
}

void UGameSaveSubsystem::LogFinishLoadingLevel()
{
	UE_LOG(LogGameSave, Log, TEXT("Level Actors loaded"));
	ClearSavedLevelActors();
}

bool UGameSaveSubsystem::TryLoadPlayerFile()
{
	return LoadBinaryArchive(EDataLoadType::DATA_Player, PlayerSaveFile());
}

bool UGameSaveSubsystem::TryLoadLevelFile()
{
	return LoadBinaryArchive(EDataLoadType::DATA_Level, ActorSaveFile());
}

APlayerController* UGameSaveSubsystem::GetPlayerController() const
{
	return UGameplayStatics::GetPlayerController(GetWorld(), PlayerIndex);
}

APawn* UGameSaveSubsystem::GetPlayerPawn() const
{
	if (const APlayerController* PC = GetPlayerController())
	{
		return PC->GetPawnOrSpectator();
	}

	return nullptr;
}

FTimerManager& UGameSaveSubsystem::GetTimerManager() const
{
	return GetWorld()->GetTimerManager();
}

bool UGameSaveSubsystem::IsAsyncSaveOrLoadTaskActive(const ESaveGameMode& Mode, const EAsyncCheckType& CheckType,
	const bool& bLog) const
{
	//This will prevent the functions from being executed at all during pause
	if (GetWorld()->IsPaused())
	{
		if (bLog)
		{
			UE_LOG(LogGameSave, Warning, TEXT(" Async save or load called during pause. Operation was canceled."));
		}

		return true;
	}

	if (CheckType == EAsyncCheckType::CT_Both || CheckType == EAsyncCheckType::CT_Load)
	{
		for (TObjectIterator<UAsyncLoadGame> It; It; ++It)
		{
			if (CheckLoadIterator(It, Mode, bLog, "Load Game Actors"))
			{
				return true;
			}
		}
	}

	if (CheckType == EAsyncCheckType::CT_Both || CheckType == EAsyncCheckType::CT_Save)
	{
		for (TObjectIterator<UAsyncSaveGame> It; It; ++It)
		{
			if (CheckLoadIterator(It, Mode, bLog, "Save Game Actors"))
			{
				return true;
			}
		}
	}

	return false;
}

bool UGameSaveSubsystem::HasValidGameMode() const
{
	//On clients, we assume the game mode is valid
	if (GetWorld()->GetNetMode() == ENetMode::NM_Client)
	{
		return true;
	}

	const AGameModeBase* GameMode = GetWorld()->GetAuthGameMode();
	return IsValid(GameMode);
}

bool UGameSaveSubsystem::HasValidPlayer() const
{
	return IsValid(GetPlayerPawn());
}

bool UGameSaveSubsystem::VerifyOrCreateDirectory(const FString& NewDir)
{
	//Not required for console
	if (IsConsoleFileSystem())
	{
		return true;
	}

	const FString SaveFile = FPaths::Combine(BaseSaveDir(), NewDir);
	if (IFileManager::Get().DirectoryExists(*SaveFile) )
	{
		return true;
	}

	return IFileManager::Get().MakeDirectory(*SaveFile, true);
}

bool UGameSaveSubsystem::SaveBinaryArchive(FBufferArchive& BinaryData, const FString& FullSavePath)
{
	bool bSuccess = false;
	const bool bNoCompression = IsConsoleFileSystem();

	if (bNoCompression)
	{
		bSuccess = SaveBinaryData(BinaryData, FullSavePath);
	}
	else
	{
		//Compress and save
		TArray<uint8> CompressedData;
		FArchiveSaveCompressedProxy Compressor = FArchiveSaveCompressedProxy(CompressedData, GetCompressionMethod());

		if (Compressor.GetError())
		{
			UE_LOG(LogGameSave, Error, TEXT("Cannot save, compressor error: %s"), *FullSavePath);
			return false;
		}

		Compressor << BinaryData;
		Compressor.Flush();

		bSuccess = SaveBinaryData(CompressedData, FullSavePath);

		Compressor.FlushCache();
		Compressor.Close();
	}

	BinaryData.FlushCache();
	BinaryData.Empty();
	BinaryData.Close();

	return bSuccess;
}

bool UGameSaveSubsystem::LoadBinaryArchive(const EDataLoadType& LoadType, const FString& FullSavePath, UObject* Object)
{
	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	if (!SaveSystem->DoesSaveGameExist(*FullSavePath, PlayerIndex))
	{
		return false;
	}

	TArray<uint8> BinaryData;
	if (!SaveSystem->LoadGame(false, *FullSavePath, PlayerIndex, BinaryData))
	{
		UE_LOG(LogGameSave, Warning, TEXT("%s could not be loaded"), *FullSavePath);
		return false;
	}

	if (SaveLoadConstants::ArrayEmpty(BinaryData))
	{
		UE_LOG(LogGameSave, Warning, TEXT("No binary data found for %s"), *FullSavePath);
		return false;
	}

	bool bSuccess = false;
	const bool bNoCompression = IsConsoleFileSystem();

	if (bNoCompression)
	{
		FMemoryReader FromBinary = FMemoryReader(BinaryData, true);
		FromBinary.Seek(0);

		bSuccess = UnpackBinaryArchive(LoadType, FromBinary, Object);

		FromBinary.FlushCache();
		FromBinary.Close();
	}
	else
	{
		FArchiveLoadCompressedProxy Decompressor = FArchiveLoadCompressedProxy(BinaryData, GetCompressionMethod());

		if (Decompressor.GetError())
		{
			UE_LOG(LogGameSave, Error, TEXT("Cannot load, file might not be compressed: %s"), *FullSavePath);
			return false;
		}

		FBufferArchive DecompressedBinary;
		Decompressor << DecompressedBinary;

		FMemoryReader FromBinary = FMemoryReader(DecompressedBinary, true);
		FromBinary.Seek(0);

		//Unpack archive 
		bSuccess = UnpackBinaryArchive(LoadType, FromBinary, Object);

		Decompressor.FlushCache();
		Decompressor.Close();

		DecompressedBinary.Empty();
		DecompressedBinary.Close();

		FromBinary.FlushCache();
		FromBinary.Close();
	}

	return bSuccess;
}

bool UGameSaveSubsystem::UnpackBinaryArchive(const EDataLoadType& LoadType, FMemoryReader FromBinary, UObject* Object)
{
		if (LoadType == EDataLoadType::DATA_Level)
	{
		bool bLevelLoadSuccess = false;

		//Check for multi level saving.
		if (IsStackBasedMultiLevelSave())
		{
			FLevelStackArchive LevelStack;
			FromBinary << LevelStack;

			//Copy from disk to memory.
			if (SaveLoadConstants::ArrayEmpty(LevelArchiveList))
			{
				LevelArchiveList = LevelStack.Archives;
			}

			//It will only unpack the archive for the current level.
			for (const FLevelArchive& StackedArchive : LevelStack.Archives)
			{
				if (StackedArchive.Level == GetLevelName() /*|| bSupportPersistentActorsForMultiLevelSave*/)
				{
					if (IsFullMultiLevelSave())
					{
						UpdateMultiLevelStreamData(StackedArchive);
					}

					UnpackLevel(StackedArchive);
				}
			}

			SavedGameMode = LevelStack.SavedGameMode;
			SavedGameState = LevelStack.SavedGameState;

			bLevelLoadSuccess = true;
		}
		else
		{
			FLevelArchive LevelArchive;
			FromBinary << LevelArchive;

			if (IsStreamMultiLevelSave())
			{
				UpdateMultiLevelStreamData(LevelArchive);
			}

			if (UnpackLevel(LevelArchive))
			{
				bLevelLoadSuccess = true;
			}
		}

		return bLevelLoadSuccess;
	}
	else if (LoadType == EDataLoadType::DATA_Player)
	{
		//Check for multi level saving.
		if (IsStackBasedMultiLevelSave())
		{
			FPlayerStackArchive PlayerStack;
			FromBinary << PlayerStack;

			//Copy from disk to memory.
			if (PlayerStackData.IsEmpty())
			{
				PlayerStackData = PlayerStack;
			}
			
			UnpackPlayer(PlayerStack.PlayerArchive);

			//Set transform per level
			for (auto It = PlayerStack.LevelPositions.CreateConstIterator(); It; ++It)
			{
				if (It.Key() == GetLevelName())
				{
					DirectSetPlayerPosition(It.Value());
					return true;		
				}
			}

			//This prevents loading previous position for an unsaved level
			ClearPlayerPosition();

			return true;
		}
		else
		{
			FPlayerArchive PlayerArchive;
			FromBinary << PlayerArchive;

			if (PlayerArchive.Level == GetLevelName() || IsPersistentPlayer())
			{
				UnpackPlayer(PlayerArchive);
				return true;
			}
		}
	}
	else if (LoadType == EDataLoadType::DATA_Object)
	{
		if (Object)
		{
			FBufferArchive ObjectArchive;
			FromBinary << ObjectArchive;

			FMemoryReader MemoryReader(ObjectArchive, true);
			FObjectAndNameAsStringProxyArchive Ar(MemoryReader, true);
			Object->Serialize(Ar);

			ObjectArchive.FlushCache();
			ObjectArchive.Close();

			MemoryReader.FlushCache();
			MemoryReader.Close();

			return true;
		}
	}

	return false;
}

bool UGameSaveSubsystem::UnpackLevel(const FLevelArchive& LevelArchive)
{
	bool bLevelLoadSuccess = false;

	ClearSavedLevelActors();

	for (const FActorSaveData& TempSavedActor : LevelArchive.SavedActors)
	{
		if (EActorType(TempSavedActor.Type) == EActorType::AT_Persistent)
		{
			SavedActors.Add(TempSavedActor);
			bLevelLoadSuccess = true;
		}
		else
		{
			if (LevelArchive.Level == GetLevelName())
			{
				SavedActors.Add(TempSavedActor);
				bLevelLoadSuccess = true;
			}
		}
	}

	if (LevelArchive.Level == GetLevelName())
	{
		SavedScripts.Append(LevelArchive.SavedScripts);
		bLevelLoadSuccess = true;
	}

	//Basic Multi-Level saving saves mode in the level stack. It is always persistent.
	if (!IsStackBasedMultiLevelSave())
	{
		if (LevelArchive.Level == GetLevelName() || IsPersistentGameMode())
		{
			SavedGameMode = LevelArchive.SavedGameMode;
			SavedGameState = LevelArchive.SavedGameState;
			bLevelLoadSuccess = true;
		}
	}

	return bLevelLoadSuccess;
}

bool UGameSaveSubsystem::UnpackPlayer(const FPlayerArchive& PlayerArchive)
{
	SavedController = PlayerArchive.SavedController;
	SavedPawn = PlayerArchive.SavedPawn;
	SavedPlayerState = PlayerArchive.SavedPlayerState;

	return true;
}

void UGameSaveSubsystem::DirectSetPlayerPosition(const FPlayerPositionArchive& PosArchive)
{
	//Set location/rotation directly, no need to have the same player data per level.
	SavedPawn.Position = PosArchive.Position;
	SavedPawn.Rotation = PosArchive.Rotation;
	SavedController.Rotation = PosArchive.ControlRotation;
}

void UGameSaveSubsystem::ClearPlayerPosition()
{
	SavedPawn.Position = FVector::ZeroVector;
	SavedPawn.Rotation = FRotator::ZeroRotator;
	SavedController.Rotation = FRotator::ZeroRotator;
}

USaveGame* UGameSaveSubsystem::CreateSaveObject(const TSubclassOf<USaveGame>& SaveGameClass)
{
	if (*SaveGameClass && (*SaveGameClass != USaveGame::StaticClass()))
	{
		USaveGame* SaveGame = NewObject<USaveGame>(GetTransientPackage(), SaveGameClass);
		return SaveGame;
	}

	return nullptr;
}

bool UGameSaveSubsystem::SaveObject(const FString& FullSavePath, USaveGame* SaveGameObject)
{
	bool bSuccess = false;

	if (SaveGameObject)
	{
		TArray<uint8> Data;

		FMemoryWriter MemoryWriter(Data, true);
		FObjectAndNameAsStringProxyArchive Ar(MemoryWriter, false);
		SaveGameObject->Serialize(Ar);

		FBufferArchive Archive;
		Archive << Data;

		bSuccess = SaveBinaryArchive(Archive, *FullSavePath);

		Archive.FlushCache();
		Archive.Close();

		MemoryWriter.FlushCache();
		MemoryWriter.Close();
	}

	return bSuccess;
}

USaveGame* UGameSaveSubsystem::LoadObject(const FString& FullSavePath, TSubclassOf<USaveGame> SaveGameClass)
{
	USaveGame* SaveGameObject = CreateSaveObject(SaveGameClass);
	if (SaveGameObject)
	{
		LoadBinaryArchive(EDataLoadType::DATA_Object, FullSavePath, SaveGameObject);
	}

	return SaveGameObject;
}

FGameObjectSaveData UGameSaveSubsystem::ParseGameModeObjectForSaving(AActor* Actor, const bool& bWorldPartition)
{
	FGameObjectSaveData GameObjectActorData;

	if (Actor && IsValidForSaving(Actor))
	{
		//bWorldPartition - This will make sure that Mode/State are not loaded each time when adding a new World Partition Region
		SaveActorToBinary(Actor, GameObjectActorData, bWorldPartition);
	}

	return GameObjectActorData;
}

FActorSaveData UGameSaveSubsystem::ParseLevelActorForSaving(AActor* Actor, const EActorType& Type)
{
	FActorSaveData ActorArray;

	if (Type == EActorType::AT_Runtime || Type == EActorType::AT_Persistent)
	{
		ActorArray.Class = BytesFromString(Actor->GetClass()->GetPathName());
	}

	ActorArray.Type = uint8(Type);

	//No transform for persistent Actors or if skipped
	if (Type != EActorType::AT_Persistent && CanProcessActorTransform(Actor))
	{
		ActorArray.Transform = Actor->GetActorTransform();
	}
	else
	{
		ActorArray.Transform = FTransform::Identity;
	}

	ActorArray.Name = BytesFromString(GetFullActorName(Actor));

	SaveActorToBinary(Actor, ActorArray.SaveData);

	return ActorArray;
}

bool UGameSaveSubsystem::SaveBinaryData(const TArray<uint8>& SavedData, const FString& FullSavePath) const
{
	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	return SaveSystem->SaveGame(false, *FullSavePath, PlayerIndex, SavedData);
}

void UGameSaveSubsystem::SaveActorToBinary(AActor* Actor, FGameObjectSaveData& OutData, const bool& bKeepLoadedTag)
{
	IRPGActorSaveInterface::Execute_ActorPreSave(Actor);

	if (!bKeepLoadedTag)
	{
		Actor->Tags.Remove(SaveLoadConstants::HasLoadedTag);
	}

	SerializeToBinary(Actor, OutData.Data);

	if (GetActorType(Actor) != EActorType::AT_LevelScript)
	{
		SaveActorComponents(Actor, OutData.Components); 
	}

	IRPGActorSaveInterface::Execute_ActorSaved(Actor);
}

void UGameSaveSubsystem::LoadActorFromBinary(AActor* Actor, const FGameObjectSaveData& InData)
{
	const EActorType Type = GetActorType(Actor);

	//For runtime Actors we need to parse the structs separately
	if (USaveLoadSettings::Get()->bAutoSaveStructs)
	{
		if (Type == EActorType::AT_Runtime || Type == EActorType::AT_Persistent)
		{
			SerializeActorStructProperties(Actor);
		}
	}

	Actor->Tags.Add(SaveLoadConstants::HasLoadedTag);

	SerializeFromBinary(Actor, InData.Data);

	if (Type != EActorType::AT_LevelScript)
	{
		LoadActorComponents(Actor, InData.Components);
	}

	IRPGActorSaveInterface::Execute_ActorLoaded(Actor); //Post Component Load
}

void UGameSaveSubsystem::SerializeToBinary(UObject* Object, TArray<uint8>& OutData)
{
	FMemoryWriter MemoryWriter(OutData, true);
	FSaveGameArchive Ar(MemoryWriter);
	Object->Serialize(Ar);

	MemoryWriter.FlushCache();
	MemoryWriter.Close();
}

void UGameSaveSubsystem::SerializeFromBinary(UObject* Object, const TArray<uint8>& InData)
{
	FMemoryReader MemoryReader(InData, true);
	FSaveGameArchive Ar(MemoryReader);
	Object->Serialize(Ar);

	MemoryReader.FlushCache();
	MemoryReader.Close();
}

void UGameSaveSubsystem::SerializeActorStructProperties(AActor* Actor)
{
	SerializeStructProperties(Actor);

	//Also for Components
	const TArray<UActorComponent*> SourceComps = GetSaveComponents(Actor);

	if (!SaveLoadConstants::ArrayEmpty(SourceComps))
	{
		for (UActorComponent* Component : SourceComps)
		{
			if (Component)
			{
				SerializeStructProperties(Component);
			}
		}
	}
}

void UGameSaveSubsystem::SerializeStructProperties(UObject* Object)
{
	//Non-array struct vars.
	for (TFieldIterator<FStructProperty> ObjectStruct(Object->GetClass()); ObjectStruct; ++ObjectStruct)
	{
		if (ObjectStruct && ObjectStruct->GetPropertyFlags() & CPF_SaveGame)
		{
			SerializeScriptStruct(ObjectStruct->Struct);
		}
	}

	//Struct-Arrays are cast as Arrays, not structs, so we work around it.
	for (TFieldIterator<FArrayProperty> ArrayProp(Object->GetClass()); ArrayProp; ++ArrayProp)
	{
		if (ArrayProp && ArrayProp->GetPropertyFlags() & CPF_SaveGame)
		{
			SerializeArrayStruct(*ArrayProp);
		}
	}

	//Map Properties
	for (TFieldIterator<FMapProperty> MapProp(Object->GetClass()); MapProp; ++MapProp)
	{
		if (MapProp && MapProp->GetPropertyFlags() & CPF_SaveGame)
		{
			SerializeMap(*MapProp);
		}
	}
}

void UGameSaveSubsystem::SerializeScriptStruct(UStruct* ScriptStruct)
{
	if (ScriptStruct)
	{
		for (TFieldIterator<FProperty> Prop(ScriptStruct); Prop; ++Prop)
		{
			if (Prop)
			{
				Prop->SetPropertyFlags(CPF_SaveGame);

				//Recursive Array
				FArrayProperty* ArrayProp = CastField<FArrayProperty>(*Prop);
				if (ArrayProp)
				{
					SerializeArrayStruct(ArrayProp);
				}

				//Recursive Struct
				FStructProperty* StructProp = CastField<FStructProperty>(*Prop);
				if (StructProp)
				{
					SerializeScriptStruct(StructProp->Struct);
				}

				//Recursive Map
				FMapProperty* MapProp = CastField<FMapProperty>(*Prop);
				if (MapProp)
				{
					SerializeMap(MapProp);
				}
			}
		}
	}
}

void UGameSaveSubsystem::SerializeArrayStruct(FArrayProperty* ArrayProp)
{
	FProperty* InnerProperty = ArrayProp->Inner;
	if (InnerProperty)
	{
		//Here we finally get to the structproperty, wich hides in the Array->Inner
		FStructProperty* ArrayStructProp = CastField<FStructProperty>(InnerProperty);
		if (ArrayStructProp)
		{
			SerializeScriptStruct(ArrayStructProp->Struct);
		}
	}
}

void UGameSaveSubsystem::SerializeMap(FMapProperty* MapProp)
{
	FProperty* ValueProp = MapProp->ValueProp;

	if (ValueProp)
	{
		ValueProp->SetPropertyFlags(CPF_SaveGame);

		FStructProperty* ValueStructProp = CastField<FStructProperty>(ValueProp);
		if (ValueStructProp)
		{
			SerializeScriptStruct(ValueStructProp->Struct);
		}
	}
}

bool UGameSaveSubsystem::HasComponentSaveInterface(const UActorComponent* Comp) const
{
	return Comp && Comp->IsRegistered() && Comp->GetClass()->ImplementsInterface(URPGCompSaveInterface::StaticClass());
}

TArray<UActorComponent*> UGameSaveSubsystem::GetSaveComponents(AActor* Actor) const
{
	TArray<UActorComponent*> SourceComps;

	IRPGActorSaveInterface::Execute_ComponentsToSave(Actor, SourceComps);

	//Get Components with interface
	for (UActorComponent* Component : Actor->GetComponents())
	{
		if (HasComponentSaveInterface(Component))
		{
			if (SourceComps.Find(Component) == INDEX_NONE)
			{
				SourceComps.Add(Component);
			}
		}
	}

	return SourceComps;
}

void UGameSaveSubsystem::SaveActorComponents(AActor* Actor, TArray<FComponentSaveData>& OutComponents)
{
	const TArray<UActorComponent*> SourceComps = GetSaveComponents(Actor);
	if (SaveLoadConstants::ArrayEmpty(SourceComps))
	{
		return;
	}

	for (UActorComponent* Component : SourceComps)
	{
		if (Component && Component->IsRegistered())
		{
			FComponentSaveData ComponentArray;
			ComponentArray.Name = BytesFromString(Component->GetName());

			const USceneComponent* SceneComp = Cast<USceneComponent>(Component);
			if (SceneComp)
			{
				ComponentArray.RelativeTransform = SceneComp->GetRelativeTransform();
			}

			const UChildActorComponent* ChildActorComp = Cast<UChildActorComponent>(Component);
			if (ChildActorComp)
			{
				AActor* ChildActor = ChildActorComp->GetChildActor();
				if (ChildActor)
				{
					if (!HasSaveInterface(ChildActor))
					{
						SerializeToBinary(ChildActor, ComponentArray.Data);
					}
					else
					{
						UE_LOG(LogGameSave, Warning, TEXT("Child Actor Component has Save Interface, skipping: %s"), *Component->GetName());
					}
				}
			}
			else
			{
				if (HasComponentSaveInterface(Component))
				{
					IRPGCompSaveInterface::Execute_ComponentPreSave(Component);
				}

				SerializeToBinary(Component, ComponentArray.Data);
			}

			OutComponents.Add(ComponentArray);
		}
	}
}

void UGameSaveSubsystem::LoadActorComponents(AActor* Actor, const TArray<FComponentSaveData>& InComponents)
{
	const TArray<UActorComponent*> SourceComps = GetSaveComponents(Actor);
	if (SaveLoadConstants::ArrayEmpty(SourceComps))
	{
		return;
	}

	for (UActorComponent* Component : SourceComps)
	{
		if (Component && Component->IsRegistered())
		{
			for (const FComponentSaveData& ComponentArray : InComponents)
			{
				if (ComponentArray.Name == BytesFromString(Component->GetName()))
				{
					USceneComponent* SceneComp = Cast<USceneComponent>(Component);
					if (SceneComp)
					{
						if (IsMovable(SceneComp))
						{
							SceneComp->SetRelativeTransform(ComponentArray.RelativeTransform, false, nullptr, ETeleportType::TeleportPhysics);
						}
					}

					UChildActorComponent* ChildActorComp = Cast<UChildActorComponent>(Component);
					if (ChildActorComp)
					{
						AActor* ChildActor = ChildActorComp->GetChildActor();
						if (ChildActor)
						{
							if (!HasSaveInterface(ChildActor))
							{
								SerializeFromBinary(ChildActor, ComponentArray.Data);
							}
						}
					}
					else
					{
						SerializeFromBinary(Component, ComponentArray.Data);

						if (HasComponentSaveInterface(Component))
						{
							IRPGCompSaveInterface::Execute_ComponentLoaded(Component);
						}
					}
				}
			}
		}
	}
}

bool UGameSaveSubsystem::HasSaveArchiveError(const FBufferArchive& CheckArchive, const ESaveErrorType& ErrorType) const
{
	FString ErrorString = FString();
	if (ErrorType == ESaveErrorType::ER_Player)
	{
		ErrorString = "Player";
	}
	else if (ErrorType == ESaveErrorType::ER_Level)
	{
		ErrorString = "Level";
	}

	if (CheckArchive.IsCriticalError())
	{
		UE_LOG(LogGameSave, Error, TEXT("%s Data contains critical errors and was not saved."), *ErrorString);
		return true;
	}

	if (CheckArchive.IsError())
	{
		UE_LOG(LogGameSave, Error, TEXT("%s Data contains errors and was not saved."), *ErrorString);
		return true;
	}

	return false;
}

bool UGameSaveSubsystem::HasSaveInterface(const AActor* Actor) const
{
	return Actor->GetClass()->ImplementsInterface(URPGActorSaveInterface::StaticClass());
}

bool UGameSaveSubsystem::IsValidActor(const AActor* Actor) const
{
	return IsValid(Actor) && HasSaveInterface(Actor);
}

bool UGameSaveSubsystem::IsValidForSaving(const AActor* Actor) const
{
	return IsValidActor(Actor) && !Actor->ActorHasTag(SaveLoadConstants::SkipSaveTag);
}

bool UGameSaveSubsystem::IsValidForLoading(const AActor* Actor) const
{
	return IsValidActor(Actor) && !Actor->ActorHasTag(SaveLoadConstants::HasLoadedTag);
}

bool UGameSaveSubsystem::HasValidTransform(const FTransform& CheckTransform) const
{
	return CheckTransform.IsValid() && CheckTransform.GetLocation() != FVector::ZeroVector;
}

bool UGameSaveSubsystem::SaveAsLevelActor(const APawn* Pawn) const
{
	return Pawn->ActorHasTag(SaveLoadConstants::PlayerPawnAsLevelActorTag);
}

bool UGameSaveSubsystem::CheckForExistingActor(const FActorSaveData& ActorArray)
{
	if (!USaveLoadSettings::Get()->bAdvancedSpawnCheck)
	{
		return false;
	}

	const UWorld* ThisWorld = GetWorld();
	if (ThisWorld && ThisWorld->PersistentLevel)
	{
		const FName LoadedActorName(*StringFromBytes(ActorArray.Name));
		AActor* NewLevelActor = Cast<AActor>(StaticFindObjectFast(nullptr, GetWorld()->PersistentLevel, LoadedActorName));
		if (NewLevelActor)
		{
			if (!NewLevelActor->ActorHasTag(SaveLoadConstants::HasLoadedTag))
			{
				ProcessLevelActor(NewLevelActor, ActorArray);
				return true;
			}
		}
		else
		{
			LogFailSpawnLevelActor(ActorArray);
		}
	}

	return false;
}

EActorType UGameSaveSubsystem::GetActorType(const AActor* Actor) const
{
	if (IsValid(Actor))
	{
		//A real player controlled pawn is saved in the player file
		const APawn* Pawn = Cast<APawn>(Actor);
		if (IsValid(Pawn) && Pawn->IsPlayerControlled() && !SaveAsLevelActor(Pawn))
		{
			return EActorType::AT_Player;
		}

		if (Cast<APlayerController>(Actor) || Cast<APlayerState>(Actor))
		{
			return EActorType::AT_Player;
		}

		if (Cast<ALevelScriptActor>(Actor))
		{
			return EActorType::AT_LevelScript;
		}

		if (Cast<AGameModeBase>(Actor) || Cast<AGameStateBase>(Actor))
		{
			return EActorType::AT_GameObject;
		}

		if (Actor->ActorHasTag(SaveLoadConstants::PersistentTag))
		{
			return EActorType::AT_Persistent;
		}

		//Set to placed if the actor was already there on level/sub-level load. Just skips saving ClassName and therefore prevents respawn
		if (IsPlacedActor(Actor))
		{
			return EActorType::AT_Placed;
		}
	}

	return EActorType::AT_Runtime;
}

bool UGameSaveSubsystem::IsMovable(const USceneComponent* SceneComp) const
{
	if (SceneComp != nullptr)
	{
		return SceneComp->Mobility == EComponentMobility::Movable;
	}

	return false;
}

bool UGameSaveSubsystem::CanProcessActorTransform(const AActor* Actor) const
{
	return IsMovable(Actor->GetRootComponent()) && !Actor->ActorHasTag(SaveLoadConstants::SkipTransformTag) && Actor->GetAttachParentActor() == nullptr;
}

FName UGameSaveSubsystem::GetLevelName() const
{
	//Get full path without PIE prefixes

	FString LevelName = GetWorld()->GetOuter()->GetName();
	const FString Prefix = GetWorld()->StreamingLevelsPrefix;

	const int Index = LevelName.Find(Prefix);
	const int Count = Prefix.Len();

	LevelName.RemoveAt(Index, Count);

	return FName(*LevelName);
}

TArray<FString> UGameSaveSubsystem::GetAllSaveGames() const
{
	TArray<FString> SaveGameNames;

	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	SaveSystem->GetSaveGameNames(SaveGameNames, PlayerIndex);

	return SaveGameNames;
}

TArray<FString> UGameSaveSubsystem::GetSaveSlotsDesktop() const
{
	TArray<FString> SaveGameNames;
	IFileManager::Get().FindFiles(SaveGameNames, *FPaths::Combine(BaseSaveDir(), TEXT("*")), false, true);

	TArray<FSaveSlotInfo> SaveSlots;

	for (const FString& SlotName : SaveGameNames)
	{
		FSaveSlotInfo SlotInfo;
		SlotInfo.Name = SlotName;

		//Use Timestamp of actual file only for sorting.
		const FString SlotPath = SlotFilePath(SlotName);
		SlotInfo.TimeStamp = IFileManager::Get().GetTimeStamp(*SlotPath);

		SaveSlots.Add(SlotInfo);
	}

	SaveSlots.Sort([](const FSaveSlotInfo& A, const FSaveSlotInfo& B)
	{
		return A.TimeStamp > B.TimeStamp;
	});

	TArray<FString> SaveSlotNames;

	for (const FSaveSlotInfo& SlotInfo : SaveSlots)
	{
		SaveSlotNames.Add(SlotInfo.Name);
	}
	
	return SaveSlotNames;
}

TArray<FString> UGameSaveSubsystem::GetSaveSlotsConsole() const
{
	const FString SlotId = SaveLoadConstants::Underscore + SaveLoadConstants::SlotSuffix;
	const FString FullSlotSuffix = SlotId;

	//Gather all save files
	const TArray<FString> SaveGameNames = GetAllSaveGames();

	//Filter out slots
	TArray<FString> SlotNames;
	for (const FString& ActualFileName : SaveGameNames)
	{
		const bool bIsActualSlot = ActualFileName.Contains(FullSlotSuffix);
		if (bIsActualSlot)
		{
			//Get actual name without suffix
			const int32 Index = ActualFileName.Find(FullSlotSuffix, ESearchCase::IgnoreCase, ESearchDir::FromEnd, INDEX_NONE);
			const int32 Count = FullSlotSuffix.Len();

			FString ReducedFileName = ActualFileName;
			ReducedFileName.RemoveAt(Index, Count);
			SlotNames.Add(ReducedFileName);
		}
	}

	//Fill with proper info
	TArray<FSaveSlotInfo> SaveSlots;
	for (const FString& SlotName : SlotNames)
	{
		FSaveSlotInfo SlotInfo;
		SlotInfo.Name = SlotName;

		const FString SlotPath = SlotFilePath(SlotName);
		SlotInfo.TimeStamp = IFileManager::Get().GetTimeStamp(*SlotPath);

		SaveSlots.Add(SlotInfo);
	}

	SaveSlots.Sort([](const FSaveSlotInfo& A, const FSaveSlotInfo& B)
	{
		return A.TimeStamp > B.TimeStamp;
	});

	//Need another copy to return the sorted list
	TArray<FString> SaveSlotNames;
	for (const FSaveSlotInfo& SlotInfo : SaveSlots)
	{
		SaveSlotNames.Add(SlotInfo.Name);
	}

	return SaveSlotNames;
}

FLevelStackArchive UGameSaveSubsystem::AddMultiLevelStackData(const FLevelArchive& LevelArchive,
	const FGameObjectSaveData& InGameMode, const FGameObjectSaveData& InGameState)
{
	//Create a new Stack
	FLevelStackArchive LevelStack;
	{
		LevelStack.AddTo(LevelArchive);
		LevelStack.SavedGameMode = InGameMode;
		LevelStack.SavedGameState = InGameState;
	}

	//Add data from memory to the Stack Archive.
	for (const FLevelArchive& MemoryArchive : LevelArchiveList)
	{
		if (MemoryArchive != LevelArchive)
		{
			LevelStack.AddTo(MemoryArchive);
		}
	}

	//Update the list in memory with data from current level.
	if (LevelArchiveList.Contains(LevelArchive))
	{
		//If already in the array, we just replace the data.
		for (FLevelArchive& ExistingArchive : LevelArchiveList)
		{
			if (ExistingArchive == LevelArchive)
			{
				ExistingArchive.ReplaceWith(LevelArchive);
			}
		}
	}
	else
	{
		LevelArchiveList.Add(LevelArchive);
	}

	return LevelStack;
}


FLevelArchive UGameSaveSubsystem::AddMultiLevelStreamData(const FLevelArchive& LevelArchive)
{
	//Get the data from memory and either replace or add current Level Actors.
	FMultiLevelStreamingData NewStreamingData = MultiLevelStreamData;
	{
		NewStreamingData.PruneActors();
		NewStreamingData.ReplaceOrAdd(LevelArchive);
	}

	//Add Actors and Scripts from StreamData.
	FLevelArchive NewLevelArchive = LevelArchive;
	{
		NewLevelArchive.SavedActors = NewStreamingData.Actors;
		NewLevelArchive.SavedScripts = NewStreamingData.Scripts;
	}

	UpdateMultiLevelStreamData(NewLevelArchive);

	return NewLevelArchive;
}

void UGameSaveSubsystem::UpdateMultiLevelStreamData(const FLevelArchive& LevelArchive)
{
	//Update data in memory
	MultiLevelStreamData.CopyFrom(LevelArchive);
}

bool UGameSaveSubsystem::HasRenderTargetResource(UTextureRenderTarget2D* TextureRenderTarget) const
{
	return TextureRenderTarget->GetResource() != nullptr;
}

bool UGameSaveSubsystem::CompressRenderTarget(UTextureRenderTarget2D* TexRT, FArchive& Ar)
{
	FImage Image;
	if (!FImageUtils::GetRenderTargetImage(TexRT, Image))
	{
		return false;
	}

	TArray64<uint8> CompressedData;
	if (!FImageUtils::CompressImage(CompressedData, *GetThumbnailFormat(), Image, 90))
	{
		return false;
	}

	Ar.Serialize((void*)CompressedData.GetData(), CompressedData.GetAllocatedSize());

	return true;
}

bool UGameSaveSubsystem::ExportRenderTarget(UTextureRenderTarget2D* TexRT, const FString& FileName)
{
	FArchive* Ar = IFileManager::Get().CreateFileWriter(*FileName);
	if (Ar)
	{
		FBufferArchive Buffer;
		if (CompressRenderTarget(TexRT, Buffer))
		{
			Ar->Serialize(const_cast<uint8*>(Buffer.GetData()), Buffer.Num());
			delete Ar;

			return true;
		}
	}
	
	return false;
}

void UGameSaveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	UE_LOG(LogGameSave, Log, TEXT("SaveLoad Initialized"));
	UE_LOG(LogGameSave, Log, TEXT("Current Save Game Slot is: %s"), *GetCurrentSaveGameName());

// #if RPG_ENGINE_MIN_UE51
// 	if (CanSupportWorldPartition())
// 	{
// 		FLevelStreamingDelegates::OnLevelStreamingTargetStateChanged.AddUObject(this, &UEMSObject::OnLevelStreamingTargetStateChanged);
// 		FLevelStreamingDelegates::OnLevelBeginMakingInvisible.AddUObject(this, &UEMSObject::OnLevelBeginMakingInvisible);
// 	}
// #endif
}

void UGameSaveSubsystem::Deinitialize()
{
// #if RPG_ENGINE_MIN_UE51
// 	FLevelStreamingDelegates::OnLevelStreamingTargetStateChanged.RemoveAll(this);
// 	FLevelStreamingDelegates::OnLevelBeginMakingInvisible.RemoveAll(this);
// #endif
}

UWorld* UGameSaveSubsystem::GetWorld() const
{
	return GetGameInstance()->GetWorld();
}

UCustomSaveGame* UGameSaveSubsystem::GetCustomSave(const TSubclassOf<UCustomSaveGame>& SaveGameClass)
{
	if (!SaveGameClass->GetDefaultObject())
	{
		return nullptr;
	}
	
	const UCustomSaveGame* CustomClass = Cast<UCustomSaveGame>(SaveGameClass->GetDefaultObject());
	if (!CustomClass)
	{
		return nullptr;
	}
	
	//Actual name of the custom save file
	FString CustomSaveName = CustomClass->SaveGameName;
	if (CustomSaveName.IsEmpty())
	{
		CustomSaveName = CustomClass->GetName();
	}
	
	const bool bUseSlot = CustomClass->bUseSaveSlot;
	
	const FString SaveFile = CustomSaveFile(CustomSaveName, bUseSlot);
	const FSoftClassPath SaveClass = CustomClass->GetClass();
	
	const FString CurrentSave = GetCurrentSaveGameName();
	const FString CachedRefName = bUseSlot ? CustomSaveName + CurrentSave : CustomSaveName;
	
	UCustomSaveGame* CachedObject = CachedCustomSaves.FindRef(CachedRefName);
	if (CachedObject)
	{
		return GetDesiredSaveObject<UCustomSaveGame>(SaveFile, SaveClass, CachedObject);
	}
	else
	{
		UCustomSaveGame* NewObject = GetDesiredSaveObject<UCustomSaveGame>(SaveFile, SaveClass, CachedObject);
		NewObject->SaveGameName = CustomSaveName;
	
		CachedCustomSaves.Add(CachedRefName, NewObject);
		
		return NewObject;
	}

	return nullptr;
}


