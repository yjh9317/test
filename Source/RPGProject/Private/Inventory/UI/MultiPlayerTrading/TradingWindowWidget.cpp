
#include "Inventory/UI/MultiPlayerTrading/TradingWindowWidget.h"

void UTradingWindowWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UTradingWindowWidget::NativeDestruct()
{
	Super::NativeDestruct();
}

FReply UTradingWindowWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

FReply UTradingWindowWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UTradingWindowWidget::EventOnItemSlotClicked(UUserWidget* SlotWidget)
{
}

void UTradingWindowWidget::EventOnTradingCloseButtonClicked()
{
}

void UTradingWindowWidget::EventOnRefreshOwnerTrading()
{
}

void UTradingWindowWidget::EventOnRefreshReceiverTrading()
{
}

void UTradingWindowWidget::EventOnLockInButtonClicked()
{
}

void UTradingWindowWidget::EventOnTradeButtonClicked()
{
}

void UTradingWindowWidget::EventOnResetButtonClicked()
{
}

void UTradingWindowWidget::EventOnCloseWidgetActionTriggered()
{
}

void UTradingWindowWidget::EventOnSplitStackAccepted()
{
}

void UTradingWindowWidget::EventOnSplitStackCancel()
{
}

void UTradingWindowWidget::EventOnItemDroppedToSenderWidget()
{
}
