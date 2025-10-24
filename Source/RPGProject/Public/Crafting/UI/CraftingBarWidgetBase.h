// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Item/Data/ItemDataStructs.h"
#include "CraftingBarWidgetBase.generated.h"

class UStationManagerComponent;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UCraftingBarWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:
	UCraftingBarWidgetBase(const FObjectInitializer& ObjectInitializer);

	//~ Begin Public API
	void RefreshWidgetData();
	void OnItemSelected();
	void AttemptCrafting();
	void StopCrafting();
	//~ End Public API

protected:
	//~ Begin UUserWidget Overrides
	virtual void NativeConstruct() override;
	//~ End UUserWidget Overrides

	//~ Begin Delegate Handlers & Internal Functions
	UFUNCTION()
	void PlayCraftingAnimation();

	UFUNCTION()
	void CraftingProcessStarted();
    
	UFUNCTION()
	void CraftingProcessStopped();

	UFUNCTION()
	void OnSliderValueChanged(float Value);
    
	UFUNCTION()
	void OnEditableTextChanged(const FText& Text);
	//~ End Delegate Handlers & Internal Functions

private:
	//~ Begin Internal State & Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStationManagerComponent> StationManagerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State", meta=(AllowPrivateAccess="true"))
	FCraftingData StoredCraftingData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State", meta=(AllowPrivateAccess="true"))
	int32 CurrentAmount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State", meta=(AllowPrivateAccess="true"))
	float SliderValue;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State", meta=(AllowPrivateAccess="true"))
	FString EditableText;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State", meta=(AllowPrivateAccess="true"))
	int32 MaxAmount;
	//~ End Internal State & Components
};
