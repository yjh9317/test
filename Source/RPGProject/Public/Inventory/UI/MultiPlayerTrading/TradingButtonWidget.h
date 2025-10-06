// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TradingButtonWidget.generated.h"

class UTextBlock;
class UButton;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTradingButtonClicked);

UCLASS()
class RPGPROJECT_API UTradingButtonWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativePreConstruct() override;

	UFUNCTION()
	void SetDefaultsState();

	UFUNCTION()
	void SetActiveState();

	UFUNCTION()
	void SetDisabledState();
	
	void SetButtonActiveStyle();
	void SetButtonDisabledStyle();
	void SetButtonDefaultStyle();
	void SetActiveText();
	void SetDefaultText();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Widget")
	TObjectPtr<UButton> Button_Trading;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Widget")
	TObjectPtr<UTextBlock> TextBlock_Trading;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	FText DefaultText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	FText ActiveText;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	FButtonStyle ActiveButtonStyle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	FButtonStyle DisabledButtonStyle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	FButtonStyle DefaultButtonStyle;
};
