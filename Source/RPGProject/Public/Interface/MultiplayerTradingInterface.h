// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Data/ItemDataStructs.h"
#include "UObject/Interface.h"
#include "MultiplayerTradingInterface.generated.h"

class UTradingComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMultiplayerTradingInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGPROJECT_API IMultiplayerTradingInterface
{
	GENERATED_BODY()

public:
	virtual bool CanTradeItems(TArray<FItemData> IncomingItems) {return false;}
	virtual bool TradeItems(UTradingComponent* Comp) { return false; }
};
