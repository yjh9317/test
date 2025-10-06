// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "Inventory/Components/InventoryCoreComponent.h"
#include "InventoryPanelButtonWidget.generated.h"

class UBorder;
/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnClickedButtonPanel,EInventoryPanel, Panel);

UCLASS()
class RPGPROJECT_API UInventoryPanelButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void SynchronizeProperties() override;
	void RefreshPanelButton(EInventoryPanel ActivePanel);
	UFUNCTION()
	void OnPanelButtonClicked();
	
	FOnClickedButtonPanel OnClickedButtonPanel;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	EInventoryPanel CurrentPanel = EInventoryPanel::P1;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> Border;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FButtonStyle ButtonStyleOn;

	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	FButtonStyle ButtonStyleOff;
};
