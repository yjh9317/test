// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbstractEquippable.generated.h"

UCLASS()
class RPGPROJECT_API AAbstractEquippable : public AActor
{
	GENERATED_BODY()
	
public:
	AAbstractEquippable();

	virtual void AttachToNewSocket(FName NewSocket) {};
	virtual void ToggleCombatMode(bool CombatMode) {};
};
