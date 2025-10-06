// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Net/UnrealNetwork.h"
#include "Hitbox_CollisionDataAssetBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class RPGPROJECT_API UHitbox_CollisionDataAssetBase : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:

	// Collision shapes to trace against the world. Each notify detects each hit actor only once.
	UPROPERTY(EditAnywhere, Instanced, Category = "HitBox", BlueprintReadOnly, Replicated)
	TArray<class UHitbox_CollisionShape*> CollisionShapes;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);
		DOREPLIFETIME(UHitbox_CollisionDataAssetBase, CollisionShapes);
	}
};
