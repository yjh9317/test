// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/External/QuestFilesManager.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"

QuestFilesManager::QuestFilesManager()
{
}

QuestFilesManager::~QuestFilesManager()
{
}


void QuestFilesManager::SaveLastQuestID(int identificatorP, const FString& ValueName)
{
	FString FilePath = FPaths::ProjectPluginsDir() / TEXT("AdvancedQuest") / TEXT("Resources") / TEXT("UniqueID.txt");

	// Create a formatted string with the key-value pairs
	FString SaveString = ValueName + FString::Printf(TEXT(": %d"), identificatorP);

	// Load the existing content of the file
	FString ExistingContent;

	if (FFileHelper::LoadFileToString(ExistingContent, *FilePath))
	{
		// Split the file content into lines
		TArray<FString> Lines;
		ExistingContent.ParseIntoArrayLines(Lines);

		// Find and replace the lines with the new values
		for (int32 i = 0; i < Lines.Num(); ++i)
		{
			if (Lines[i].StartsWith(ValueName))
			{
				Lines[i] = ValueName + FString::Printf(TEXT(": %d"), identificatorP);
			}
		}

		// Join the lines back into a single string
		SaveString = FString::Join(Lines, TEXT("\n"));
	}

	if (FFileHelper::SaveStringToFile(SaveString, *FilePath))
	{
		// Succeed to save to file
	};
}

int QuestFilesManager::LoadLastQuestID(const FString& ValueName)
{
	FString FilePath = FPaths::ProjectPluginsDir() / TEXT("AdvancedQuest") / TEXT("Resources") / TEXT("UniqueID.txt");

	int identificator = 0;

	FString QuestIDString;
	if (FFileHelper::LoadFileToString(QuestIDString, *FilePath))
	{
		// Split the file content into lines
		TArray<FString> Lines;
		QuestIDString.ParseIntoArrayLines(Lines);

		// Iterate through each line to find the values
		for (const FString& Line : Lines)
		{
			TArray<FString> KeyValues;
			Line.ParseIntoArray(KeyValues, TEXT(":"));

			if (KeyValues.Num() == 2)
			{
				// Trim any whitespace from the key and value
				FString Key = KeyValues[0].TrimStartAndEnd();
				FString Value = KeyValues[1].TrimStartAndEnd();

				// Check the key and parse the corresponding value
				if (Key.Equals(ValueName, ESearchCase::IgnoreCase))
				{
					identificator = FCString::Atoi(*Value);
				}
			}
		}
	}

	return identificator;
}
