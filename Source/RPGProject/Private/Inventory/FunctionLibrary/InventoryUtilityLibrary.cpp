// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"
#include "Components/Image.h"
#include "Interaction/Data/InteractableDefinitions.h"
#include "Inventory/Components/InventoryCoreComponent.h"
#include "Engine/AssetManager.h"
#include "Item/ItemBase.h"
#include "Engine/DataTable.h"
#include "Header/DebugHeader.h"
#include "Interaction/Data/InteractableDefinitions.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Sound/SoundBase.h"

#pragma region Sort


TArray<FItemData> UInventoryUtilityLibrary::SortItemsByType(const TArray<FItemData>& DataArray, const TArray<EItemType>& SortingOrder)
{
	// 타입별 우선순위 맵 생성 (O(m) where m = SortingOrder.Num())
	TMap<EItemType, int32> TypePriority;
	for (int32 i = 0; i < SortingOrder.Num(); ++i)
	{
		TypePriority.Add(SortingOrder[i], i);
	}
    
	// 데이터 복사 및 정렬
	TArray<FItemData> SortedArray = DataArray;
    
	SortedArray.Sort([&TypePriority](const FItemData& A, const FItemData& B)
	{
		if (!IsItemClassValid(A)) return false;
		if (!IsItemClassValid(B)) return true;
        
		UItemBase* ItemA = Cast<UItemBase>(A.ItemClass->GetDefaultObject());
		UItemBase* ItemB = Cast<UItemBase>(B.ItemClass->GetDefaultObject());
        
		if (!ItemA) return false;
		if (!ItemB) return true;
        
		int32 PriorityA = TypePriority.Contains(ItemA->Type) ? 
			TypePriority[ItemA->Type] : MAX_int32;
		int32 PriorityB = TypePriority.Contains(ItemB->Type) ? 
			TypePriority[ItemB->Type] : MAX_int32;
        
		return PriorityA < PriorityB;
	});
    
	return SortedArray;
}

TArray<FItemData> UInventoryUtilityLibrary::SortItemsByRarity(const TArray<FItemData>& DataArray,
	const TArray<EItemRarity>& SortingOrder)
{
	// 희소성별 우선순위 맵
	TMap<EItemRarity, int32> RarityPriority;
	for (int32 i = 0; i < SortingOrder.Num(); ++i)
	{
		RarityPriority.Add(SortingOrder[i], i);
	}
    
	TArray<FItemData> SortedArray = DataArray;
    
	SortedArray.Sort([&RarityPriority](const FItemData& A, const FItemData& B)
	{
		if (!IsItemClassValid(A)) return false;
		if (!IsItemClassValid(B)) return true;
        
		UItemBase* ItemA = Cast<UItemBase>(A.ItemClass->GetDefaultObject());
		UItemBase* ItemB = Cast<UItemBase>(B.ItemClass->GetDefaultObject());
        
		if (!ItemA) return false;
		if (!ItemB) return true;
        
		int32 PriorityA = RarityPriority.Contains(ItemA->Rarity) ? 
			RarityPriority[ItemA->Rarity] : MAX_int32;
		int32 PriorityB = RarityPriority.Contains(ItemB->Rarity) ? 
			RarityPriority[ItemB->Rarity] : MAX_int32;
        
		return PriorityA < PriorityB;
	});
    
	return SortedArray;
}

TArray<FItemData> UInventoryUtilityLibrary::SortItemsByValue(const TArray<FItemData>& DataArray)
{
	TArray<FItemData> SortedArray = DataArray;
    
	SortedArray.Sort([](const FItemData& A, const FItemData& B)
	{
		float ValueA = CalculateStackedItemValue(A);
		float ValueB = CalculateStackedItemValue(B);
		return ValueA > ValueB; // 내림차순
	});
    
	return SortedArray;
}

