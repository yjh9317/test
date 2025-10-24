// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PostProcessActor.generated.h"

class UPostProcessComponent;

USTRUCT()
struct FAdvancedEffectFeatures
{
	GENERATED_BODY()
	
};

UCLASS()
class RPGPROJECT_API APostProcessActor : public AActor
{
	GENERATED_BODY()
	
public:	
	APostProcessActor();

	UFUNCTION(BlueprintCallable)
	UMaterialInstanceDynamic* CreateMaterialInstance(UMaterial* Material);
protected:
	virtual void BeginPlay() override;

	UPROPERTY()
	TArray<UMaterial*> AllMaterials;

	UPROPERTY(EditDefaultsOnly)
	UPostProcessComponent* PostProcessComponent;

	UPROPERTY(EditDefaultsOnly)
	FPostProcessSettings PostProcessSettings;
};
