// Fill out your copyright notice in the Description page of Project Settings.


#include "Crafting/Component/CraftingStationComponent.h"

#include "Inventory/Components/InventoryComponent.h"
#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Components/StationManagerComponent.h"

UCraftingStationComponent::UCraftingStationComponent(const FObjectInitializer& ObjectInitializer)
{
}

void UCraftingStationComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCraftingStationComponent::EventTryToCraftCurrentItem()
{
	AActor* OwnerActor = GetOwner();
	if (OwnerActor == nullptr)
	{
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(OwnerActor);
	if (OwnerPawn == nullptr)
	{
		return; 
	}

	APlayerController* LocalPC = Cast<APlayerController>(OwnerPawn->GetController());

	if (nullptr == LocalPC)
	{
		return; 
	}

	UStationManagerComponent* StationsManager = LocalPC->FindComponentByClass<UStationManagerComponent>();
	if(nullptr == StationsManager)
	{
		return;
	}
	
	const FCraftingData& FirstItem = GetFirstItemFromQueue();
	if(StationsManager && StationsManager->CanCraft(this))
	{
		StationsManager->Server_TryToCraftItem(this,FirstItem,OwnerActor);	
	}
	else
	{
		StopCraftingProcess(FirstItem,LocalPC);
	}
}

void UCraftingStationComponent::EventItemCrafted(AActor* OwningPlayer)
{
	Super::EventItemCrafted(OwningPlayer);
}

void UCraftingStationComponent::EventSpawnCraftedItem(const FCraftingData& CraftingData, AActor* OwningActor)
{
	Super::EventSpawnCraftedItem(CraftingData, OwningActor);
}

void UCraftingStationComponent::EventFailedToInitializeCraftingProcess(const FCraftingData& CraftingData,
	int32 AmountToCraft)
{
	Super::EventFailedToInitializeCraftingProcess(CraftingData, AmountToCraft);
}

void UCraftingStationComponent::CreateCraftingList()
{
	// Create Crafting List from Data Table
	CraftingList.Empty();

	for(const auto& DT : CraftingDataTables)
	{
		TArray<FName> RowNames{};
		UDataTableFunctionLibrary::GetDataTableRowNames(DT,RowNames);

		for(const auto& RowName : RowNames)
		{
			FCraftingData FoundCraftingData = *DT->FindRow<FCraftingData>(RowName,"");
			if(UInventoryUtilityLibrary::IsCraftingDataValid(FoundCraftingData))
			{
				CraftingList.Add(FoundCraftingData);
				AddCategoryToInUse(FoundCraftingData.Category);
			}
		}
	}
	
	// Add to Crafting List from Data Table Rows
	for(const auto& RowHandle : SingleCraftableItems)
	{
		FCraftingData FoundCraftingData;
		if(UInventoryUtilityLibrary::GetCraftingDataFromTableRow(RowHandle,FoundCraftingData))
		{
			if(UInventoryUtilityLibrary::IsCraftingDataValid(FoundCraftingData))
			{
				CraftingList.Add(FoundCraftingData);
				AddCategoryToInUse(FoundCraftingData.Category);
			}
		}
	}
	// Sort Array by Rarity
	TArray<FCraftingData> LocalCraftingDataList;
	SortCraftingList(CurrentSortingMethod,CraftingList,LocalCraftingDataList);
	CraftingList = LocalCraftingDataList;
}

TArray<FCraftingData> UCraftingStationComponent::GetCraftingListArray()
{
	return CraftingList;
}

void UCraftingStationComponent::StopCraftingProcess(const FCraftingData& InCraftingData, AActor* OwningActor)
{
	if(nullptr == GetWorld() || nullptr == OwningActor)
	{
		return;
	}
	if(IsCurrentlyCrafted(InCraftingData))
	{
		GetWorld()->GetTimerManager().ClearTimer(CraftingTimer);
		IsCrafting = false;
	}
	
	UStationManagerComponent* StationsManager = OwningActor->FindComponentByClass<UStationManagerComponent>();
	if(StationsManager)
	{
		StationsManager->Server_ReturnSecuredMaterials(this,InCraftingData,OwningActor);
		OnCraftingProcessStopped.Broadcast(InCraftingData);
		RemoveFromCraftingQueue(InCraftingData);
	}
}

bool UCraftingStationComponent::HasPlayerEnoughGold(const FCraftingData& InCraftingData, int32 AmountToCraft,
	AActor* OwningActor)
{
	if(nullptr == OwningActor)
	{
		return false;
	}
	UInventoryComponent* LocalInventoryComp = OwningActor->FindComponentByClass<UInventoryComponent>();
	if(nullptr == LocalInventoryComp)
	{
		return true;
	}
	float RequiredGold = InCraftingData.CraftingCost * CraftingCostMultiplier * AmountToCraft;
	return LocalInventoryComp->OwnerGold >= RequiredGold;
}

void UCraftingStationComponent::ClearCraftingQueue(AActor* OwningPlayer)
{
	if(nullptr == GetWorld() || nullptr == OwningPlayer)
	{
		return;
	}
	UStationManagerComponent* StationsManager = OwningPlayer->FindComponentByClass<UStationManagerComponent>();
	if(nullptr == StationsManager)
	{
		return;
	}
	GetWorld()->GetTimerManager().ClearTimer(CraftingTimer);
	IsCrafting = false;

	for(const auto& CraftingData: CraftingQueue)
	{
		StationsManager->Server_ReturnSecuredMaterials(this,CraftingData,OwningPlayer);
	}
	CraftingQueue.Empty();
	OnClearedCraftingQueue.Broadcast();
	OnRefreshed.Broadcast();
}

int32 UCraftingStationComponent::GetMaxAmountThatCanBeCrafted(const FCraftingData& InCraftingData, AActor* OwningActor)
{
	if (OwningActor == nullptr)
	{
		return 0;
	}
	UInventoryComponent* InventoryComp = OwningActor->FindComponentByClass<UInventoryComponent>();
	if (InventoryComp == nullptr)
	{
		return 0;
	}

	TArray<FItemData> LocalCraftableItemArray;
	TArray<FItemStack> LocalMaterialsArray;
	UInventoryUtilityLibrary::GetCraftableData(InCraftingData, LocalCraftableItemArray, LocalMaterialsArray);

	if (LocalMaterialsArray.IsEmpty())
	{
		return 1;
	}

	int32 MaxPossibleCrafts = INT32_MAX;

	for (const FItemStack& Material : LocalMaterialsArray)
	{
		if (Material.TableAndRow.DataTable == nullptr) return 0;
		const FItemData* ItemDataFromDT = Material.TableAndRow.DataTable->FindRow<FItemData>(Material.TableAndRow.RowName, "");
		if (ItemDataFromDT == nullptr) return 0;

		FItemData OwnedItemData;
		if (!InventoryComp->GetItemByData(*ItemDataFromDT, OwnedItemData))
		{
			return 0;
		}
        
		int32 CraftableAmountWithThisMaterial = OwnedItemData.Quantity / Material.Quantity;
		MaxPossibleCrafts = FMath::Min(MaxPossibleCrafts, CraftableAmountWithThisMaterial);
	}
    
	return MaxPossibleCrafts == INT32_MAX ? 0 : MaxPossibleCrafts;
}


void UCraftingStationComponent::SortCurrentListAndRefresh(ESortMethod Method)
{
	TArray<FCraftingData> SortedCraftingArray;
	SortCraftingList(Method,CraftingList,SortedCraftingArray);
	CraftingList = SortedCraftingArray;
	OnRefreshed.Broadcast();
}
