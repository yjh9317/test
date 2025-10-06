// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Action/Actions/InstantAction.h"
#include "ProjectileAction.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UProjectileAction : public UInstantAction
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	TSubclassOf<class AActor> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	float ProjectileSpeed = 1000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	FName SpawnSocketName = "Muzzle";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Projectile")
	bool bUseTargetPrediction = false;

protected:
	virtual void ExecuteInstant() override {};
	virtual FVector CalculateProjectileDirection() const {return {};}
	virtual FTransform GetSpawnTransform() const {return {};}
};
