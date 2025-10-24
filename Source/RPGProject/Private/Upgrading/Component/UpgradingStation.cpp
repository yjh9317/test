
#include "Upgrading/Component/UpgradingStation.h"

#include "Interface/ComponentManager.h"
#include "Inventory/Components/EquipmentComponent.h"
#include "Inventory/Components/InventoryComponent.h"
#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"
#include "Kismet/BlueprintMapLibrary.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Components/StationManagerComponent.h"

UUpgradingStation::UUpgradingStation()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UUpgradingStation::BeginPlay()
{
	Super::BeginPlay();

	CreateUpgradingList();
}

void UUpgradingStation::EventTryToCraftCurrentItem()
{
	APlayerController* LocalPC = UGameplayStatics::GetPlayerController(GetWorld(),0);
	if(LocalPC)
	{
		UStationManagerComponent* LocalStationComp = LocalPC->FindComponentByClass<UStationManagerComponent>();
		if(LocalStationComp)
		{
			if(LocalStationComp->CanCraft(this))
			{
				LocalStationComp->Server_TryToCraftItem(this,GetFirstItemFromQueue(),LocalPC);
			}
			else
			{
				StopCraftingProcess(GetFirstItemFromQueue(),LocalPC);
			}
		}
	}
}

void UUpgradingStation::EventItemCrafted(AActor* OwningPlayer)
{
	Super::EventItemCrafted(OwningPlayer);
}

void UUpgradingStation::EventSpawnCraftedItem(const FCraftingData& CraftingData, AActor* OwningActor)
{
	Super::EventSpawnCraftedItem(CraftingData, OwningActor);
}

void UUpgradingStation::EventFailedToInitializeCraftingProcess(const FCraftingData& CraftingData, int32 AmountToCraft)
{
	Super::EventFailedToInitializeCraftingProcess(CraftingData, AmountToCraft);
}

void UUpgradingStation::CreateUpgradingList()
{
	// Create Upgrading List from Data Tables
	ClearInUseCategories();
	UpgradingItemList.Empty();
	UpgradingListMap.Empty();

	for(const auto& DT : UpgradingDataTables)
	{
		TArray<FName> RowNames; 
		UDataTableFunctionLibrary::GetDataTableRowNames(DT,RowNames);

		for(const auto& RowName : RowNames)
		{
			FCraftingData LocalUpgradingData = *DT->FindRow<FCraftingData>(RowName,"");

			if(UInventoryUtilityLibrary::IsCraftingDataValid(LocalUpgradingData))
			{
				TArray<FItemStack> LocalInputItems = LocalUpgradingData.InputItems;
				if(LocalInputItems.IsValidIndex(0) && LocalInputItems[0].TableAndRow.DataTable)
				{
					FItemData LocalItemData = *LocalInputItems[0].TableAndRow.DataTable->FindRow<FItemData>(LocalInputItems[0].TableAndRow.RowName,"");
					if(UInventoryUtilityLibrary::IsItemClassValid(LocalItemData))
					{
						UpgradingItemList.Add(LocalItemData.ID,LocalUpgradingData.CraftingID);
						UpgradingListMap.Add(LocalUpgradingData.CraftingID,LocalUpgradingData);
						AddCategoryToInUse(LocalUpgradingData.Category);
					}
				}
			}
		}
	}
	// Sort default List
	TArray<FCraftingData> LocalSortCraftingData;
	UpgradingListMap.GenerateValueArray(LocalSortCraftingData);
	SortCraftingList(CurrentSortingMethod,LocalSortCraftingData,SortedUpgradingList);
}

TArray<FCraftingData> UUpgradingStation::GetCurrentItemsRelatedUpgradingList(AActor* OwningPlayer)
{
	TArray<FCraftingData> LocalUpgradingArray;
	IComponentManager* ComponentManager = Cast<IComponentManager>(OwningPlayer);
	if(ComponentManager)
	{
		UEquipmentComponent* LocalEquipComp = ComponentManager->GetEquipmentComponent();
		if(LocalEquipComp)
		{
			UInventoryComponent* LocalInventoryComp = ComponentManager->GetInventoryComponent();
			if(LocalInventoryComp)
			{
				TArray<FItemData> ItemsArray;
				ItemsArray.Append(LocalEquipComp->GetEquipmentItems());
				ItemsArray.Append(LocalInventoryComp->GetCombinedInventories());

				for(const auto& LocalItemData : ItemsArray)
				{
					LocalUpgradingArray.Append(GetUpgradingDataFromItemID(LocalItemData.ID));
				}
			}
		}
	}
	return LocalUpgradingArray;
}

const TMap<FString, FCraftingData>& UUpgradingStation::GetUpgradingListMap() const
{
	return UpgradingListMap;
}

TArray<FString> UUpgradingStation::GetUpgradingIDsFromItemID(const FString& ID)
{
	TArray<FString> LocalUpgradingIDList;
	
	TArray<FString> ItemIDList;
	UpgradingItemList.GenerateValueArray(ItemIDList);
	TArray<FString> CraftingItemIDList;
	UpgradingItemList.GenerateKeyArray(CraftingItemIDList);
	
	for(int Index = 0; Index < ItemIDList.Num(); ++Index)
	{
		if(ItemIDList[Index] == ID)
		{
			LocalUpgradingIDList.AddUnique(CraftingItemIDList[Index]);
		}
	}
	return LocalUpgradingIDList;
}

