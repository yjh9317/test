// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Action/BaseAction.h"
#include "RollAction.generated.h"

class UMotionWarpingComponent;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API URollAction : public UBaseAction
{
	GENERATED_BODY()
public:
	URollAction(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, Category = "Roll")
	float RollDistance = 300.0f;
    
	UPROPERTY(EditDefaultsOnly, Category = "Roll")
	float ObstacleCheckHeight = 100.0f;
    
	UPROPERTY(EditDefaultsOnly, Category = "Roll")
	FName WarpTargetName = "RollTargetLocation";

public:
	virtual void Initialize(AActor* InOwner) override;
	
protected:
	UPROPERTY()
	TObjectPtr<UMotionWarpingComponent> CachedMotionWarpingComponent;
	mutable FVector CachedWarpLocation;
	mutable FVector CachedInputDirection;

	static constexpr float CacheValidityDuration = 0.1f;
	virtual bool CanExecute() const override;
protected:
	virtual void OnExecute() override;
	bool CalculateWarpLocation() const;
	void InvalidateCache();
	bool IsCacheValid() const;
};
