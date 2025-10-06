// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "AbstractConsumable.generated.h"


enum class EAdditionalBuffEffect : uint8;
enum class EStatCategory : uint8;
enum class EBuffEffectCategory : uint8;

UCLASS(Abstract)
class RPGPROJECT_API AAbstractConsumable : public AActor
{
	GENERATED_BODY()
	
public:	
	AAbstractConsumable();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(Categories="Character.Stat"))
	FGameplayTag StatTag;
	
	EBuffEffectCategory BuffEffect;
	EAdditionalBuffEffect AdditionalBuff;
	float Value;
	float Duration;
	float Cooldown;
	int32 NumberOfReps;
	int32 CurrentRep;
	
	virtual void UseBuff() {};
	virtual void RemoveBuff() {};
	virtual void InitializeTimelineAnimation() {};
	virtual void AddValueOverDuration() {};
	virtual void AddValueForDuration() {};
	virtual void ResetAddValueForDuration() {};
	virtual void RemoveValueFromStat() {};
	virtual void AddValueToStat() {};
	virtual void PlayAdditionalEffect() {};
	virtual void RemoveAdditionalEffect() {};
protected:
	virtual void BeginPlay() override;

	FTimerHandle OverDurationTimer;
	FTimerHandle ForDurationTimer;
	UPROPERTY()
	TObjectPtr<USoundBase> ConsumeSound;

};
