// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Quest/RPGQuest.h"
#include "QuestListenerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQuestStateChangedListenerDelegate, URPGQuest*, questUpdate, EQuestState, QuestState);

UCLASS(Blueprintable, meta = (ABSTRACT))
class RPGPROJECT_API UQuestListenerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UQuestListenerComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Quest")
	TArray<int> QuestToFollow;

protected:
	virtual void BeginPlay() override;

	void ListenToQuests();

	/* References */
	UPROPERTY(BlueprintReadOnly, Category = "Advanced Quest | Quest")
	class UQuestManager* QuestManager = nullptr;

	UFUNCTION(Category = "Advanced Quest | Events")
	void OnQuestStateChangedWrapper(URPGQuest* questUpdate, EQuestState QuestState);

	// Use this delegate to be able to update actors in blueprint
	// when OnQuestStateChanged is triggered
	UPROPERTY(BlueprintAssignable, Category = "Advanced Quest | Events")
	FOnQuestStateChangedListenerDelegate OnQuestStateChanged;

	UFUNCTION(BlueprintCallable, Category = "Advanced Quest | Events")
	void BindFunctionsToQuestDelegates(URPGQuest* quest);

		
};
