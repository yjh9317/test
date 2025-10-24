// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "QuestFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UQuestFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Quest")
	static int AssignUniqueID(const FString& valueName);
	
	UFUNCTION(BlueprintCallable, Category = "Quest | Component")
	static UActorComponent* AddComponent(TSubclassOf<class UActorComponent> ComponentClass, AActor* Actor, USceneComponent* ParentComponent, FName Name);

	UFUNCTION(BlueprintCallable, Category = "Quest | Component")
	static void RemoveComponent(UActorComponent* ActorComponent, AActor* Actors);

	UFUNCTION(BlueprintCallable, Category = "Quest | Debug")
	static void ShowFormattedDialog(const FString& InFileName);

	UFUNCTION(BlueprintCallable, Category = "Quest | Debug")
	static void UpdateActor(AActor* Actor);
	
	UFUNCTION(BlueprintCallable, Category = "Quest | Debug")
	static void SavePackage(UObject* Object);
};
