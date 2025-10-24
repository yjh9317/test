// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "RPG_IFocusTargetMethod.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API URPG_IFocusTargetMethod : public UObject
{
	GENERATED_BODY()
public:
	/*
	 * Get the location of the target we want the camera to look at.
	 * @param   Owner               The owner of the camera.
	 * @param   OwnerLocation       The world location of the owner of camera.
	 * @param   ViewPointLocation   Camera's location.
	 * @param   ViewPointRotation   Camera's rotation.
	 */
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG Camera Focus")
	AActor* GetTargetLocation(class AActor* InOwner, FVector OwnerLocation, FVector ViewPointLocation, FRotator ViewPointRotation, FVector& OutTargetLocation);

private:
	/** Getter for the cached world pointer, will return null if the actor is not actually spawned in a level */
	virtual UWorld* GetWorld() const override;
};
