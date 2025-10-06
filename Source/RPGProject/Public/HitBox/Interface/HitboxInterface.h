// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "HitboxInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHitboxInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGPROJECT_API IHitboxInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	void OnHitAsVictim(float Damage,AActor* Attacker,AController* Instigator,FHitResult HitResult,UDamageType* DamageType,FGameplayTag GameplayTag);
	void OnHitAsAttacker(AActor* Victim,FHitResult HitResult,UDamageType* DamageType,FGameplayTag GameplayTag);
};
