// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveSystem/AsyncLoadGame.h"
#include "SaveSystem/GameSaveSubsystem.h"


UAsyncLoadGame* UAsyncLoadGame::AsyncLoadActors(UObject* WorldContextObject, int32 Data, bool bFullReload)
{
	return nullptr;
}

bool UAsyncLoadGame::NativeSpawnLoadTask(UGameSaveSubsystem* Object, const int32& Data, const bool& bFullReload)
{
	return false;
}

bool UAsyncLoadGame::NativeLoadLevelActors(UGameSaveSubsystem* Object)
{
	return false;
}

void UAsyncLoadGame::Activate()
{
	Super::Activate();
}

void UAsyncLoadGame::FinishLoading()
{
}

void UAsyncLoadGame::StartDeferredLoad()
{
}

void UAsyncLoadGame::PreLoading()
{
}

void UAsyncLoadGame::StartLoading()
{
}

void UAsyncLoadGame::LoadPlayer()
{
}

void UAsyncLoadGame::LoadLevel()
{
}

void UAsyncLoadGame::CompleteLoadingTask()
{
}

void UAsyncLoadGame::FailLoadingTask()
{
}

void UAsyncLoadGame::DeferredLoadActors()
{
}

void UAsyncLoadGame::ClearFailTimer()
{
}
