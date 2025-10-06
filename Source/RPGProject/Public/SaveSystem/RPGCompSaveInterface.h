// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RPGCompSaveInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URPGCompSaveInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGPROJECT_API IRPGCompSaveInterface
{
	GENERATED_BODY()

public:

	/**Executed after the Component has been loaded.*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Easy Multi Save")
	void ComponentLoaded();

	/**Executed before the Component is saved.*/
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Easy Multi Save")
	void ComponentPreSave();
};
