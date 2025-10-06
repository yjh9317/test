// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TradingWindowWidget.generated.h"

class UTradingBaseWidget;
class UTradingSplitStackWidget;
class UTradingInventoryWidget;
class UHorizontalBox;
class UTradingButtonWidget;
class UButton;
class UTradingComponent;
class UInputAction;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UTradingWindowWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	
	UFUNCTION()
	void EventOnItemSlotClicked(UUserWidget* SlotWidget);

	UFUNCTION()
	void EventOnTradingCloseButtonClicked();
	
	UFUNCTION()
	void EventOnRefreshOwnerTrading();
	
	UFUNCTION()
	void EventOnRefreshReceiverTrading();

	UFUNCTION()
	void EventOnLockInButtonClicked();

	UFUNCTION()
	void EventOnTradeButtonClicked();

	UFUNCTION()
	void EventOnResetButtonClicked();

	UFUNCTION()
	void EventOnCloseWidgetActionTriggered();

	UFUNCTION()
	void EventOnSplitStackAccepted();

	UFUNCTION()
	void EventOnSplitStackCancel();

	UFUNCTION()
	void EventOnItemDroppedToSenderWidget();

	void UpdateTradeButton();
	void NavToTradingSender();
	void NavToTradingReceiver();
	void NavToTradingInventory();

	UPROPERTY( BlueprintReadOnly, Category="Component")
	TObjectPtr<UTradingComponent> OwnerTradingComponent;
	
	UPROPERTY( BlueprintReadOnly, Category="Component")
	TObjectPtr<UTradingComponent> ReceiverTradingComponent;
	
	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<UTradingButtonWidget> TradingButton_LockIn;
	
	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<UTradingButtonWidget> TradingButton_Reset;

	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<UTradingButtonWidget> TradingButton_Trade;
	
	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<UButton> Button_Close;
	
	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<UHorizontalBox> HorizontalBox_ButtonsBar;
	
	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<UTradingInventoryWidget> PlayerTradingInventoryWidget;
	
	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<UTradingSplitStackWidget> TradingSplitStackWidget;
	
	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<UTradingBaseWidget> TradingReceiverWidget;
	
	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<UTradingBaseWidget> TradingSenderWidget;
	
	UPROPERTY( BlueprintReadOnly, Category="Widget")
	TObjectPtr<UUserWidget> GoldWeightBarWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	TObjectPtr<AActor> TradingReceiverActor;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	TObjectPtr<UInputAction> CloseWidgetAction;

	
};