TArray<FItemData> UInventoryUtilityLibrary::SortItemsByWeight(const TArray<FItemData>& DataArray)
{
	TArray<FItemData> SortedArray = DataArray;
    
	SortedArray.Sort([](const FItemData& A, const FItemData& B)
	{
		float WeightA = CalculateStackedItemWeight(A);
		float WeightB = CalculateStackedItemWeight(B);
		return WeightA < WeightB; // 오름차순
	});
    
	return SortedArray;
}

TArray<FItemData> UInventoryUtilityLibrary::QuickSortItems(const TArray<FItemData>& DataArray)
{
	TArray<FItemData> SortedArray = DataArray;
    
	// 유효한 아이템을 앞으로, 빈 슬롯을 뒤로
	SortedArray.Sort([](const FItemData& A, const FItemData& B)
	{
		bool bAValid = IsItemClassValid(A);
		bool bBValid = IsItemClassValid(B);
        
		if (bAValid && !bBValid) return true;
		if (!bAValid && bBValid) return false;
		if (!bAValid && !bBValid) return false;
        
		// 둘 다 유효한 경우 ID로 정렬
		return A.ID < B.ID;
	});
    
	return SortedArray;
}

TArray<FCraftingData> UInventoryUtilityLibrary::SortCraftingItemsByRarity(const TArray<FCraftingData>& DataArray,
	const TArray<EItemRarity>& SortingOrder)
{
	TArray<FCraftingData> LocalDataArray{};
	
	for(auto Data : LocalDataArray)
	{
		
	}
	
	return LocalDataArray;
}

TArray<FCraftingData> UInventoryUtilityLibrary::SortCraftingItemsByValue(const TArray<FCraftingData>& DataArray)
{
	TArray<FCraftingData> LocalDataArray{};

	for(auto Data : LocalDataArray)
	{
		
	}
	
	return LocalDataArray;
}

TArray<FCraftingData> UInventoryUtilityLibrary::SortCraftingItemsByWeight(const TArray<FCraftingData>& DataArray)
{
	TArray<FCraftingData> LocalDataArray{};
	
	for(auto Data : LocalDataArray)
	{
		
	}
	
	return LocalDataArray;
}

TArray<FCraftingData> UInventoryUtilityLibrary::SortCraftingItemsByType(const TArray<FCraftingData>& DataArray,
	const TArray<EItemType>& SortingOrder)
{
	TArray<FCraftingData> LocalDataArray{};
	
	for(auto Data : LocalDataArray)
	{
		
	}
	
	return LocalDataArray;
}

TArray<FCraftingData> UInventoryUtilityLibrary::SortCraftingItemsBySubcategory(const TArray<FCraftingData>& DataArray,
	const TArray<ECraftingSubcategory>& SortingOrder)
{
	TArray<FCraftingData> LocalDataArray{};

	for (auto Data : LocalDataArray)
	{
	}
    	
	return LocalDataArray;
}

void UInventoryUtilityLibrary::GetCraftableData(const FCraftingData& CraftingData, TArray<FItemData>& OutCraftableItems,
	TArray<FItemStack>& OutMaterials)
{
	TArray<FItemData> LocalCraftableItems{};	
	const TArray<FItemStack>& CraftableItems = CraftingData.OutputItems;

	for(const auto& SingleDTItem : CraftableItems)
	{
		const UDataTable* DT = SingleDTItem.TableAndRow.DataTable;
		FName RowName = SingleDTItem.TableAndRow.RowName;

		if(DT)
		{
			FItemData* FoundItemData = DT->FindRow<FItemData>(RowName, "");

			if (FoundItemData)
			{
				FItemData LocalItemData = *FoundItemData;
				LocalItemData.Quantity = SingleDTItem.Quantity;
				LocalCraftableItems.Add(LocalItemData);
			}
		}
	}
	if(!LocalCraftableItems.IsEmpty())
	{
		OutCraftableItems = LocalCraftableItems;
	}
	
	OutMaterials = CraftingData.InputItems;
}

#pragma endregion

#pragma region Sound

#pragma endregion

#pragma region Leveling

#pragma endregion

