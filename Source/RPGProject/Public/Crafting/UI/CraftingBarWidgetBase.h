// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/Data/ItemDataStructs.h"
#include "CraftingBarWidgetBase.generated.h"

class UStationsManager;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UCraftingBarWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UCraftingBarWidgetBase(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;
	UFUNCTION()
	void RefreshWidgetData();

	UFUNCTION()
	void OnItemSelected();

	UFUNCTION()
	void PlayCraftingAnimation();

	UFUNCTION()
	void AttemptCrafting();

	UFUNCTION()
	void CraftingProcessStarted();

	UFUNCTION()
	void StopCrafting();

	UFUNCTION()
	void CraftingProcessStopped();

	UFUNCTION()
	void OnSliderValueChanged(float Value);
	
	UFUNCTION()
	void OnEditableTextChanged(const FText& Text);


	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStationsManager> StationsManager;

	UPROPERTY(VisibleAnywhere)
	FCraftingData StoredCraftingData;

	UPROPERTY(VisibleAnywhere)
	int32 CurrentAmount;

	UPROPERTY(VisibleAnywhere)
	float SliderValue;

	UPROPERTY(VisibleAnywhere)
	FString EditableText;

	UPROPERTY(VisibleAnywhere)
	int32 MaxAmount;
};
