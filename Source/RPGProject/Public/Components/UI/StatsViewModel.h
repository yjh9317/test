// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/NoExportTypes.h"
#include "StatsViewModel.generated.h"

class UStatsComponent;
/**
 * 
 */

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUIValueChanged,const  FGameplayTag& ValueTag, float NewValue);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUITextChanged, const FGameplayTag& TextTag, FText NewText);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUIColorChanged,const  FGameplayTag& ColorTag, FLinearColor NewColor);

UCLASS()
class RPGPROJECT_API UStatsViewModel : public UObject
{
	GENERATED_BODY()

public:
	UStatsViewModel();
	
	FOnUIValueChanged OnUIValueChanged;
	FOnUITextChanged OnUITextChanged;
	FOnUIColorChanged OnUIColorChanged;


public:
	void Initialize(UStatsComponent* InStatsComponent);
	void Shutdown();

protected:
	UPROPERTY()
	TObjectPtr<UStatsComponent> StatsComponent;

	bool bIsInitialized;
	
	void BindToStatsComponent();
	void UnbindFromStatsComponent();

    UFUNCTION()
    void OnStatChangedInComponent(FGameplayTag StatTag, float NewValue);

	void UpdateUIValues(FGameplayTag StatTag);
	void UpdatePercent(FGameplayTag StatTag);
	void UpdateText(FGameplayTag StatTag);
	void UpdateColor(FGameplayTag StatTag);
	float CalculatePercent(FGameplayTag CurrentTag, FGameplayTag MaxTag) const;

public:
	virtual void BeginDestroy() override;
};
