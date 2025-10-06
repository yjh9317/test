// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveSystem/AsyncSaveGame.h"
#include "SaveSystem/SaveLoadSettings.h"
#include "Async/Async.h"
#include "SaveSystem/GameSaveSubsystem.h"
#include "SaveSystem/SaveLoadData.h"

UAsyncSaveGame* UAsyncSaveGame::AsyncSaveActors(UObject* WorldContextObject, int32 Data)
{
	if (UGameSaveSubsystem* Object = UGameSaveSubsystem::Get(WorldContextObject))
	{
		if (!Object->IsAsyncSaveOrLoadTaskActive(GetMode(Data)))
		{
			UAsyncSaveGame* SaveTask = NewObject<UAsyncSaveGame>(GetTransientPackage());
			SaveTask->WorldContext = WorldContextObject;
			SaveTask->Data = Data;
			SaveTask->Mode = GetMode(Data);
			SaveTask->SaveLoadObject = Object;

			return SaveTask;
		}
	}

	return nullptr;
}

bool UAsyncSaveGame::NativeSpawnSaveTask(UGameSaveSubsystem* Object, const int32& Data)
{
	UAsyncSaveGame* SaveTask = NewObject<UAsyncSaveGame>(GetTransientPackage());
	if (SaveTask)
	{
		SaveTask->WorldContext = Object;
		SaveTask->Data = Data;
		SaveTask->Mode = GetMode(Data);
		SaveTask->SaveLoadObject = Object;
		SaveTask->Activate();

		return true;
	}

	return false;
}

bool UAsyncSaveGame::NativeSaveLevelActors(UGameSaveSubsystem* Object)
{
	return NativeSpawnSaveTask(Object, ENUM_TO_FLAG(ESaveTypeFlags::SF_Level));
}

void UAsyncSaveGame::Activate()
{
	bIsActive = true;

	if (SaveLoadObject)
	{
		SaveLoadObject->PrepareLoadAndSaveActors(Data, EAsyncCheckType::CT_Save, false);

		SaveLoadObject->GetTimerManager().SetTimerForNextTick(this, &UAsyncSaveGame::StartSaving);
	}
}

void UAsyncSaveGame::SavePlayer()
{
	bFinishedStep = false;

	if (SaveLoadObject)
	{
		if (USaveLoadSettings::Get()->bMultiThreadSaving && FPlatformProcess::SupportsMultithreading())
		{
			AsyncTask(ENamedThreads::AnyNormalThreadNormalTask, [this]()
			{
				InternalSavePlayer();
			});
		}
		else
		{
			InternalSavePlayer();
		}

		TryMoveToNextStep(ENextStepType::SaveLevel);
	}
}

void UAsyncSaveGame::StartSaving()
{
}


void UAsyncSaveGame::InternalSavePlayer()
{
}

void UAsyncSaveGame::SaveLevel()
{
}

void UAsyncSaveGame::InternalSaveLevel()
{
}

void UAsyncSaveGame::FinishSaving()
{
}

void UAsyncSaveGame::CompleteSavingTask()
{
}

void UAsyncSaveGame::TryMoveToNextStep(ENextStepType Step)
{
}

ESaveGameMode UAsyncSaveGame::GetMode(int32 Data)
{
	return ESaveGameMode::MODE_All;
}

