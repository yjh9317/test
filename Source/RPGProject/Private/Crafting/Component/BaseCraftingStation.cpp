
#include "Crafting/Component/BaseCraftingStation.h"
#include "Header/DebugHeader.h"
#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"
#include "Algo/Reverse.h"
#include "Kismet/KismetArrayLibrary.h"

UBaseCraftingStation::UBaseCraftingStation()
{

	PrimaryComponentTick.bCanEverTick = true;

}

void UBaseCraftingStation::EventTryToCraftCurrentItem()
{
	// Called on Crafting Timer Completion
}

void UBaseCraftingStation::EventItemCrafted(AActor* OwningPlayer)
{
	OnItemCrafted.Broadcast(GetFirstItemFromQueue(),OwningPlayer);
	if(GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(CraftingTimer);
	}
	DecreaseCraftingQuantity(GetFirstItemFromQueue());
	
	if(TryToStartCraftingProcess())
	{
		FinishCraftingProcess();	
	}
	OnRefreshed.Broadcast();
}

void UBaseCraftingStation::EventSpawnCraftedItem(const FCraftingData& CraftingData,AActor* OwningActor)
{
	// Called from StationsManager Executed On Server
	OnSpawnCraftedItem.Broadcast(CraftingData,OwningActor);
}

void UBaseCraftingStation::EventFailedToInitializeCraftingProcess(const FCraftingData& CraftingData,int32 AmountToCraft)
{
	// Called from StationsManager Executed On Client
	OnFailedToInitializeProcess.Broadcast(CraftingData,AmountToCraft);
}

void UBaseCraftingStation::InitializeCraftingProcess(const FCraftingData& InCraftingData, int32 AmountToCraft)
{
	if(AmountToCraft <= 0) return;
	
	FCraftingData LocalCraftingData = InCraftingData;

	LocalCraftingData.CraftingQuantity = AmountToCraft;
	LocalCraftingData.MaxQuantity = AmountToCraft;
	AddToCraftingQueue(MoveTemp(LocalCraftingData));
}

bool UBaseCraftingStation::IsCurrentlyCrafted(const FCraftingData& InCraftingData)
{
	if(InCraftingData.CraftingID == GetFirstItemFromQueue().CraftingID)
	{
		return true;
	}
	return false;
}

void UBaseCraftingStation::StopCraftingProcess(const FCraftingData& InCraftingData, AActor* OwningActor)
{
	
}

void UBaseCraftingStation::FinishCraftingProcess()
{
	IsCrafting = false;
	FCraftingData FirstItem = GetFirstItemFromQueue();
	
	OnCraftingProcessFinished.Broadcast(FirstItem);
	RemoveFromCraftingQueue(FirstItem);
}

void UBaseCraftingStation::GetCraftingProcessTime(float& CurrentTime, float& TimeLeft)
{
	if(!GetWorld()) return;

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	if(TimerManager.IsTimerActive(CraftingTimer))
	{
		CurrentTime = TimerManager.GetTimerElapsed(CraftingTimer);
		TimeLeft = TimerManager.GetTimerRemaining(CraftingTimer);
	}
	else
	{
		CurrentTime = 0.f;
		TimeLeft = 0.f;
	}
}

void UBaseCraftingStation::StartCraftingTimer(float CraftingTime)
{
	if(!GetWorld()) return;

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	
	float DurationMultiplier = 1.f / CraftingDurationRate;
	float FinalCraftingTime  = CraftingTime * CraftingTime;
	FinalCraftingTime = FMath::Clamp(FinalCraftingTime,1.f,FinalCraftingTime);

	TimerManager.SetTimer(
		StartCraftingTimerHandle,
		this,
		&UBaseCraftingStation::EventTryToCraftCurrentItem,
		0.f,
		false
		,FinalCraftingTime);
}

bool UBaseCraftingStation::TryToStartCraftingProcess()
{
	FCraftingData CraftingData = GetFirstItemFromQueue();
	if(UInventoryUtilityLibrary::IsCraftingDataValid(CraftingData))
	{
		return StartCraftingProcess(CraftingData);
	}
	return false;
}

