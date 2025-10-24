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
	void RefreshPanelButton(EInventoryPanel ActivePanel);

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnClickedButtonPanel OnClickedButtonPanel;

protected:
	virtual void NativeConstruct() override;
	virtual void SynchronizeProperties() override;

	UFUNCTION()
	void OnPanelButtonClicked();
    
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> Border;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button;

private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config", meta=(AllowPrivateAccess="true"))
	EInventoryPanel CurrentPanel = EInventoryPanel::P1;
    
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config", meta=(AllowPrivateAccess="true"))
	FButtonStyle ButtonStyleOn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Config", meta=(AllowPrivateAccess="true"))
	FButtonStyle ButtonStyleOff;
};
