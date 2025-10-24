// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RPG_PawnInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class URPG_PawnInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGPROJECT_API IRPG_PawnInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Get the value of the camera rotation input (usually the right thumbstick or the mouse).
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "RPG Camera Interface")
	FRotator GetRotationInput() const;

	// Get the value of the movement input (usually WASD or the left thumbstick).
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "RPG Camera Interface")
	FVector GetMovementInput() const;
};

UINTERFACE(MinimalAPI)
class URPG_PawnMovementInterface : public UInterface
{
	GENERATED_BODY()
};

class RPGPROJECT_API IRPG_PawnMovementInterface
{
	GENERATED_BODY()

	public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "RPG|Movement Interface")
	FVector GetOwnerVelocity() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "RPG|Movement Interface")
	bool IsOwnerFalling() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "RPG|Movement Interface")
	bool IsOwnerStrafing() const;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "RPG|Movement Interface")
	bool IsOwnerMovingOnGround() const;
};