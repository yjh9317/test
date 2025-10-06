// Fill out your copyright notice in the Description page of Project Settings.


#include "SaveSystem/SaveLoadBlueprintLibrary.h"

#include "SaveSystem/GameSaveSubsystem.h"
#include "SaveSystem/InfoSaveGame.h"

/**
Save Game Users
**/

void USaveLoadBlueprintLibrary::SetCurrentSaveUserName(UObject* WorldContextObject, const FString& UserName)
{
	if (UGameSaveSubsystem* SaveLoadObject = UGameSaveSubsystem::Get(WorldContextObject))
	{
		SaveLoadObject->SetCurrentSaveUserName(UserName);
	}
}

void USaveLoadBlueprintLibrary::DeleteSaveUser(UObject* WorldContextObject, const FString& UserName)
{
	if (UGameSaveSubsystem* SaveLoadObject = UGameSaveSubsystem::Get(WorldContextObject))
	{
		SaveLoadObject->DeleteAllSaveDataForUser(UserName);
	}
}

TArray<FString> USaveLoadBlueprintLibrary::GetAllSaveUsers(UObject* WorldContextObject)
{
	if (UGameSaveSubsystem* SaveLoadObject = UGameSaveSubsystem::Get(WorldContextObject))
	{
		return SaveLoadObject->GetAllSaveUsers();
	}

	return TArray<FString>();
}

/**
Save Slots
**/

void USaveLoadBlueprintLibrary::SetCurrentSaveGameName(UObject* WorldContextObject, const FString & SaveGameName)
{
	if (UGameSaveSubsystem* SaveLoadObject = UGameSaveSubsystem::Get(WorldContextObject))
	{
		SaveLoadObject->SetCurrentSaveGameName(SaveGameName);
	}
}

TArray<FString> USaveLoadBlueprintLibrary::GetSortedSaveSlots(UObject* WorldContextObject)
{
	if (UGameSaveSubsystem* SaveLoadObject = UGameSaveSubsystem::Get(WorldContextObject))
	{
		return SaveLoadObject->GetSortedSaveSlots();
	}

	return TArray<FString>();
}

UInfoSaveGame* USaveLoadBlueprintLibrary::GetSlotInfoSaveGame(UObject* WorldContextObject, FString& SaveGameName)
{
	if (UGameSaveSubsystem* SaveLoadObject = UGameSaveSubsystem::Get(WorldContextObject))
	{
		SaveGameName = SaveLoadObject->GetCurrentSaveGameName();
		return SaveLoadObject->GetSlotInfoObject(SaveGameName);
	}

	return nullptr;
}

UInfoSaveGame* USaveLoadBlueprintLibrary::GetNamedSlotInfo(UObject* WorldContextObject, const FString& SaveGameName)
{
	if (UGameSaveSubsystem* SaveLoadObject = UGameSaveSubsystem::Get(WorldContextObject))
	{
		return SaveLoadObject->GetSlotInfoObject(SaveGameName);
	}

	return nullptr;
}

bool USaveLoadBlueprintLibrary::DoesSaveSlotExist(UObject* WorldContextObject, const FString& SaveGameName)
{
	if (SaveGameName.IsEmpty())
	{
		return false;
	}

	if (UGameSaveSubsystem* SaveLoadObject = UGameSaveSubsystem::Get(WorldContextObject))
	{
		if (SaveLoadObject->DoesSaveGameExist(SaveGameName))
		{
			return true;
		}
		else
		{
			if (SaveLoadObject->GetSortedSaveSlots().Contains(SaveGameName))
			{
				return true;
			}
		}
	}

	return false;
}

/**
File System
**/

void USaveLoadBlueprintLibrary::DeleteAllSaveDataForSlot(UObject* WorldContextObject, const FString& SaveGameName)
{
	if (UGameSaveSubsystem* SaveLoadObject = UGameSaveSubsystem::Get(WorldContextObject))
	{
		SaveLoadObject->DeleteAllSaveDataForSlot(SaveGameName);
	}
}

void USaveLoadBlueprintLibrary::ClearMultiLevelSave(UObject* WorldContextObject)
{
	if (UGameSaveSubsystem* SaveLoadObject = UGameSaveSubsystem::Get(WorldContextObject))
	{
		// SaveLoadObject->ClearMultiLevelSave();
	}
}