#pragma region Crafting

#pragma endregion

#pragma region Function

bool UInventoryUtilityLibrary::AreItemSlotsEqual(const FItemData& FirstItemData,const FItemData& SecondItemData)
{
	TArray<EItemSlot> Slots{EItemSlot::POCKET1,EItemSlot::POCKET2,EItemSlot::POCKET3,EItemSlot::POCKET4};

	EItemSlot FirstItemSlot = FirstItemData.EquipmentSlot;
	EItemSlot SecondItemSlot = SecondItemData.EquipmentSlot;
	
	if((FirstItemSlot == SecondItemSlot) ||
		 (Slots.Contains(FirstItemSlot) && Slots.Contains(SecondItemSlot)))
	{
		return true;	
	}

	return false;
}

// bool UInventoryUtilityLibrary::FindEmptyEquipmentSlot(TMap<EItemSlot,FItemData> EquipmentData,TArray<EItemSlot> SlotsToSearch, EItemSlot DefaultSlot,EItemSlot& OutSlot)
// {
// 	for(auto Slot : SlotsToSearch)
// 	{
// 		const FItemData* FoundItemData = EquipmentData.Find(Slot);
// 		if (!FoundItemData || !IsItemClassValid(*FoundItemData))
// 		{
// 			OutSlot = Slot;
// 			return true;
// 		}
// 	}
// 	
// 	OutSlot = DefaultSlot;
// 	return false;
// }

bool UInventoryUtilityLibrary::AreItemsTheSame(const FItemData& FirstItemData,const FItemData& SecondItemData)
{
	return FirstItemData.ID == SecondItemData.ID;
}

bool UInventoryUtilityLibrary::HasPartialStack(TArray<FItemData>& DataArray,const FItemData& ItemData, int32& SlotIndex)
{
	for (int i = 0; i < DataArray.Num(); ++i)
	{
		FItemData ItemDataInSlot = DataArray[i]; 
		
		if (IsItemClassValid(ItemDataInSlot))
		{
			UItemBase* ItemBaseInSlot = Cast<UItemBase>(ItemDataInSlot.ItemClass->GetDefaultObject());
			UItemBase* ItemBaseToStack = Cast<UItemBase>(ItemData.ItemClass->GetDefaultObject());

			if (!ItemBaseToStack || !ItemBaseInSlot) return false;

			if ((ItemDataInSlot.ID == ItemData.ID) &&
				ItemBaseInSlot->Stackable &&
				ItemBaseToStack->Stackable)
			{
				SlotIndex = i;
				return true;
			}
		}
	}
	return false;
}

bool UInventoryUtilityLibrary::FindEmptySlotInArray(const TArray<FItemData>& DataArray,int32 &OutIndex)
{
	for (int32 i = 0; i < DataArray.Num(); ++i)
	{
		if (!IsItemClassValid(DataArray[i]))
		{
			OutIndex = i;
			return true;
		}
	}
	OutIndex = -1;
	return false;
}

bool UInventoryUtilityLibrary::CanItemBeRemoved(const FItemData& InItemData)
{
	UItemBase* ItemBase = Cast<UItemBase>(InItemData.ItemClass->GetDefaultObject());
	if(ItemBase->RemoveType != EItemRemoveType::CannotBeRemoved)
	{
		return true;
	}
	return false;
}

int32 UInventoryUtilityLibrary::FindItemIndex(TArray<FItemData>& DataArray,const FItemData& InItemData)
{
	int32 LocalIndex = -1;
	
	for (int i = 0; i < DataArray.Num(); ++i)
	{
		FItemData LocalItemData = DataArray[i];
		if(AreItemsTheSame(LocalItemData,InItemData))
		{
			LocalIndex = i;
		}
	}
	return LocalIndex;
}

