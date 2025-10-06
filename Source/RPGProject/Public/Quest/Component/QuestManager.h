// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "QuestManager.generated.h"


class URPGQuest;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UQuestManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UQuestManager();

protected:
	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	void LateBeginPlay();

public:
	UFUNCTION(BlueprintCallable, Category = "Advanced Quest | Quest")
	URPGQuest* QueryQuest(int QuestID);

// protected:
// 	UPROPERTY(BlueprintReadOnly, Category = "Advanced Quest | Quest")
// 	TMap<int, UAQ_Quest*> QuestDataCenter;
//
//
// 	/* Delegates */
// 	UPROPERTY(BlueprintCallable, Category = "Advanced Quest | Date")
// 	FOnNewDayDelegate OnNewDayDelegate;
//
// 	UPROPERTY(BlueprintCallable, Category = "Advanced Quest | Date")
// 	FOnNewWeekDelegate OnNewWeekDelegate;
protected:
	UPROPERTY(BlueprintReadOnly, Category = "Advanced Quest | Quest")
	TMap<int, URPGQuest*> QuestDataCenter;

public:
	// UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Advanced Quest | Save-Load")
	// void LoadQuestData();
	//
	// UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Advanced Quest | Save-Load")
	// void SaveQuestData();

};
