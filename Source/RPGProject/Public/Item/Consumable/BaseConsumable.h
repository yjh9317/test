// Fill out your copyright notice in the Dscription page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Consumable/AbstractConsumable.h"
#include "BaseConsumable.generated.h"

class UStatsComponent;
class UPostProcessComponent;
class UTimelineComponent;
class UBuffComponent;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API ABaseConsumable : public AAbstractConsumable
{
	GENERATED_BODY()

public:
	//~ Begin AAbstractConsumable Overrides (Public)
	virtual void UseBuff() override;
	virtual void RemoveBuff() override;
	//~ End AAbstractConsumable Overrides (Public)

protected:
	//~ Begin AActor Overrides
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~ End AActor Overrides

	//~ Begin AAbstractConsumable Overrides (Protected)
	virtual void InitializeTimelineAnimation() override;
	virtual void AddValueOverDuration() override;
	virtual void AddValueForDuration() override;
	virtual void ResetAddValueForDuration() override;
	virtual void RemoveValueFromStat() override;
	virtual void AddValueToStat() override;
	virtual void PlayAdditionalEffect() override;
	virtual void RemoveAdditionalEffect() override;
	//~ End AAbstractConsumable Overrides (Protected)

private:
	void LoadAndPlaySoundAsync();
    
	UFUNCTION() // UFUNCTION is needed for timeline delegate binding
	void UpdateEffectTimeline();

	UPROPERTY()
	TObjectPtr<UTimelineComponent> EffectTimeline;

	UPROPERTY(EditDefaultsOnly, Category="Config|Timeline", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCurveFloat> EffectCurve; 
    
	UPROPERTY()
	TObjectPtr<UBuffComponent> BuffComponent;

	UPROPERTY()
	TObjectPtr<UStatsComponent> StatsComponent;
	
};
