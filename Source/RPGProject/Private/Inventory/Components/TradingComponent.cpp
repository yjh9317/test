
#include "Inventory/Components/TradingComponent.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Header/DebugHeader.h"
#include "Interface/MultiplayerTradingInterface.h"
#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Shared/Macros/ProjectMacros.h"

UTradingComponent::UTradingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UTradingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTradingComponent, bHasAcceptedTrade);
	DOREPLIFETIME(UTradingComponent, bIsTradeLockedIn);
	DOREPLIFETIME(UTradingComponent, CurrentlyTradedGold);
	DOREPLIFETIME(UTradingComponent, CurrentlyTradedItems);
}

void UTradingComponent::ShowTradingWidget(AActor* Receiver)
{
	UInventoryUtilityLibrary::PlaySoundOnOpeningWidget(this);

	TradingWidget = CreateWidget<UUserWidget>(PlayerController,TradingWidgetClass);
	TradingWidget->AddToViewport(1);
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController,TradingWidget);
	PlayerController->bShowMouseCursor = true;
	PlayerController->SetIgnoreMoveInput(true);
	PlayerController->SetIgnoreLookInput(true);
}

void UTradingComponent::CloseTradingWidget()
{
	if(!TradingWidget) return;
	TradingWidget->RemoveFromParent();
	
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController,false);
	PlayerController->bShowMouseCursor = false;
	PlayerController->ResetIgnoreInputFlags();
}


void UTradingComponent::TradeItems(UTradingComponent* SenderComponent)
{
	if(GetOwner() && GetOwner()->Implements<UMultiplayerTradingInterface>())
	{
		IMultiplayerTradingInterface* TradingInterface = Cast<IMultiplayerTradingInterface>(GetOwner());
		TradingInterface->TradeItems(SenderComponent);
	}
}

void UTradingComponent::AddItemToTrade(const FItemData& InItemData)
{
	if(HAS_AUTHORITY_SAFE())
	{
		CurrentlyTradedItems.Add(InItemData);
		OnRefresh.Broadcast();
		SetTradeLock(false);
	}
	
}

void UTradingComponent::RemoveItemFromTrade(const FItemData& InItemData)
{
	if(HAS_AUTHORITY_SAFE())
	{
		CurrentlyTradedItems.Remove(InItemData);
		OnRefresh.Broadcast();
		SetTradeLock(false);
	}
}

bool UTradingComponent::IsItemBeingTraded(const FItemData& InItemData)
{
	for(const FItemData& TradedItem   : CurrentlyTradedItems)
	{
		if(UInventoryUtilityLibrary::AreItemsTheSame(TradedItem ,InItemData))
		{
			if(TradedItem.Index == InItemData.Index)
			{
				return true; 
			}
		}
	}
	return false;
}

void UTradingComponent::SetGoldToTrade(float Value)
{
	if(HAS_AUTHORITY_SAFE())
	{
		CurrentlyTradedGold = FMath::Clamp(Value,0.f,Value);
		SetTradeLock(false);
	}
}

void UTradingComponent::SetTradeLock(bool Value)
{
	if(bIsTradeLockedIn != Value) return;
	
	bIsTradeLockedIn = true;

	TArray<UTradingComponent*> TradingComponents = GetTradingComponents();

	for(const auto& LocalTradingComp: TradingComponents)
	{
		LocalTradingComp->bHasAcceptedTrade = false;
	}
}

bool UTradingComponent::CanMakeATrade(UTradingComponent* Sender, FString& FailureMessage)
{
	return true;
}

TArray<UTradingComponent*> UTradingComponent::GetTradingComponents()
{
	return {};
}
#pragma region OnRepFunction

void UTradingComponent::OnRep_bHasAcceptedTrade()
{
	OnRefresh.Broadcast();
}

void UTradingComponent::OnRep_bIsTradeLockedIn()
{
	OnRefresh.Broadcast();
}

void UTradingComponent::OnRep_CurrentlyTradedGold()
{
	OnRefresh.Broadcast();
}

#pragma endregion 

#pragma region Network

void UTradingComponent::Server_AddItemToTrade_Implementation(const FItemData& InItemData)
{
	AddItemToTrade(InItemData);
}

void UTradingComponent::Server_RemoveItemFromTade_Implementation(const FItemData& InItemData)
{
	RemoveItemFromTrade(InItemData);
}

void UTradingComponent::Server_SetTradeLock_Implementation(bool Value)
{
	SetTradeLock(Value);
}

void UTradingComponent::Server_SetGoldToTrade_Implementation(float Value)
{
	SetGoldToTrade(Value);
}

void UTradingComponent::Server_ClearCurrentItems_Implementation()
{
	CurrentlyTradedItems.Empty();
}

void UTradingComponent::Server_ClearCurrentGold_Implementation()
{
	CurrentlyTradedGold = 0.f;
}

void UTradingComponent::Server_TradeAccepted_Implementation()
{
	bHasAcceptedTrade = true;
}

void UTradingComponent::Server_TryToTradeItems_Implementation(UTradingComponent* SenderComp)
{
	FString LocalFailureMessage;
	
	if(CanMakeATrade(SenderComp,LocalFailureMessage))
	{
		if(SenderComp->CanMakeATrade(this,LocalFailureMessage))
		{
			TradeItems(SenderComp);
			SenderComp->TradeItems(this);
			Server_ResetTrade();
			SenderComp->Server_ResetTrade();
		}
		else 
		{
			DebugHeader::LogWarning(TEXT("2nd Player Failed to Trade"));
			TArray<UTradingComponent*> TradingComponents = GetTradingComponents();
			for(const auto& LocalTradingComp: TradingComponents)
			{
				LocalTradingComp->Client_TradeFailed(LocalFailureMessage);
				LocalTradingComp->SetTradeLock(false);
			}
		}
	}
	else 
	{
		DebugHeader::LogWarning(TEXT("1st Player Failed to Trade"));
		TArray<UTradingComponent*> TradingComponents = GetTradingComponents();
		for(const auto& LocalTradingComp: TradingComponents)
		{
			LocalTradingComp->Client_TradeFailed(LocalFailureMessage);
			LocalTradingComp->SetTradeLock(false);
		}
	}
}

void UTradingComponent::Server_ResetTrade_Implementation()
{
	CurrentlyTradedItems.Empty();
	CurrentlyTradedGold = 0.f;
	bHasAcceptedTrade = false;
	SetTradeLock(false);
}

void UTradingComponent::Server_InitializeTrade_Implementation(AActor* TradeReceiver)
{
	ReceiverActor = TradeReceiver; 
}

void UTradingComponent::Server_EndTrading_Implementation()
{
	
	TArray<UTradingComponent*> TradingComponents = GetTradingComponents();

	for(const auto& LocalTradingComp: TradingComponents)
	{
		LocalTradingComp->Server_ResetTrade();
		LocalTradingComp->ClientCloseTradingWidget();
		OnTradingEnded.Broadcast();
	}
}

void UTradingComponent::Client_TradeFailed_Implementation(const FString& FailureMessage)
{
	OnTradeFailed.Broadcast(FailureMessage);
}

void UTradingComponent::Client_TradeShowTradingWidget_Implementation(AActor* Receiver)
{
	ShowTradingWidget(Receiver);
}

void UTradingComponent::ClientCloseTradingWidget_Implementation()
{
	CloseTradingWidget();
}

void UTradingComponent::Client_InitializeTradingComponent_Implementation(APlayerController* PC)
{
	PlayerController = PC;
}
#pragma endregion 
