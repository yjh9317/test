
#include "Upgrading/Component/UpgradingStation.h"

UUpgradingStation::UUpgradingStation()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UUpgradingStation::BeginPlay()
{
	Super::BeginPlay();

}

void UUpgradingStation::EventTryToCraftCurrentItem()
{
	Super::EventTryToCraftCurrentItem();
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
}

TArray<FCraftingData> UUpgradingStation::GetCurrentItemsRelatedUpgradingList(AActor* OwningPlayer)
{
	return {};
}

TMap<FString, FCraftingData> UUpgradingStation::GetUpgradingListMap()
{
	return {};
}

TArray<FString> UUpgradingStation::GetUpgradingIDsFromItemID(const FString& ID)
{
	return {};
}

TArray<FCraftingData> UUpgradingStation::GetUpgradingDataFromItemID(const FString& ID)
{
	return {};
}

TArray<FCraftingData> UUpgradingStation::GetUpgradingList()
{
	return {};
}

void UUpgradingStation::StopCraftingProcess(const FCraftingData& InCraftingData, AActor* OwningActor)
{
	Super::StopCraftingProcess(InCraftingData, OwningActor);
}

bool UUpgradingStation::HasPlayerEnoughGold(const FCraftingData& InCraftingData, int32 AmountToCraft,
	AActor* OwningActor)
{
	return Super::HasPlayerEnoughGold(InCraftingData, AmountToCraft, OwningActor);
}

void UUpgradingStation::ClearCraftingQueue(AActor* OwningPlayer)
{
	Super::ClearCraftingQueue(OwningPlayer);
}

int32 UUpgradingStation::GetMaxAmountThatCanBeCrafted(const FCraftingData& InCraftingData, AActor* OwningActor)
{
	return Super::GetMaxAmountThatCanBeCrafted(InCraftingData, OwningActor);
}

bool UUpgradingStation::CanInitializeCraftingProcess(const FCraftingData& InCraftingData, int32 AmountToCraft,
	AActor* OwningActor, FText& FailureMessage)
{
	return Super::CanInitializeCraftingProcess(InCraftingData, AmountToCraft, OwningActor, FailureMessage);
}

void UUpgradingStation::SortCurrentListAndRefresh(ESortMethod Method)
{
	Super::SortCurrentListAndRefresh(Method);
}


