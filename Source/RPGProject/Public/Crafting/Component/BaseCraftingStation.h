// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/Data/ItemDataStructs.h"
#include "BaseCraftingStation.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCraftingProcessStarted,const FCraftingData& CraftingData);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAddedToCraftingQueue,const FCraftingData& CraftingData,int32 QueueIndex);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRemovedFromCraftingQueue,const FCraftingData& CraftingData,int32 QueueIndex);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCraftingProcessFinished,const FCraftingData& CraftingData);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnCraftingProcessStopped,const FCraftingData& CraftingData);
DECLARE_MULTICAST_DELEGATE(FOnClearedCraftingQueue);
DECLARE_MULTICAST_DELEGATE(FOnRefreshed);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemCrafted,const FCraftingData& CraftingData,AActor* OwningActor);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSpawnCraftedItem,const FCraftingData& CraftingData,AActor* OwningActor);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnFailedToInitializeProcess,const FCraftingData& CraftingData,int32 AmountToCraft);



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UBaseCraftingStation : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBaseCraftingStation();

	UFUNCTION()
	virtual void EventTryToCraftCurrentItem();

	UFUNCTION()
	virtual void EventItemCrafted(AActor* OwningPlayer);

	UFUNCTION()
	virtual void EventSpawnCraftedItem(const FCraftingData& CraftingData,AActor* OwningActor);

	UFUNCTION()
	virtual void EventFailedToInitializeCraftingProcess(const FCraftingData& CraftingData,int32 AmountToCraft);

	void InitializeCraftingProcess(const FCraftingData& InCraftingData,int32 AmountToCraft);
	bool IsCurrentlyCrafted(const FCraftingData& InCraftingData);
	virtual void StopCraftingProcess(const FCraftingData& InCraftingData,AActor* OwningActor);
	void FinishCraftingProcess();
	void GetCraftingProcessTime(float& CurrentTime, float& TimeLeft);
	void StartCraftingTimer(float CraftingTime);
	bool TryToStartCraftingProcess();
	bool StartCraftingProcess(const FCraftingData& InCraftingData);
	bool CanStartCraftingProcess(const FCraftingData& InCraftingData);
	void AddToCraftingQueue(const FCraftingData& InCraftingData);
	bool IsCraftableItemInQueue(const FCraftingData& InCraftingData);
	bool GetItemFromQueueByID(const FString& InCraftingID,FCraftingData& OutCraftingData);
	int32 GetItemQueueIndex(const FString& InCraftingID);
	void ModifyCraftingQuantity(const FString& InCraftingID,int32 InCraftingQuantity,int32 InMaxQuantity, FCraftingData& OutCraftingData);
	void RemoveFromCraftingQueue(const FCraftingData& InCraftingData);
	FCraftingData GetFirstItemFromQueue();
	void DecreaseCraftingQuantity(const FCraftingData& InCraftingData);
	virtual bool HasPlayerEnoughGold(const FCraftingData& InCraftingData,int32 AmountToCraft,AActor* OwningActor);
	virtual void ClearCraftingQueue(AActor* OwningPlayer);
	bool CanBeAddedToQueue();
	virtual bool CanInitializeCraftingProcess(const FCraftingData& InCraftingData,int32 AmountToCraft,AActor* OwningActor,FText& FailureMessage);
	virtual int32 GetMaxAmountThatCanBeCrafted(const FCraftingData& InCraftingData,AActor* OwningActor);
	void SortCraftingList(ESortMethod Method,const TArray<FCraftingData>& InCraftingArray,TArray<FCraftingData>& OutSortedList);
	virtual void SortCurrentListAndRefresh(ESortMethod Method);
	void AddCategoryToInUse(ECraftingCategory CraftingCategory);
	void ClearInUseCategories();
	

	FOnCraftingProcessStarted OnCraftingProcessStarted;
	FOnAddedToCraftingQueue OnAddedToCraftingQueue;
	FOnRemovedFromCraftingQueue OnRemovedFromCraftingQueue;
	FOnCraftingProcessFinished OnCraftingProcessFinished;
	FOnCraftingProcessStopped OnCraftingProcessStopped;
	FOnClearedCraftingQueue OnClearedCraftingQueue;
	FOnRefreshed OnRefreshed;
	FOnItemCrafted OnItemCrafted;
	FOnSpawnCraftedItem OnSpawnCraftedItem;
	FOnFailedToInitializeProcess OnFailedToInitializeProcess;
	

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Station Default")
	bool CanCraftItems;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Station Default")
	ECraftingStation CraftingStation;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Station Default")
	bool IsCrafting;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Station Default")
	FTimerHandle CraftingTimer;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Station Default")
	bool SpawnCraftedItem;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Station Default")
	bool CraftOnlyWhenWindowIsOpen;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Station Default")
	float CraftingCostMultiplier;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Station Default")
	float CraftingDurationRate;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Station Default")
	bool ShowLockedItems;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Station Default")
	TArray<FCraftingData> CraftingQueue;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Station Default")
	int32 MaxQueuedItems;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Station Default")
	TArray<EItemRarity> SortingOrderRarity;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Station Default")
	TArray<ECraftingSubcategory> SortingOrderSubcategory;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Station Default")
	TArray<EItemType> SortingOrderType;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Station Default")
	bool bReverseSortingOrder;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Station Default")
	ESortMethod CurrentSortingMethod;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Station Default")
	TArray<ECraftingCategory> InUseCategories;

	FTimerHandle StartCraftingTimerHandle;
};