bool UInventoryUtilityLibrary::FindItemByID(TArray<FItemData>& DataArray, FString ID, FItemData& OutItemData)
{
	for (int i = 0; i < DataArray.Num(); ++i)
	{
		FItemData LocalItemData = DataArray[i];
		
		if(ID == LocalItemData.ID)
		{
			OutItemData = LocalItemData;
			return true;
		}
	}
	return false;
}

bool UInventoryUtilityLibrary::FindAllItemsByID(TArray<FItemData>& DataArray,const FString& ItemID, int32& QuantityFound,
	TArray<FItemData>& OutDataArray)
{
	int32 LocalQuantity = 0;
	TArray<FItemData> LocalDataArray{};
	for (int i = 0; i < DataArray.Num(); ++i)
	{
		FItemData LocalItemData = DataArray[i];
		if(LocalItemData.ID == ItemID)
		{
			int32 ClampQuantity = UKismetMathLibrary::FClamp(LocalItemData.Quantity,1,LocalItemData.Quantity);
			LocalDataArray.Add(LocalItemData);
			LocalQuantity += ClampQuantity;
		}
	}

	if(LocalDataArray.Num() > 0)
	{
		OutDataArray = LocalDataArray;
		QuantityFound = LocalQuantity;
		return true;
	}
	DebugHeader::LogWarning(TEXT("UInventoryUtilityLibrary::FindAllItemsByID : No items found with ID: %s"), *ItemID);
	return false;
}

bool UInventoryUtilityLibrary::IsItemClassValid(const FItemData& InItemData)
{
	UClass* ActualClassPtr = InItemData.ItemClass.Get();;
	if(UKismetSystemLibrary::IsValidClass(ActualClassPtr))
	{
		return true;
	}
	return false;
}

int32 UInventoryUtilityLibrary::FindItemStat(TArray<FItemStatRow>& DataArray, const FGameplayTag& StatTag)
{
	int32 LocalVariable = -1;
	
	for (int i = 0; i < DataArray.Num(); ++i)
	{
		FItemStatRow LocalItemStatRow = DataArray[i];

		if(LocalItemStatRow.StatTag == StatTag)
		{
			LocalVariable = i;
		}
	}
	return LocalVariable;
}



bool UInventoryUtilityLibrary::IsStackableAndHaveStacks(const FItemData& InItemData, int32 Quantity)
{
	if(!InItemData.ItemClass) return false;
	
	UItemBase* ItemBase = Cast<UItemBase>(InItemData.ItemClass->GetDefaultObject());
	if(ItemBase && ItemBase->Stackable && InItemData.Quantity > Quantity)
	{
		return true;
	}
	DebugHeader::LogWarning(TEXT("UInventoryUtilityLibrary::IsStackableAndHaveStacks : Item is not stackble"));
	return false;
}

bool UInventoryUtilityLibrary::AreItemsStackable(const FItemData& FirstItemData,const FItemData& SecondItemData)
{
	UItemBase* FirstItemBase = Cast<UItemBase>(FirstItemData.ItemClass->GetDefaultObject());
	UItemBase* SecondItemBase = Cast<UItemBase>(SecondItemData.ItemClass->GetDefaultObject());
	
	if(FirstItemBase && SecondItemBase && FirstItemBase->Stackable && SecondItemBase->Stackable)
	{
		return true;
	}
	DebugHeader::LogWarning(TEXT("UInventoryUtilityLibrary::AreItemsStackable : Items are not stackble"));
	return false;
}

int32 UInventoryUtilityLibrary::FindAmountOfEmptySlots(TArray<FItemData>& DataArray)
{
	int LocalVariable = 0;
	
	for (int i = 0; i < DataArray.Num(); ++i)
	{
		FItemData LocalItemData = DataArray[i];
		if(!IsItemClassValid(LocalItemData))
		{
			++LocalVariable;
		}
	}
	return LocalVariable;
}