TArray<FCraftingData> UUpgradingStation::GetUpgradingDataFromItemID(const FString& ID)
{
	TArray<FCraftingData> LocalUpgradingDatas;
	
	TArray<FString> UpgradingIDList = GetUpgradingIDsFromItemID(ID);

	for(const auto& UpgradingID : UpgradingIDList)
	{
		if(UpgradingListMap.Find(UpgradingID))
		{
			LocalUpgradingDatas.Add(UpgradingListMap[UpgradingID]);
		}
	}
	return LocalUpgradingDatas;
}

const TArray<FCraftingData>& UUpgradingStation::GetUpgradingList() const
{
	return SortedUpgradingList;
}

void UUpgradingStation::StopCraftingProcess(const FCraftingData& InCraftingData, AActor* OwningActor)
{
	UWorld* World = GetWorld();
	if(IsCurrentlyCrafted(InCraftingData) && World)
	{
		World->GetTimerManager().ClearTimer(CraftingTimer);
		IsCrafting = false;

		if(OwningActor)
		{
			UStationManagerComponent* LocalStationComp = OwningActor->FindComponentByClass<UStationManagerComponent>();
			if(LocalStationComp)
			{
				LocalStationComp->Server_ReturnSecuredMaterials(this,InCraftingData,OwningActor);
				OnCraftingProcessStopped.Broadcast(InCraftingData);
				RemoveFromCraftingQueue(InCraftingData);
			}
		}
	}
}

bool UUpgradingStation::HasPlayerEnoughGold(const FCraftingData& InCraftingData, int32 AmountToCraft,
	AActor* OwningActor)
{
	if(OwningActor)
	{
		if(UInventoryComponent* LocalInventoryComp = OwningActor->FindComponentByClass<UInventoryComponent>())
		{
			return LocalInventoryComp->OwnerGold >= InCraftingData.CraftingCost * CraftingCostMultiplier * AmountToCraft;
		}
	}
	return true;
}

void UUpgradingStation::ClearCraftingQueue(AActor* OwningPlayer)
{
	if(!OwningPlayer) return;
	
	UWorld* World = GetWorld();
	UStationManagerComponent* LocalStationComp = OwningPlayer->FindComponentByClass<UStationManagerComponent>();
	if(LocalStationComp && World)
	{
		World->GetTimerManager().ClearTimer(CraftingTimer);
		IsCrafting = false;

		for(const auto& LocalCraftingData : CraftingQueue)
		{
			LocalStationComp->Server_ReturnSecuredMaterials(this,LocalCraftingData,OwningPlayer);
		}
		CraftingQueue.Empty();
		OnClearedCraftingQueue.Broadcast();
		OnRefreshed.Broadcast();
	}
}

int32 UUpgradingStation::GetMaxAmountThatCanBeCrafted(const FCraftingData& InCraftingData, AActor* OwningActor)
{
	if (!OwningActor) return 0;
	UInventoryComponent* LocalInventoryComp = OwningActor->FindComponentByClass<UInventoryComponent>();
	if (!LocalInventoryComp) return 0;

	TArray<FItemData> LocalCraftableItemArray;
	TArray<FItemStack> LocalMaterialsArray;
	UInventoryUtilityLibrary::GetCraftableData(InCraftingData, LocalCraftableItemArray, LocalMaterialsArray);

	if(LocalMaterialsArray.Num() == 0) return 0;

	int32 MaxCraftableAmount = INT32_MAX;
	
	for (const auto& LocalMaterial : LocalMaterialsArray)
	{
		if (!LocalMaterial.TableAndRow.DataTable || LocalMaterial.Quantity <= 0) return 0;

		FItemData FoundItemDataFromDataTable = *LocalMaterial.TableAndRow.DataTable->FindRow<FItemData>(LocalMaterial.TableAndRow.RowName, "");

		FItemData OwnedItemData;
		if (!LocalInventoryComp->GetItemByData(FoundItemDataFromDataTable, OwnedItemData))
		{
			return 0;
		}

		int32 AmountPossibleWithThis = OwnedItemData.Quantity / LocalMaterial.Quantity;

		MaxCraftableAmount = FMath::Min(MaxCraftableAmount, AmountPossibleWithThis);
	}
	return MaxCraftableAmount;
}

bool UUpgradingStation::CanInitializeCraftingProcess(const FCraftingData& InCraftingData, int32 AmountToCraft,
	AActor* OwningActor, FText& FailureMessage)
{
	if(!CanCraftItems)
	{
		FailureMessage = FText::FromString(TEXT("Item cannot be Upgraded."));
		return false;
	}

	if(!CanBeAddedToQueue())
	{
		FailureMessage = FText::FromString(TEXT("An Item is already being Upgraded."));
		return false;
	}

	if(IsCurrentlyCrafted(InCraftingData))
	{
		FailureMessage = FText::FromString(TEXT("An Item is currently being Upgraded."));
		return false;
	}

	if(!HasPlayerEnoughGold(InCraftingData,AmountToCraft,OwningActor))
	{
		FailureMessage = FText::FromString(TEXT("Not enough Gold to Upgrade an Item."));
		return false;
	}
	return true;
}

void UUpgradingStation::SortCurrentListAndRefresh(ESortMethod Method)
{
	TArray<FCraftingData> SortedList;
	SortCraftingList(Method,SortedUpgradingList,SortedList);
	
	SortedUpgradingList = SortedList;
	OnRefreshed.Broadcast();
}