/**
Thumbnail Saving
Simple saving as .png from a 2d scene capture render target source.
**/

UTexture2D* USaveLoadBlueprintLibrary::ImportSaveThumbnail(UObject* WorldContextObject, const FString& SaveGameName)
{
	if (UGameSaveSubsystem* SaveLoadObject = UGameSaveSubsystem::Get(WorldContextObject))
	{
		return SaveLoadObject->ImportSaveThumbnail(SaveGameName);
	}	

	return nullptr;
}

void USaveLoadBlueprintLibrary::ExportSaveThumbnail(UObject* WorldContextObject, UTextureRenderTarget2D* TextureRenderTarget, const FString& SaveGameName)
{
	if (UGameSaveSubsystem* SaveLoadObject = UGameSaveSubsystem::Get(WorldContextObject))
	{
		return SaveLoadObject->ExportSaveThumbnail(TextureRenderTarget, SaveGameName);
	}
}

/**
Other Functions
**/

void USaveLoadBlueprintLibrary::SetActorSaveProperties(UObject* WorldContextObject, bool bSkipSave,  bool bPersistent, bool bSkipTransform)
{
	AActor* SaveActor = Cast<AActor>(WorldContextObject);
	if (SaveActor)
	{
		// if (bSkipSave)
		// {
		// 	SaveActor->Tags.AddUnique(SaveLoadObject::SkipSaveTag);
		// }
		// else
		// {
		// 	SaveActor->Tags.Remove(SaveLoadObject::SkipSaveTag);
		// }
		//
		// if (bPersistent)
		// {
		// 	SaveActor->Tags.AddUnique(SaveLoadObject::PersistentTag);
		// }
		// else
		// {
		// 	SaveActor->Tags.Remove(SaveLoadObject::PersistentTag);
		// }
		//
		// if (bSkipTransform)
		// {
		// 	SaveActor->Tags.AddUnique(SaveLoadObject::SkipTransformTag);
		// }
		// else
		// {
		// 	SaveActor->Tags.Remove(SaveLoadObject::SkipTransformTag);
		// }
	}
}

bool USaveLoadBlueprintLibrary::IsSavingOrLoading(UObject* WorldContextObject)
{
	if (UGameSaveSubsystem* SaveLoadObject = UGameSaveSubsystem::Get(WorldContextObject))
	{
		return SaveLoadObject->IsAsyncSaveOrLoadTaskActive(ESaveGameMode::MODE_All, EAsyncCheckType::CT_Both, false);
	}

	return false;
}

/**
Custom Objects
**/

bool USaveLoadBlueprintLibrary::SaveCustom(UObject* WorldContextObject)
{
	if (UGameSaveSubsystem* SaveLoadObject = UGameSaveSubsystem::Get(WorldContextObject))
	{
		return SaveLoadObject->SaveAllCustomObjects();
	}

	return false;
}

UCustomSaveGame* USaveLoadBlueprintLibrary::GetCustomSave(UObject* WorldContextObject, TSubclassOf<UCustomSaveGame> SaveGameClass)
{
	if (SaveGameClass->GetDefaultObject())
	{
		if (UGameSaveSubsystem* SaveLoadObject = UGameSaveSubsystem::Get(WorldContextObject))
		{
			return SaveLoadObject->GetCustomSave(SaveGameClass);
		}
	}

	return nullptr;
}

void USaveLoadBlueprintLibrary::DeleteCustomSave(UObject* WorldContextObject, UCustomSaveGame* SaveGame)
{
	if (UGameSaveSubsystem* SaveLoadObject = UGameSaveSubsystem::Get(WorldContextObject))
	{
		SaveLoadObject->DeleteCustomSave(SaveGame);
	}
}

bool USaveLoadBlueprintLibrary::IsWorldPartition(UObject* WorldContextObject)
{
	if (UGameSaveSubsystem* SaveLoadObject = UGameSaveSubsystem::Get(WorldContextObject))
	{
		return SaveLoadObject->CanSupportWorldPartition();
	}

	return false;
}