float UInventoryUtilityLibrary::CalculateStackedItemValue(const FItemData& InItemData)
{
	if(!InItemData.ItemClass) 
	{
		DebugHeader::LogWarning(TEXT("CalculateStackedItemValue: ItemClass is null"));
		return 0.f;
	}
    
	UItemBase* ItemBase = Cast<UItemBase>(InItemData.ItemClass->GetDefaultObject());
	if(!ItemBase)
	{
		DebugHeader::LogWarning(TEXT("CalculateStackedItemValue: ItemBase is null for item %s"), *InItemData.ID);
		return 0.f;
	}

	int32 ClampQuantity = FMath::Max(1, InItemData.Quantity); // FClamp 대신 Max 사용이 더 명확
	float StackedValue = ItemBase->Value * InItemData.ValueModifier;
	return ClampQuantity * StackedValue;
}

float UInventoryUtilityLibrary::CalculateStackedItemWeight(const FItemData& InItemData)
{
	if(!InItemData.ItemClass) 
	{
		DebugHeader::LogWarning(TEXT("CalculateStackedItemWeight: ItemClass is null"));
		return 0.f;
	}
    
	UItemBase* ItemBase = Cast<UItemBase>(InItemData.ItemClass->GetDefaultObject());
	if(!ItemBase)
	{
		DebugHeader::LogWarning(TEXT("CalculateStackedItemWeight: ItemBase is null for item %s"), *InItemData.ID);
		return 0.f;
	}
    
	int32 ClampQuantity = FMath::Max(1, InItemData.Quantity);
	float StackedWeight = ItemBase->Weight * InItemData.ValueModifier;
	return ClampQuantity * StackedWeight;
}

bool UInventoryUtilityLibrary::CanWeaponsBeSwapped(const FItemData& FirstItemData,const FItemData& SecondItemData)
{
	if (!FirstItemData.ItemClass || !SecondItemData.ItemClass)
	{
		DebugHeader::LogError(TEXT("CanWeaponsBeSwapped : ItemData`s ItemClass is nullptr"));
		return false;
	}
	
	UItemBase* FirstItemBase = Cast<UItemBase>(FirstItemData.ItemClass->GetDefaultObject());
	UItemBase* SecondItemBase = Cast<UItemBase>(SecondItemData.ItemClass->GetDefaultObject());
	
	if(IsItemClassValid(SecondItemData) && FirstItemBase && SecondItemBase)
	{
		if(FirstItemBase->WeaponType == EWeaponType::OneHand &&
			SecondItemBase->WeaponType == EWeaponType::OneHand)
		{
			return true;
		}
		if(FirstItemBase->WeaponType == EWeaponType::OneHand &&
		(SecondItemData.EquipmentSlot == EItemSlot::WEAPON || SecondItemData.EquipmentSlot == EItemSlot::SHIELD))
		{
			return true;
		}
	}

	if (FirstItemBase->WeaponType == EWeaponType::OneHand &&
		(SecondItemData.EquipmentSlot == EItemSlot::WEAPON || SecondItemData.EquipmentSlot == EItemSlot::SHIELD))
	{
		return true;
	}
	
	return false;
}

bool UInventoryUtilityLibrary::AreWeaponTypesEqual(const FItemData& FirstItemData,const FItemData& SecondItemData)
{
	UItemBase* FirstItemBase = Cast<UItemBase>(FirstItemData.ItemClass->GetDefaultObject());
	UItemBase* SecondItemBase = Cast<UItemBase>(SecondItemData.ItemClass->GetDefaultObject());
	
	if(IsItemClassValid(SecondItemData) && FirstItemBase && SecondItemBase)
	{
		if(FirstItemBase->WeaponType ==	SecondItemBase->WeaponType)
		{
			return true;
		}
	}
	return false;
}

EInventoryPanel UInventoryUtilityLibrary::GetInventoryPanelFromItem(const FItemData& InItemData)
{
	if (!InItemData.ItemClass)
	{
		return EInventoryPanel::NONE;
	}
	
	if(UObject* ItemBaseClass = InItemData.ItemClass->GetDefaultObject())
	{
		UItemBase* ItemBase = Cast<UItemBase>(ItemBaseClass);
		if(nullptr == ItemBase) return EInventoryPanel::NONE;
		return ItemBase->InventoryPanel;
	}
	return EInventoryPanel::NONE;
}