bool UBaseCraftingStation::StartCraftingProcess(const FCraftingData& InCraftingData)
{
	if(CanStartCraftingProcess(InCraftingData))
	{
		StartCraftingTimer(InCraftingData.CraftingTime);
		IsCrafting = true;
		OnCraftingProcessStarted.Broadcast(InCraftingData);
		return true;
	}
	return false;
}

bool UBaseCraftingStation::CanStartCraftingProcess(const FCraftingData& InCraftingData)
{
	if(!GetWorld()) return false;

	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	
	return InCraftingData.CraftingQuantity > 0 && !TimerManager.TimerExists(CraftingTimer);
}

void UBaseCraftingStation::AddToCraftingQueue(const FCraftingData& InCraftingData)
{
	if(!IsCraftableItemInQueue(InCraftingData))
	{
		int32 index = CraftingQueue.AddUnique(InCraftingData);
		OnAddedToCraftingQueue.Broadcast(InCraftingData,index);
		TryToStartCraftingProcess();
	}
	else
	{
		FCraftingData LocalCraftingData;
		ModifyCraftingQuantity(InCraftingData.CraftingID,InCraftingData.CraftingQuantity,InCraftingData.MaxQuantity,LocalCraftingData);
		OnAddedToCraftingQueue.Broadcast(InCraftingData,GetItemQueueIndex(InCraftingData.CraftingID));
		TryToStartCraftingProcess();
	}
}

bool UBaseCraftingStation::IsCraftableItemInQueue(const FCraftingData& InCraftingData)
{
	FCraftingData CraftingData = GetFirstItemFromQueue();
	if(UInventoryUtilityLibrary::IsCraftingDataValid(CraftingData))
	{
		FCraftingData TempCraftingData{};
		return GetItemFromQueueByID(InCraftingData.CraftingID,TempCraftingData);
	}
	return false;
}

bool UBaseCraftingStation::GetItemFromQueueByID(const FString& InCraftingID, FCraftingData& OutCraftingData)
{
	int32 QueueIndex = GetItemQueueIndex(InCraftingID);
	if(QueueIndex > -1)
	{
		OutCraftingData = CraftingQueue[QueueIndex];
		return true;
	}
	return false;	
}

int32 UBaseCraftingStation::GetItemQueueIndex(const FString& InCraftingID)
{
	for (int i = 0; i < CraftingQueue.Num(); ++i)
	{
		if( CraftingQueue[i].CraftingID == InCraftingID)
		{
			return i;
		}
	}
	return -1;
}

void UBaseCraftingStation::ModifyCraftingQuantity(const FString& InCraftingID, int32 InCraftingQuantity,
	int32 InMaxQuantity, FCraftingData& OutCraftingData)
{
	int32 QueueIndex = GetItemQueueIndex(InCraftingID);
	if(CraftingQueue.IsValidIndex(QueueIndex))
	{
		CraftingQueue[QueueIndex].CraftingQuantity += InCraftingQuantity;
		CraftingQueue[QueueIndex].MaxQuantity += InMaxQuantity;
		OutCraftingData = CraftingQueue[QueueIndex];
	}
	else
	{
		DebugHeader::LogWarning(TEXT("CraftingQueue[QueueIndex] is not valid index"));
	}
}

void UBaseCraftingStation::RemoveFromCraftingQueue(const FCraftingData& InCraftingData)
{
	int32 QueueIndex = GetItemQueueIndex(InCraftingData.CraftingID);
	if(QueueIndex > -1)
	{
		CraftingQueue.RemoveAt(QueueIndex);
		OnRemovedFromCraftingQueue.Broadcast(InCraftingData,QueueIndex);
		TryToStartCraftingProcess();
	}
}

FCraftingData UBaseCraftingStation::GetFirstItemFromQueue()
{
	if(CraftingQueue.IsValidIndex(0))
	{
		return CraftingQueue[0];
	}
	return {};
}

void UBaseCraftingStation::DecreaseCraftingQuantity(const FCraftingData& InCraftingData)
{
	FCraftingData CraftingData = GetFirstItemFromQueue();
	if(UInventoryUtilityLibrary::IsCraftingDataValid(CraftingData))
	{
		int32 QueueIndex = GetItemQueueIndex(InCraftingData.CraftingID);
		if(QueueIndex > -1)
		{
			CraftingQueue[QueueIndex].CraftingQuantity -= 1;
		}
	}
}

