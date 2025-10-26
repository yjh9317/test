// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "AbstractConsumable.generated.h"


enum class EAdditionalBuffEffect : uint8;
enum class EBuffEffectCategory : uint8;

UCLASS(Abstract)
class RPGPROJECT_API AAbstractConsumable : public AActor
{
	GENERATED_BODY()
	
public: 
	AAbstractConsumable();

	//~ Begin Public API
	virtual void UseBuff() {};
	virtual void RemoveBuff() {};
	//~ End Public API

protected:
	//~ Begin AActor Overrides
	virtual void BeginPlay() override;
	//~ End AActor Overrides

	//~ Begin Hooks for Child Classes
	virtual void InitializeTimelineAnimation() {};
	virtual void AddValueOverDuration() {};
	virtual void AddValueForDuration() {};
	virtual void ResetAddValueForDuration() {};
	virtual void RemoveValueFromStat() {};
	virtual void AddValueToStat() {};
	virtual void PlayAdditionalEffect() {};
	virtual void RemoveAdditionalEffect() {};
	//~ End Hooks for Child Classes

	//~ Begin Configuration Properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Consumable")
	EBuffEffectCategory BuffEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Consumable")
	EAdditionalBuffEffect AdditionalBuff;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Consumable")
	float Value;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Consumable")
	float Duration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Consumable")
	float Cooldown;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Consumable", meta=(Categories="Character.Stat"))
	FGameplayTag StatTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Consumable")
	int32 NumberOfReps;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Consumable|Sound")
	TObjectPtr<USoundBase> ConsumeSound;
	//~ End Configuration Properties

	//~ Begin State Properties
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="State")
	int32 CurrentRep;
	//~ End State Properties

	FTimerHandle OverDurationTimer;
	FTimerHandle ForDurationTimer;

private:
};