bool UInventoryUtilityLibrary::IsItemCurrency(const FItemData& InItemData)
{
	if (!InItemData.ItemClass)
	{
		return false;
	}
	
	if(UObject* ItemBaseClass = InItemData.ItemClass->GetDefaultObject())
	{
		UItemBase* ItemBase = Cast<UItemBase>(ItemBaseClass);
		if(nullptr == ItemBase) return false;
		return ItemBase->Type == EItemType::Currency;
	}
	return false;
}

bool UInventoryUtilityLibrary::SwitchHasOwnerAuthority(UActorComponent* Component)
{
	if(AActor* Actor = GetOwnerFromComponent(Component))
	{
		// Authority
		if(Actor->HasAuthority())
		{
			return true;
		}
	}
	// Remote
	return false;
}

bool UInventoryUtilityLibrary::IsCraftingDataValid(const FCraftingData& CraftingData)
{
	if (CraftingData.OutputItems.IsValidIndex(0) && CraftingData.InputItems.IsValidIndex(0))
	{
		return true;
	}
	return false;
}

bool UInventoryUtilityLibrary::AreCraftingDataTheSame(const FCraftingData& FirstCraftingData,const FCraftingData& SecondCraftingData)
{
	if (FCString::Strcmp(*FirstCraftingData.CraftingID, *SecondCraftingData.CraftingID))
	{
		return true;
	}
	return false;
}

bool UInventoryUtilityLibrary::GetCraftingDataFromTableRow(FDataTableRowHandle RowHandle,FCraftingData& OutCraftingData)
{
	if(const UDataTable* DataTable = RowHandle.DataTable)
	{
		if(FCraftingData* CraftingData = DataTable->FindRow<FCraftingData>(RowHandle.RowName,""))
		{
			OutCraftingData = *CraftingData;
			return true;
		}
	}
	return false;
}

EOperationStatus UInventoryUtilityLibrary::SetImageTextureFromAssetAsync(UImage* TargetImage, TSoftObjectPtr<UObject> TextureAsset)
{
	if (!TargetImage || TextureAsset.IsNull())
	{
		return EOperationStatus::Failed;
	}

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

	Streamable.RequestAsyncLoad(TextureAsset.ToSoftObjectPath(), FStreamableDelegate::CreateLambda([TargetImage, TextureAsset]()
	{
		UObject* LoadedAsset = TextureAsset.Get();
		if (UTexture2D* Texture = Cast<UTexture2D>(LoadedAsset))
		{
			TargetImage->SetBrushFromTexture(Texture);
		}
	}));
	
	if (TextureAsset.IsValid())
	{
		if (UTexture2D* Texture = Cast<UTexture2D>(TextureAsset.Get()))
		{
			TargetImage->SetBrushFromTexture(Texture);
		}
		return EOperationStatus::Complete;
	}
	return EOperationStatus::Failed;
}

EOperationStatus UInventoryUtilityLibrary::SetItemImageTextureAsync(UImage* TargetImage,const FItemData& InItemData)
{
	if(!InItemData.ItemClass) return EOperationStatus::None;
	
	if(nullptr == TargetImage)
	{
		DebugHeader::LogWarning(TEXT("UInventoryUtilityLibrary::SetItemImageTextureAsync : TargetImage is nullptr"));
		return EOperationStatus::None;
	}

	UObject* ItemClass = InItemData.ItemClass->GetDefaultObject();

	if(nullptr == ItemClass)
	{
		return EOperationStatus::None;
	}
	UItemBase* ItemBase = Cast<UItemBase>(ItemClass);
	
	if(IsItemClassValid(InItemData) && ItemBase)
	{
		if(EOperationStatus::Complete == SetImageTextureFromAssetAsync(TargetImage,ItemBase->Image))
		{
			return EOperationStatus::Complete;
		}
	}
	return EOperationStatus::Failed;
}


