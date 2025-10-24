// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/Data/ItemDataStructs.h"
#include "TradingComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRefresh);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTradeFailed,const FString&, Message);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTradingEnded);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UTradingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTradingComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	FOnRefresh OnRefresh;
	FOnTradeFailed OnTradeFailed;
	FOnTradingEnded OnTradingEnded;

#pragma region Network
	
	UFUNCTION(Server,Reliable)
	void Server_AddItemToTrade(const FItemData& InItemData);

	UFUNCTION(Server,Reliable)
	void Server_RemoveItemFromTade(const FItemData& InItemData);

	UFUNCTION(Server,Reliable)
	void Server_SetTradeLock(bool Value);
	
	UFUNCTION(Server,Reliable)
	void Server_SetGoldToTrade(float Value);

	UFUNCTION(Server,Reliable)
	void Server_ClearCurrentItems();

	UFUNCTION(Server,Reliable)
	void Server_ClearCurrentGold();

	UFUNCTION(Server,Reliable)
	void Server_TradeAccepted();

	UFUNCTION(Server,Reliable)
	void Server_TryToTradeItems(UTradingComponent* SenderComp);

	UFUNCTION(Server,Reliable)
	void Server_ResetTrade();

	UFUNCTION(Server,Reliable)
	void Server_InitializeTrade(AActor* TradeReceiver);

	UFUNCTION(Server,Reliable)
	void Server_EndTrading();

	UFUNCTION(Client,Reliable)
	void Client_InitializeTradingComponent(APlayerController* PC);
	
	UFUNCTION(Client,Reliable)
	void Client_TradeFailed(const FString& FailureMessage);

	UFUNCTION(Client,Reliable)
	void Client_TradeShowTradingWidget(AActor* Receiver);

	UFUNCTION(Client,Reliable)
	void ClientCloseTradingWidget();

	UFUNCTION()
	void TradeItems(UTradingComponent* SenderComponent);

#pragma endregion 
	
protected:

	void ShowTradingWidget(AActor* Receiver);
	void CloseTradingWidget();
	
	void AddItemToTrade(const FItemData& InItemData);
	void RemoveItemFromTrade(const FItemData& InItemData);
	bool IsItemBeingTraded(const FItemData& InItemData);
	void SetGoldToTrade(float Value);
	void SetTradeLock(bool Value);
	bool CanMakeATrade(UTradingComponent* Sender,FString& FailureMessage);
	TArray<UTradingComponent*> GetTradingComponents();
	

	
private:
	// MultiplayerTrading Widget으로 바꿔야함
	UPROPERTY()
	TObjectPtr<UUserWidget> TradingWidget;

	// MultiplayerTrading Widget으로 바꿔야함
	UPROPERTY()
	TSubclassOf<UUserWidget> TradingWidgetClass;
	
	UPROPERTY()
	TObjectPtr<APlayerController> PlayerController;

	UPROPERTY()
	TObjectPtr<AActor> ReceiverActor;

	UPROPERTY(ReplicatedUsing = OnRep_bHasAcceptedTrade)
	bool bHasAcceptedTrade;

	UPROPERTY(ReplicatedUsing = OnRep_bIsTradeLockedIn)
	bool bIsTradeLockedIn;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentlyTradedGold)
	float CurrentlyTradedGold;
	
	UPROPERTY(Replicated)
	TArray<FItemData> CurrentlyTradedItems;

#pragma region OnRepFunction
	
	UFUNCTION()
	void OnRep_bHasAcceptedTrade();

	UFUNCTION()
	void OnRep_bIsTradeLockedIn();

	UFUNCTION()
	void OnRep_CurrentlyTradedGold();

#pragma endregion 
};

