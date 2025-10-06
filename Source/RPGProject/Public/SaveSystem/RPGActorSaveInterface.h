// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RPGActorSaveInterface.generated.h"


UINTERFACE(Category = "SaveLoad", BlueprintType, meta = (DisplayName = "Actor Save Interface"))
class URPGActorSaveInterface : public UInterface
{
	GENERATED_BODY()
};


class RPGPROJECT_API IRPGActorSaveInterface
{
	GENERATED_BODY()
	
public:
	/**Executed after the Actor and all of it's components have been loaded.*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Easy Multi Save")
	void ActorLoaded();

	/**Executed when the Actor and all of it's components have been saved.*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Easy Multi Save")
	void ActorSaved();

	/**Executed before the Actor and all of it's components are saved.*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Easy Multi Save")
	void ActorPreSave();

	/**
	* (Not Level Blueprints) Holds the array of components that you want to save for the Actor.
	* This function is not relevant for Level Blueprints, as they cannot have components.
	*
	* @param Components - The Components that you want to save with the Actor.
	*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Easy Multi Save")
	void ComponentsToSave(TArray<UActorComponent*>& Components);
};
