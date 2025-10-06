// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Crafting/Component/BaseCraftingStation.h"
#include "UpgradingStation.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UUpgradingStation : public UBaseCraftingStation
{
	GENERATED_BODY()

public:	
	UUpgradingStation();

protected:
	virtual void BeginPlay() override;
	virtual void EventTryToCraftCurrentItem() override;
	virtual void EventItemCrafted(AActor* OwningPlayer) override;
	virtual void EventSpawnCraftedItem(const FCraftingData& CraftingData, AActor* OwningActor) override;
	virtual void EventFailedToInitializeCraftingProcess(const FCraftingData& CraftingData, int32 AmountToCraft) override;

	UFUNCTION(Category="Upgrading System")
	void CreateUpgradingList();

	UFUNCTION(Category="Upgrading System")
	TArray<FCraftingData> GetCurrentItemsRelatedUpgradingList(AActor* OwningPlayer);

	UFUNCTION(Category="Upgrading System")
	TMap<FString,FCraftingData> GetUpgradingListMap();

	UFUNCTION(Category="Upgrading System")
	TArray<FString> GetUpgradingIDsFromItemID(const FString& ID);

	UFUNCTION(Category="Upgrading System")
	TArray<FCraftingData> GetUpgradingDataFromItemID(const FString& ID);

	UFUNCTION(Category="Upgrading System")
	TArray<FCraftingData> GetUpgradingList();

	UFUNCTION(Category="Station Default")
	virtual void StopCraftingProcess(const FCraftingData& InCraftingData, AActor* OwningActor) override;
	UFUNCTION(Category="Station Default")
	virtual bool HasPlayerEnoughGold(const FCraftingData& InCraftingData, int32 AmountToCraft, AActor* OwningActor) override;
	UFUNCTION(Category="Station Default")
	virtual void ClearCraftingQueue(AActor* OwningPlayer) override;
	UFUNCTION(Category="Station Default")
	virtual int32 GetMaxAmountThatCanBeCrafted(const FCraftingData& InCraftingData, AActor* OwningActor) override;
	UFUNCTION(Category="Station Default")
	virtual bool CanInitializeCraftingProcess(const FCraftingData& InCraftingData, int32 AmountToCraft, AActor* OwningActor, FText& FailureMessage) override;
	UFUNCTION(Category="Station Default")
	virtual void SortCurrentListAndRefresh(ESortMethod Method) override;
		
};