bool UBaseCraftingStation::HasPlayerEnoughGold(const FCraftingData& InCraftingData, int32 AmountToCraft,
	AActor* OwningActor)
{
	return true;
}

void UBaseCraftingStation::ClearCraftingQueue(AActor* OwningPlayer)
{
	
}

bool UBaseCraftingStation::CanBeAddedToQueue()
{
	return CraftingQueue.Num() < MaxQueuedItems;
}

bool UBaseCraftingStation::CanInitializeCraftingProcess(const FCraftingData& InCraftingData,
	int32 AmountToCraft, AActor* OwningActor, FText& FailureMessage)
{
	if(!CanCraftItems)
	{
		FailureMessage = FText::FromString("Item cannot be Crafted.");
		return false;
	}

	if(!CanBeAddedToQueue())
	{
		FailureMessage = FText::FromString("Queue is full.");
		return false;
	}

	if(!IsCurrentlyCrafted(InCraftingData))
	{
		FailureMessage = FText::FromString("Item is currently being Crafted");
		return false;
	}

	if(!HasPlayerEnoughGold(InCraftingData,AmountToCraft,OwningActor))
	{
		FailureMessage = FText::FromString("Item is currently being Crafted");
		return false;
	}

	return true;
}

int32 UBaseCraftingStation::GetMaxAmountThatCanBeCrafted(const FCraftingData& InCraftingData,
	AActor* OwningActor)
{
	return 1;
}

void UBaseCraftingStation::SortCraftingList(ESortMethod Method, const TArray<FCraftingData>& InCraftingArray,
	TArray<FCraftingData>& OutSortedList)
{
	TArray<FCraftingData> LocalCraftingDataArray{};
	switch (Method)
	{
	case ESortMethod::ByType:
		{
			TArray<EItemType> LocalSortingTypeArray = SortingOrderType;
		
			if(bReverseSortingOrder)
			{
				Algo::Reverse(LocalSortingTypeArray);
			}
			UInventoryUtilityLibrary::SortCraftingItemsByType(LocalCraftingDataArray,LocalSortingTypeArray);
			break;
		}
	case ESortMethod::ByRarity:
		{
			TArray<EItemRarity> LocalSortingRarityArray = SortingOrderRarity;
			if(bReverseSortingOrder)
			{
				Algo::Reverse(LocalSortingRarityArray);
			}
			UInventoryUtilityLibrary::SortCraftingItemsByRarity(LocalCraftingDataArray,LocalSortingRarityArray);
			break;
		}
	case ESortMethod::ByValue:
		{
			UInventoryUtilityLibrary::SortCraftingItemsByValue(LocalCraftingDataArray);
			if(bReverseSortingOrder)
			{
				Algo::Reverse(LocalCraftingDataArray);
			}
			break;
		}
	case ESortMethod::ByWeight:
		{
			UInventoryUtilityLibrary::SortCraftingItemsByWeight(LocalCraftingDataArray);
			if(bReverseSortingOrder)
			{
				Algo::Reverse(LocalCraftingDataArray);
			}
			break;
		}
	case ESortMethod::ByCraftingSubcategory:
		{
			TArray<ECraftingSubcategory> LocalSortingSubCategoryArray = SortingOrderSubcategory;
			if(bReverseSortingOrder)
			{
				Algo::Reverse(LocalSortingSubCategoryArray);
			}
			UInventoryUtilityLibrary::SortCraftingItemsBySubcategory(LocalCraftingDataArray,LocalSortingSubCategoryArray);
			break;
		}
	}
	OutSortedList = LocalCraftingDataArray;
}

void UBaseCraftingStation::SortCurrentListAndRefresh(ESortMethod Method)
{
	OnRefreshed.Broadcast();
}

void UBaseCraftingStation::AddCategoryToInUse(ECraftingCategory CraftingCategory)
{
	InUseCategories.AddUnique(CraftingCategory);
}

void UBaseCraftingStation::ClearInUseCategories()
{
	InUseCategories.Empty();
}