void UInventoryUtilityLibrary::SetImageTextureFromAsset(UImage* TargetImage, TSoftObjectPtr<UObject> TextureAsset)
{
	UObject* Object = UKismetSystemLibrary::LoadAsset_Blocking(TextureAsset);
	if(UTexture2D* Texture2D = Cast<UTexture2D>(Object))
	{
		TargetImage->SetBrushFromTexture(Texture2D);
	}
}

TArray<FItemData> UInventoryUtilityLibrary::GetAllItemsOfType(const TArray<FItemData>& DataArray, EItemType Type)
{
	TArray<FItemData> LocalArray{};
	for(auto& Data : DataArray)
	{
		UItemBase* SlotItemBase = Cast<UItemBase>(Data.ItemClass->GetDefaultObject());
		
		if(IsItemClassValid(Data) && SlotItemBase)
		{
			if(SlotItemBase->Type == Type)
			{
				LocalArray.Add(Data);
			}
		}
	}
	return LocalArray;
}

FSlateColor UInventoryUtilityLibrary::GetRarityColor(EItemRarity Rarity)
{
	FLinearColor LinearColor{};
	switch (Rarity)
	{
	case EItemRarity::None:
		LinearColor = FLinearColor{0.f,0.f,0.f,1.f};
		break;
	case EItemRarity::Common:
		LinearColor = FLinearColor{0.775822f,0.775822f,0.775822f,1.f};
		break;
	case EItemRarity::Superior:
		LinearColor = FLinearColor{0.775822f,0.775822f,0.775822f,1.f};
		break;
	case EItemRarity::Epic:
		LinearColor = FLinearColor{0.f,0.411458f,0.155397f,1.f};
		break;
	case EItemRarity::Legendary:
		LinearColor = FLinearColor{0.645833f,0.477914f,0.045227f,1.f};
		break;
	case EItemRarity::Consumable:
		LinearColor = FLinearColor{0.05448f,0.291771f,0.53948f,1.f};
	break;
	case EItemRarity::Usable:
		LinearColor = FLinearColor{0.6875f,0.224721f,0.072573f,1.f};
		break;
	}
	return FSlateColor {LinearColor};	
}


#pragma endregion 


void UInventoryUtilityLibrary::PlaySoundOnOpeningWidget(const UObject* WorldContextObject)
{
	static const TCHAR* SoundPath = TEXT("/Game/Asset/Inventory/Sounds/S_OpenWidget.S_OpenWidget");
	USoundBase* SoundToPlay = LoadObject<USoundBase>(nullptr, SoundPath);
	
	if (!WorldContextObject)
	{
		DebugHeader::LogWarning("UInventoryUtilityLibrary::PlaySoundOnOpeningWidget: WorldContextObject is null.");
		return;
	}

	if (!SoundToPlay)
	{
		DebugHeader::LogWarning("UInventoryUtilityLibrary::PlaySoundOnOpeningWidget: SoundToPlay is null.");
		return;
	}
	UGameplayStatics::PlaySound2D(WorldContextObject, SoundToPlay);
}

void UInventoryUtilityLibrary::PlaySoundOnTabSwitched(const UObject* WorldContextObject)
{
	static const TCHAR* SoundPath = TEXT("/Game/Asset/Inventory/Sounds/S_Switch.S_Switch");
	USoundBase* SoundToPlay = LoadObject<USoundBase>(nullptr, SoundPath);
	
	if (!WorldContextObject)
	{
		DebugHeader::LogWarning("UInventoryUtilityLibrary::PlaySoundOnOpeningWidget: WorldContextObject is null.");
		return;
	}

	if (!SoundToPlay)
	{
		DebugHeader::LogWarning("UInventoryUtilityLibrary::PlaySoundOnOpeningWidget: SoundToPlay is null.");
		return;
	}
	UGameplayStatics::PlaySound2D(WorldContextObject, SoundToPlay);
}
