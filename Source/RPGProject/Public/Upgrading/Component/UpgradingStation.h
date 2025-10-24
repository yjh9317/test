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
	//~ Begin UActorComponent Overrides
	virtual void BeginPlay() override;
	//~ End UActorComponent Overrides

	//~ Begin UBaseCraftingStation Overrides
	virtual void EventTryToCraftCurrentItem() override;
	virtual void EventItemCrafted(AActor* OwningPlayer) override;
	virtual void EventSpawnCraftedItem(const FCraftingData& CraftingData, AActor* OwningActor) override;
	virtual void EventFailedToInitializeCraftingProcess(const FCraftingData& CraftingData, int32 AmountToCraft) override;
	virtual void StopCraftingProcess(const FCraftingData& InCraftingData, AActor* OwningActor) override;
	virtual bool HasPlayerEnoughGold(const FCraftingData& InCraftingData, int32 AmountToCraft, AActor* OwningActor) override;
	virtual void ClearCraftingQueue(AActor* OwningPlayer) override;
	virtual int32 GetMaxAmountThatCanBeCrafted(const FCraftingData& InCraftingData, AActor* OwningActor) override;
	virtual bool CanInitializeCraftingProcess(const FCraftingData& InCraftingData, int32 AmountToCraft, AActor* OwningActor, FText& FailureMessage) override;
	virtual void SortCurrentListAndRefresh(ESortMethod Method) override;
	//~ End UBaseCraftingStation Overrides

private:
	//~ Begin Upgrading System Logic
	UFUNCTION(Category="Upgrading System")
	void CreateUpgradingList();

	UFUNCTION(Category="Upgrading System")
	TArray<FCraftingData> GetCurrentItemsRelatedUpgradingList(AActor* OwningPlayer);

	UFUNCTION(Category="Upgrading System")
	const TMap<FString, FCraftingData>& GetUpgradingListMap() const;

	UFUNCTION(Category="Upgrading System")
	TArray<FString> GetUpgradingIDsFromItemID(const FString& ID);

	UFUNCTION(Category="Upgrading System")
	TArray<FCraftingData> GetUpgradingDataFromItemID(const FString& ID);

	UFUNCTION(Category="Upgrading System")
	const TArray<FCraftingData>& GetUpgradingList() const;
	//~ End Upgrading System Logic

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variable", meta=(AllowPrivateAccess="true"))
	TArray<UDataTable*> UpgradingDataTables;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variable", meta=(AllowPrivateAccess="true"))
	TMap<FString,FString> UpgradingItemList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variable", meta=(AllowPrivateAccess="true"))
	TMap<FString,FCraftingData> UpgradingListMap;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variable", meta=(AllowPrivateAccess="true"))
	TArray<FCraftingData> SortedUpgradingList;
};
