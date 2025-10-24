// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class RPGPROJECT_API QuestFilesManager
{
public:
	QuestFilesManager();
	~QuestFilesManager();

	static void SaveLastQuestID(int identificatorP, const FString& ValueName);
	static int LoadLastQuestID(const FString& ValueName);
};
