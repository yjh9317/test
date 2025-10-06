// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Crafting/Component/BaseCraftingStation.h"
#include "CraftingStationComponent.generated.h"


UCLASS()
class RPGPROJECT_API UCraftingStationComponent : public UBaseCraftingStation
{
	GENERATED_BODY()
	
public:
	UCraftingStationComponent(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void EventTryToCraftCurrentItem() override;
	virtual void EventItemCrafted(AActor* OwningPlayer) override;
	virtual void EventSpawnCraftedItem(const FCraftingData& CraftingData, AActor* OwningActor) override;
	virtual void EventFailedToInitializeCraftingProcess(const FCraftingData& CraftingData, int32 AmountToCraft) override;

	UFUNCTION(Category="Crafting System")
	void CreateCraftingList();

	UFUNCTION(Category="Crafting System")
	TArray<FCraftingData> GetCraftingListArray();


	UFUNCTION(Category="Station Default")
	virtual void StopCraftingProcess(const FCraftingData& InCraftingData, AActor* OwningActor) override;
	UFUNCTION(Category="Station Default")
	virtual bool HasPlayerEnoughGold(const FCraftingData& InCraftingData, int32 AmountToCraft, AActor* OwningActor) override;
	UFUNCTION(Category="Station Default")
	virtual void ClearCraftingQueue(AActor* OwningPlayer) override;
	UFUNCTION(Category="Station Default")
	virtual int32 GetMaxAmountThatCanBeCrafted(const FCraftingData& InCraftingData, AActor* OwningActor) override;
	UFUNCTION(Category="Station Default")
	virtual void SortCurrentListAndRefresh(ESortMethod Method) override;

	UPROPERTY()
	TArray<UDataTable*> CraftingDataTables;
	
	UPROPERTY()
	TArray<FDataTableRowHandle> SingleCraftableItems;

	UPROPERTY()
	TArray<FCraftingData> CraftingList;
};
