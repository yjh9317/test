
#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "Inventory/Components/InventoryCoreComponent.h"
#include "Item/Data/ItemDataStructs.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "InventoryUtilityLibrary.generated.h"

enum class EOperationStatus;
class UImage;
class USoundBase;
/**
 * 
 */
UCLASS(Blueprintable)
class RPGPROJECT_API UInventoryUtilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
#pragma region Utility Function
	static bool AreItemSlotsEqual(const FItemData& FirstItemData,const FItemData& SecondItemData);
	// static bool FindEmptyEquipmentSlot(TMap<EItemSlot,FItemData> EquipmentData,TArray<EItemSlot> SlotsToSearch, EItemSlot DefaultSlot = EItemSlot::NONE,EItemSlot& OutSlot);
	static bool AreItemsTheSame(const FItemData& FirstItemData,const FItemData& SecondItemData);
	static bool HasPartialStack(TArray<FItemData>& DataArray,const FItemData& InItemData,int32& SlotIndex);
	static bool FindEmptySlotInArray(const TArray<FItemData>& DataArray,int32 &OutIndex);
	static bool CanItemBeRemoved(const FItemData& InItemData);
	static int32 FindItemIndex(TArray<FItemData>& DataArray,const FItemData& InItemData);
	static bool FindItemByID(TArray<FItemData>& DataArray, FString ID,FItemData& OutItemData);
	static bool FindAllItemsByID(TArray<FItemData>& DataArray,const FString& ItemID,int32& QuantityFound,TArray<FItemData>& OutDataArray);
	static bool IsItemClassValid(const FItemData& InItemData);
	static int32 FindItemStat(TArray<FItemStatRow>& DataArray,const FGameplayTag& StatTag);
	static bool IsStackableAndHaveStacks(const FItemData& InItemData,int32 Quantity);
	static bool AreItemsStackable(const FItemData& FirstItemData,const FItemData& SecondItemData);
	static int32 FindAmountOfEmptySlots(TArray<FItemData>& DataArray);
	static float CalculateStackedItemValue(const FItemData& InItemData);
	static float CalculateStackedItemWeight(const FItemData& InItemData);
	static bool CanWeaponsBeSwapped(const FItemData& FirstItemData,const FItemData& SecondItemData);
	static bool AreWeaponTypesEqual(const FItemData& FirstItemData,const FItemData& SecondItemData);
	static EInventoryPanel GetInventoryPanelFromItem(const FItemData& InItemData);
	static bool IsItemCurrency(const FItemData& InItemData);
	static bool SwitchHasOwnerAuthority(UActorComponent* Component);
	static bool IsCraftingDataValid(const FCraftingData& CraftingData);
	static bool AreCraftingDataTheSame(const FCraftingData& FirstCraftingData,const FCraftingData& SecondCraftingData);
	static bool GetCraftingDataFromTableRow(FDataTableRowHandle RowHandle,FCraftingData& OutCraftingData);
	static EOperationStatus SetImageTextureFromAssetAsync(UImage* TargetImage,TSoftObjectPtr<UObject> TextureAsset);
	static EOperationStatus SetItemImageTextureAsync(UImage* TargetImage, const FItemData& InItemData);
	static void SetImageTextureFromAsset(UImage* TargetImage,TSoftObjectPtr<UObject> TextureAsset);
	static TArray<FItemData> GetAllItemsOfType(const TArray<FItemData>& DataArray,EItemType Type);
	static FSlateColor GetRarityColor(EItemRarity Rarity);	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void PlaySoundOnOpeningWidget(const UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	static void PlaySoundOnTabSwitched(const UObject* WorldContextObject);


#pragma endregion

#pragma region Sort
	static TArray<FItemData> SortItemsByType(const TArray<FItemData>& DataArray,const TArray<EItemType>& SortingOrder);
	static TArray<FItemData> SortItemsByRarity(const TArray<FItemData>& DataArray,const TArray<EItemRarity>& SortingOrder);
	static TArray<FItemData> SortItemsByValue(const TArray<FItemData>& DataArray);
	static TArray<FItemData> SortItemsByWeight(const TArray<FItemData>& DataArray);
	static TArray<FItemData> QuickSortItems(const TArray<FItemData>& DataArray);
	static TArray<FCraftingData> SortCraftingItemsByRarity(const TArray<FCraftingData>& DataArray,const TArray<EItemRarity>& SortingOrder);
	static TArray<FCraftingData> SortCraftingItemsByValue(const TArray<FCraftingData>& DataArray);
	static TArray<FCraftingData> SortCraftingItemsByWeight(const TArray<FCraftingData>& DataArray);
	static TArray<FCraftingData> SortCraftingItemsByType(const TArray<FCraftingData>& DataArray,const TArray<EItemType>& SortingOrder);
	static TArray<FCraftingData> SortCraftingItemsBySubcategory(const TArray<FCraftingData>& DataArray,const TArray<ECraftingSubcategory>& SortingOrder);
#pragma endregion
#pragma region Sound
#pragma endregion
#pragma region Crafting
	static void GetCraftableData(const FCraftingData& CraftingData,TArray<FItemData>& OutCraftableItems,TArray<FItemStack>& OutMaterials);
#pragma endregion

	UFUNCTION(BlueprintCallable, Category = "MyFunctions")
	static AActor* GetOwnerFromComponent(UActorComponent* Component)
	{
		return (Component != nullptr) ? Component->GetOwner() : nullptr;
	}

	template<typename AssetType>
	static TSharedPtr<FStreamableHandle> RequestAsyncLoadItem(
	const TSoftObjectPtr<AssetType>& AssetToLoad,
	TFunction<void(UItemBase*)> OnLoadedCallback
)
	{
		if (AssetToLoad.IsNull())
		{
			return nullptr;
		}

		FSoftObjectPath PathToLoad = AssetToLoad.ToSoftObjectPath();
		FStreamableManager& StreamableManager = UAssetManager::Get().GetStreamableManager();
		
		TSharedPtr<FStreamableHandle> Handle = StreamableManager.RequestAsyncLoad(
		   PathToLoad,
		   [AssetToLoad, OnLoadedCallback, PathToLoad]() mutable {
			  UItemBase* LoadedAsset = AssetToLoad.Get();
			  if (!LoadedAsset)
			  {
				 UE_LOG(LogTemp, Warning, TEXT("AsyncLoadGeneric: Failed to load asset %s or wrong type."), *PathToLoad.ToString());
			  }
			  if (OnLoadedCallback)
			  {
				 OnLoadedCallback(LoadedAsset);
			  }
		   }
		);
		return Handle;
	}
	
};

