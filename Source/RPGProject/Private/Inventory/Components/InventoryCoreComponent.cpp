#include "Inventory/Components/InventoryCoreComponent.h"
#include "GameFramework/PlayerState.h"
#include "Header/DebugHeader.h"
#include "Inventory/Components/InventoryComponent.h"
#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"
#include "Item/ItemBase.h"
#include "Kismet/DataTableFunctionLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"

UInventoryCoreComponent::UInventoryCoreComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = false;
}

void UInventoryCoreComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeInventory(nullptr);
}

void UInventoryCoreComponent::InitializeInventory(APlayerController* PC)
{
	InventoryPanelArray.SetNum(InventoryPanelSize);
	
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		for (const auto& Panel : PanelsToUse)
		{
			BuildInventory(Panel);
		}
		BuildInitialInventory();
	}
}


void UInventoryCoreComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

#pragma region BuildInventory

void UInventoryCoreComponent::BuildInventory(EInventoryPanel Panel)
{
	TArray<FItemData> LocalInventory{};
	int32 InventorySize = 0;
	GetInventoryAndSize(Panel, LocalInventory, InventorySize);
	
	if (UseInventorySize)
	{
		LocalInventory.SetNum(InventoryDefaultSize);

		for (int i = 0; i < InventoryDefaultSize - 1; ++i)
		{
			LocalInventory[i].IsEquipped = false;
			LocalInventory[i].Index = i;
		}
		ApplyChangesToInventoryArray(Panel, LocalInventory);
	}
	else
	{
		TArray<FItemData> NewInventory{};
		for (int i = 0; i < InventorySize; ++i)
		{
			if(!LocalInventory.IsValidIndex(i)) continue;
			FItemData LocalItemData = LocalInventory[i];
			if (UInventoryUtilityLibrary::IsItemClassValid(LocalItemData))
			{
				LocalItemData.IsEquipped = false;
				NewInventory.Add(LocalItemData);
			}
		}
		ApplyChangesToInventoryArray(Panel, NewInventory);
	}
}

void UInventoryCoreComponent::BuildInitialInventory()
{
	PopulateInventory();
	AddInventoryRow();
	RandomizeInitialItems();

	AllItemsFromDataTables.Empty();
	SingleDTItems.Empty();
	RandomizedItemsData.Empty();

	if (SortInitialItems)
	{
		SortInventory(DefaultSortingMethod, EInventoryPanel::P1, false);
	}
}

#pragma endregion

#pragma region Function

void UInventoryCoreComponent::ApplyChangesToInventoryArray(EInventoryPanel Panel,const TArray<FItemData>& Inventory)
{
	if(InventoryPanelArray.Num() <= 0)
	{
		return;
	}
	
	if (PanelsToUse.Contains(Panel))
	{
		switch (Panel)
		{
		default:
		case EInventoryPanel::P1:
			InventoryPanelArray[0] = Inventory;
			InventoryArrayForDebug = InventoryPanelArray[0];
			break;
		case EInventoryPanel::P2:
			InventoryPanelArray[1] = Inventory;
			InventoryArrayForDebug = InventoryPanelArray[1];
			break;
		case EInventoryPanel::P3:
			InventoryPanelArray[2] = Inventory;
			InventoryArrayForDebug = InventoryPanelArray[2];
			break;
		case EInventoryPanel::P4:
			InventoryPanelArray[3] = Inventory;
			InventoryArrayForDebug = InventoryPanelArray[3];
			break;
		}
	}
}

bool UInventoryCoreComponent::TransferItemFromInventory(UInventoryCoreComponent* Sender, FItemData& ItemData,
                                                        const FItemData& InSlotData, EInputMethod Method, AActor* OwningPlayer)
{
	// check gold
	if (UInventoryUtilityLibrary::IsItemClassValid(ItemData) && Sender &&
		(!Sender->CheckOwnerGold || HasEnoughGold(ItemData)))
	{
		// get inventory
		EInventoryPanel LocalPanel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(ItemData);
		TArray<FItemData> LocalInventory{};
		int32 InventorySize = 0;
		GetInventoryAndSize(LocalPanel, LocalInventory, InventorySize);

		// check stack and add
		int32 SlotIndex = 0;
		if (UInventoryUtilityLibrary::HasPartialStack(LocalInventory, ItemData, SlotIndex))
		{
			AddToStackInInventory(ItemData, SlotIndex);
		}
		else
		{
			int32 LocalEmptySlotIndex = -1;
			switch (Method)
			{
			case EInputMethod::RightClick:
				if (GetEmptyInventorySlot(ItemData,LocalEmptySlotIndex))
				{
					AddItemToInventoryArray(ItemData,LocalEmptySlotIndex);
				}
				else
				{
					return false;
				}
				break;
			case EInputMethod::DragAndDrop:
				AddItemToInventoryArray(ItemData, InSlotData.Index);
				break;
			}
		}

		// add item weight
		float ItemWeight = UInventoryUtilityLibrary::CalculateStackedItemWeight(ItemData);
		AddWeightToInventory(ItemWeight);

		// Remove gold And Add Gold To Owner
		if (Sender->CheckOwnerGold && CheckOwnerGold)
		{
			float ItemGold = UInventoryUtilityLibrary::CalculateStackedItemValue(ItemData);
			RemoveGoldFromOwner(ItemGold);
			Sender->AddGoldToOwner(ItemGold);
		}

		Sender->RemoveItemFromInventoryArray(ItemData);
		Sender->RemoveWeightFromInventory(ItemWeight);
		return true;
	}
	return false;
}

bool UInventoryCoreComponent::TransferItemFromEquipment(UEquipmentComponent* Sender, FItemData ItemData,
                                                        FItemData InSlotData, EInputMethod Method)
{
	if (UInventoryUtilityLibrary::IsItemClassValid(ItemData) && Sender)
	{
	}
	return false;
}

void UInventoryCoreComponent::SortByMethod(ESortMethod Method, EInventoryPanel SinglePanel,
                                           TArray<FItemData> LocalInventory)
{
	switch (Method)
	{
	case ESortMethod::Quicksort:
		UInventoryUtilityLibrary::QuickSortItems(LocalInventory);
		break;
	case ESortMethod::ByRarity:
		UInventoryUtilityLibrary::SortItemsByRarity(LocalInventory, CanSortRarities);
		break;
	case ESortMethod::ByType:
		UInventoryUtilityLibrary::SortItemsByType(LocalInventory, CanSortTypes);
		break;
	case ESortMethod::ByValue:
		UInventoryUtilityLibrary::SortItemsByValue(LocalInventory);
		break;
	case ESortMethod::ByWeight:
		UInventoryUtilityLibrary::SortItemsByWeight(LocalInventory);
		break;
	default:
		break;
	}

	// Apply And Update(Widget)
	ApplyChangesToInventoryArray(SinglePanel, LocalInventory);
	BuildInventory(SinglePanel);
	OnRefreshInventory.Broadcast(SinglePanel);

	// Client
	if (!UKismetSystemLibrary::IsStandalone(GetWorld()))
	{
		UpdateViewersInventory(SinglePanel);
		Client_UpdateItems(this, SinglePanel, LocalInventory);
	}
}

void UInventoryCoreComponent::SortInventory(ESortMethod Method, EInventoryPanel SinglePanel, bool bEveryPanel)
{
	TArray<FItemData> LocalInventory{};
	int32 InventorySize = 0;

	if (bEveryPanel)
	{
		for (const auto& Panel : PanelsToUse)
		{
			GetInventoryAndSize(Panel, LocalInventory, InventorySize);
			SortByMethod(Method, Panel, LocalInventory);
		}
		LocalInventory.Empty();
	}
	else
	{
		SortByMethod(Method, SinglePanel, LocalInventory);
	}
}

void UInventoryCoreComponent::SwapItemsInInventory(FItemData FirstItemData, FItemData SecondItemData)
{
	if (UInventoryUtilityLibrary::SwitchHasOwnerAuthority(this) &&
		UInventoryUtilityLibrary::IsItemClassValid(FirstItemData) &&
		UInventoryUtilityLibrary::IsItemClassValid(SecondItemData))
	{
		if (UInventoryUtilityLibrary::AreItemsTheSame(FirstItemData, SecondItemData) &&
			UInventoryUtilityLibrary::AreItemsStackable(FirstItemData, SecondItemData)) // Stack 
		{
			RemoveItemFromInventoryArray(FirstItemData);
			AddToStackInInventory(FirstItemData, SecondItemData.Index);
		}
		else // Swap
		{
			EInventoryPanel FirstItemPanel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(FirstItemData);
			TArray<FItemData> LocalInventory{};
			int32 InventorySize = 0;
			GetInventoryAndSize(FirstItemPanel, LocalInventory, InventorySize);

			// swap index
			int32 FirstItemIndex = FirstItemData.Index;
			int32 SecondItemIndex = SecondItemData.Index;

			if(!LocalInventory.IsValidIndex(FirstItemIndex) || !LocalInventory.IsValidIndex(SecondItemIndex))
			{
				DebugHeader::LogWarning(TEXT("SwapItemsInInventory : not valid index"));
				return;
			}

			FirstItemData.IsEquipped = false;
			FirstItemData.Index = SecondItemIndex;
			LocalInventory[SecondItemIndex] = FirstItemData;
			
			SecondItemData.IsEquipped = false;
			SecondItemData.Index = FirstItemIndex;
			LocalInventory[FirstItemIndex] = SecondItemData;
			ApplyChangesToInventoryArray(FirstItemPanel, LocalInventory);
			OnRefreshInventory.Broadcast(FirstItemPanel);


			// Client
			if (!UKismetSystemLibrary::IsStandalone(GetWorld()))
			{
				UpdateViewersInventory(FirstItemPanel);
			}
		}
	}
}

void UInventoryCoreComponent::MoveItemInInventory(FItemData SourceItemData, int32 TargetIndex)
{
	if (UInventoryUtilityLibrary::SwitchHasOwnerAuthority(this) &&
		UInventoryUtilityLibrary::IsItemClassValid(SourceItemData))
	{
		EInventoryPanel LocalPanel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(SourceItemData);
		TArray<FItemData> LocalInventory{};
		int32 InventorySize = 0;
		GetInventoryAndSize(LocalPanel, LocalInventory, InventorySize);

		int32 SourceItemIndex  = SourceItemData.Index;

		SourceItemData.IsEquipped = false;
		SourceItemData.Index = TargetIndex;
		LocalInventory[TargetIndex] = SourceItemData;

		LocalInventory[SourceItemIndex] = FItemData();
		LocalInventory[SourceItemIndex].Index = SourceItemIndex;
		
		ApplyChangesToInventoryArray(LocalPanel, LocalInventory);
		OnRefreshInventory.Broadcast(LocalPanel);

		if (!UKismetSystemLibrary::IsStandalone(GetWorld()))
		{
			UpdateViewersInventory(LocalPanel);
		}
	}
}


void UInventoryCoreComponent::RandomizeInitialItems()
{
	TArray<FItemData> DrawnItems{};
	TArray<FItemData> NotDrawnItems{};

	for (const auto& LootTable : RandomizedItemsData)
	{
		DrawnItems.Empty();
		NotDrawnItems.Empty();
		UDataTable* DT = LootTable.DataTable;
		TArray<FName> RowNames = DT->GetRowNames();

		for (auto& Name : RowNames)
		{
			FItemData FoundItemData = *DT->FindRow<FItemData>(Name, "");

			if (LootTable.MaxLootItems > DrawnItems.Num()) break;

			UItemBase* FoundItemBase = Cast<UItemBase>(FoundItemData.ItemClass->GetDefaultObject());
			if (nullptr == FoundItemBase)
			{
				DebugHeader::LogWarning("UInventoryCoreComponent::RandomizeInitialItems : FoundItemBase is nullptr");
				continue;
			}

			bool IsDrop = FoundItemBase->DropPercentage >= UKismetMathLibrary::RandomIntegerInRange(0, 100);
			if (UInventoryUtilityLibrary::IsItemClassValid(FoundItemData) && IsDrop)
			{
				DrawnItems.Add(FoundItemData);
				FItemData ChangedItemData;
				if(RandomizeItemParameters(FoundItemData,ChangedItemData))
				{
					int32 LocalEmptySlotIndex = -1;
					if (GetEmptyInventorySlot(ChangedItemData,LocalEmptySlotIndex))
					{
						AddItemToInventoryArray(ChangedItemData, LocalEmptySlotIndex);
						float Weight = UInventoryUtilityLibrary::CalculateStackedItemWeight(ChangedItemData);
						AddWeightToInventory(Weight);
					}
				}
			}
			else
			{
				NotDrawnItems.Add(FoundItemData);
			}
		}

		if (DrawnItems.Num() < LootTable.MaxLootItems)
		{
			int32 RemainingLootItems = LootTable.MaxLootItems - DrawnItems.Num();
			for (int i = 1; i < RemainingLootItems; ++i)
			{
				int32 LastIndex = NotDrawnItems.Num() - 1;
				if (LastIndex >= 0)
				{
					int32 RandomInt = UKismetMathLibrary::RandomIntegerInRange(0, LastIndex);
					FItemData DrawnItemData = NotDrawnItems[RandomInt];
					NotDrawnItems.Remove(DrawnItemData);
					FItemData ChangedItemData ={};
					if(RandomizeItemParameters(DrawnItemData,ChangedItemData))
					{
						int32 LocalEmptySlotIndex = -1;
						if (GetEmptyInventorySlot(ChangedItemData,LocalEmptySlotIndex))
						{
							AddItemToInventoryArray(ChangedItemData, LocalEmptySlotIndex);
							float Weight = UInventoryUtilityLibrary::CalculateStackedItemWeight(ChangedItemData);
							AddWeightToInventory(Weight);
						}
					}
				}
			}
		}
	}
	
}

bool UInventoryCoreComponent::RandomizeItemParameters(const FItemData& InItemData,FItemData& OutItemData)
{
	if (!InItemData.ItemClass)
	{
		return false;
	}

	UItemBase* ItemBase = Cast<UItemBase>(InItemData.ItemClass->GetDefaultObject());
	if (nullptr == ItemBase)
	{
		return false;
	}

	TSet<TSubclassOf<UItemBase>> LootClassesSet = ItemBase->LootItemClass;
	TArray<TSubclassOf<UItemBase>> LootClassesArray;
	for (const auto& Item : LootClassesSet)
	{
		LootClassesArray.Add(Item);
	}

	int32 LastIndex = LootClassesArray.Num() - 1;
	int32 RandomInt = UKismetMathLibrary::RandomIntegerInRange(0, LastIndex);
	if(LootClassesArray.IsValidIndex(RandomInt))
	{
		OutItemData.ItemClass = LootClassesArray[RandomInt];

		int32 RandomQuantity = UKismetMathLibrary::RandomIntegerInRange(ItemBase->MinRandQuantity,
																		ItemBase->MaxRandQuantity);
		int32 ClampedQuantity = UKismetMathLibrary::Clamp(RandomQuantity, 1, RandomQuantity);
		OutItemData.Quantity = ClampedQuantity;

		int32 RandomDurability = UKismetMathLibrary::RandomIntegerInRange(ItemBase->MinRandDurability,
																		  ItemBase->MaxRandDurability);
		int32 ClampedDurability = UKismetMathLibrary::Clamp(RandomDurability, 1, 100);
		OutItemData.Durability = ClampedDurability;
	}
	return true;
}

void UInventoryCoreComponent::SplitItemsInInventory(UInventoryCoreComponent* Sender, FItemData ItemData,
                                                    const FItemData& InSlotData, FItemData StackableLeft, EInputMethod Method,
                                                    EItemDestination Initiator,
                                                    EItemDestination Destination, AActor* OwningPlayer)
{
	bool IsTransfer = TransferItemFromInventory(Sender, ItemData, InSlotData, Method, OwningPlayer);
	FString Message{};
	Sender->Client_TransferItemReturnValue(IsTransfer, Message);

	if (IsTransfer && UInventoryUtilityLibrary::IsStackableAndHaveStacks(StackableLeft, 0))
	{
		Sender->AddItemToInventoryArray(StackableLeft, StackableLeft.Index);
	}
}

void UInventoryCoreComponent::ConfirmationPopupAccepted(UInventoryCoreComponent* Sender, FItemData ItemData,
                                                        const FItemData& InSlotData, EInputMethod Method,
                                                        EItemDestination Initiator, EItemDestination Destination,
                                                        AActor* OwningPlayer)
{
	if (!UInventoryUtilityLibrary::IsStackableAndHaveStacks(ItemData, 1))
	{
		bool IsTransfer = TransferItemFromInventory(Sender, ItemData, InSlotData, Method, OwningPlayer);
		FString Message{};
		Sender->Client_TransferItemReturnValue(IsTransfer, Message);
	}
}

void UInventoryCoreComponent::SwitchActivePanel(EInventoryPanel Panel)
{
	if (Panel != ActivePanel && PanelsToUse.Contains(Panel))
	{
		ActivePanel = Panel;
		OnSwitchedActivePanel.Broadcast(ActivePanel);
	}
}

#pragma endregion

#pragma region Set

void UInventoryCoreComponent::AddItemToInventoryArray(FItemData& InItemData, int32 SlotIndex)
{
	if (!UInventoryUtilityLibrary::SwitchHasOwnerAuthority(this)) return;

	ModifyItemValue(InItemData);
	EInventoryPanel LocalPanel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(InItemData);

	TArray<FItemData> LocalInventory{};
	int32 InventorySize = 0;
	GetInventoryAndSize(LocalPanel, LocalInventory, InventorySize);

	if (UseInventorySize || SlotIndex >= 0)
	{
		InItemData.Index = SlotIndex;
		if (LocalInventory.IsValidIndex(SlotIndex))
		{
			LocalInventory[SlotIndex] = InItemData;
			ApplyChangesToInventoryArray(LocalPanel, LocalInventory);
			RefreshInventoryOnAdd(InItemData, SlotIndex, LocalPanel);
		}
	}
	else
	{
		if (UInventoryUtilityLibrary::IsItemClassValid(InItemData))
		{
			InItemData.IsEquipped = false;
			LocalInventory.Add(InItemData);
			ApplyChangesToInventoryArray(LocalPanel, LocalInventory);
			RefreshInventoryOnAdd(InItemData, SlotIndex, LocalPanel);
		}
	}
}

void UInventoryCoreComponent::AddToStackInInventory(FItemData& InItemData, int32 SlotIndex)
{
	InItemData.Index = SlotIndex;
	EInventoryPanel Panel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(InItemData);

	FItemData LocalItemData = {};
	if(GetItemBySlot(Panel,SlotIndex,LocalItemData))
	{
		LocalItemData.Quantity += InItemData.Quantity;
		AddItemToInventoryArray(LocalItemData, SlotIndex);
	}
}

void UInventoryCoreComponent::AddGoldToOwner(float Gold)
{
	if (CheckOwnerGold)
	{
		float NewGold = FMath::Clamp(OwnerGold + Gold, 0, OwnerGold + Gold);
		Multicast_SetOwnerGold(NewGold);
	}
}

void UInventoryCoreComponent::AddWeightToInventory(float Weight)
{
	if (UseInventoryWeight)
	{
		float NewWeight = FMath::Clamp(CurrentInventoryWeight + Weight, 0, CurrentInventoryWeight + Weight);
		Multicast_SetCurrentWeight(NewWeight);
	}
}

bool UInventoryCoreComponent::AddItemWithSpecifiedQuantity(FItemData InItemData, int32 Quantity)
{
	float ClampedQuantity = FMath::Clamp(Quantity, 1, Quantity);
	InItemData.Quantity = ClampedQuantity;

	if (UInventoryUtilityLibrary::IsItemClassValid(InItemData))
	{
		EInventoryPanel LocalPanel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(InItemData);
		TArray<FItemData> LocalInventory{};
		int32 InventorySize = 0;
		GetInventoryAndSize(LocalPanel, LocalInventory, InventorySize);

		int32 SlotIndex = 0;
		if (UInventoryUtilityLibrary::HasPartialStack(LocalInventory, InItemData, SlotIndex))
		{
			AddToStackInInventory(InItemData, SlotIndex);
			float Weight = UInventoryUtilityLibrary::CalculateStackedItemWeight(InItemData);
			AddWeightToInventory(Weight);
			return true;
		}

		int32 LocalEmptySlotIndex = -1;
		if (GetEmptyInventorySlot(InItemData,LocalEmptySlotIndex))
		{
			AddItemToInventoryArray(InItemData, LocalEmptySlotIndex);
			float Weight = UInventoryUtilityLibrary::CalculateStackedItemWeight(InItemData);
			AddWeightToInventory(Weight);
			return true;
		}

		return false;
	}
	return false;
}

void UInventoryCoreComponent::RemoveItemFromInventoryArray(const FItemData& InItemData)
{
	if (UInventoryUtilityLibrary::SwitchHasOwnerAuthority(this))
	{
		EInventoryPanel Panel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(InItemData);

		TArray<FItemData> LocalInventory{};
		int32 LocalSize = 0;
		GetInventoryAndSize(Panel, LocalInventory, LocalSize);

		if (UseInventorySize)
		{
			FItemData LocalItemData{};
			int32 ItemIndex = InItemData.Index;
			LocalItemData.Index = ItemIndex;

			if (LocalInventory.IsValidIndex(ItemIndex))
			{
				LocalInventory[ItemIndex] = LocalItemData;
			}
		}
		else
		{
			LocalInventory.Remove(InItemData);
		}

		ApplyChangesToInventoryArray(Panel, LocalInventory);

		if (UKismetSystemLibrary::IsStandalone(GetWorld()))
		{
			//Server
			OnRemovedFromInventoryArray.Broadcast(InItemData);
			OnRefreshInventory.Broadcast(Panel);
		}
		else
		{
			//Client
			UpdateViewersItem(InItemData, true);
			Client_UpdateRemovedItem(this, InItemData);
		}
	}
}

void UInventoryCoreComponent::RemoveGoldFromOwner(float Gold)
{
	if (CheckOwnerGold)
	{
		float NewGold = FMath::Clamp(OwnerGold - Gold, 0, OwnerGold - Gold);
		Multicast_SetOwnerGold(NewGold);
	}
}

void UInventoryCoreComponent::RemoveWeightFromInventory(float Weight)
{
	if (UseInventoryWeight)
	{
		float NewWeight = FMath::Clamp(CurrentInventoryWeight - Weight, 0, CurrentInventoryWeight - Weight);
		Multicast_SetCurrentWeight(NewWeight);
	}
}

void UInventoryCoreComponent::RemoveItemQuantity(FItemData InItemData, int32 RemoveQuantity)
{
	int32 ItemQuantity = InItemData.Quantity;

	if (ItemQuantity > RemoveQuantity)
	{
		float Weight = UInventoryUtilityLibrary::CalculateStackedItemWeight(InItemData);
		RemoveWeightFromInventory(Weight);

		float RemainingQuantity = ItemQuantity - RemoveQuantity;
		InItemData.Quantity = RemainingQuantity;

		Weight = UInventoryUtilityLibrary::CalculateStackedItemWeight(InItemData);
		AddItemToInventoryArray(InItemData, InItemData.Index);
		AddWeightToInventory(Weight);
	}
	else
	{
		RemoveItemFromInventoryArray(InItemData);
		float Weight = UInventoryUtilityLibrary::CalculateStackedItemWeight(InItemData);
		RemoveWeightFromInventory(Weight);
	}
}

void UInventoryCoreComponent::ModifyItemValue(FItemData& InItemData) const
{
	InItemData.ValueModifier = ValueModifier;
}

#pragma endregion

#pragma region Get

bool UInventoryCoreComponent::GetEmptyInventorySlot(const FItemData& InItemData,int32& OutEmptySlot)
{
	if (UseInventorySize)
	{
		EInventoryPanel LocalPanel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(InItemData);
		TArray<FItemData> LocalInventory{};
		int32 InventorySize = 0;
		GetInventoryAndSize(LocalPanel, LocalInventory, InventorySize);
		
		int32 LocalEmptySlotIndex = 0;
		if(UInventoryUtilityLibrary::FindEmptySlotInArray(LocalInventory, LocalEmptySlotIndex))
		{
			OutEmptySlot = LocalEmptySlotIndex;
			return true;
		}
		else
		{
			OutEmptySlot = 0;
			return false;	
		}
	}
	OutEmptySlot = -1;
	return true;
}

bool UInventoryCoreComponent::GetItemBySlot(EInventoryPanel Panel, int32 InSlotIndex, FItemData& OutItemData) const
{
	TArray<FItemData> LocalInventory{};
	int32 InventorySize = 0;
	GetInventoryAndSize(Panel, LocalInventory, InventorySize);

	if (LocalInventory.IsValidIndex(InSlotIndex))
	{
		OutItemData = LocalInventory[InSlotIndex];
		return true;
	}
	return false;
}

bool UInventoryCoreComponent::HasEnoughGold(const FItemData& InItemData) const
{
	if (CheckOwnerGold)
	{
		return OwnerGold >= UInventoryUtilityLibrary::CalculateStackedItemValue(InItemData);
	}
	return true;
}

bool UInventoryCoreComponent::IsInventoryOverweight() const
{
	if (UseInventoryWeight)
	{
		return CurrentInventoryWeight > MaxInventoryWeight;
	}
	return false;
}

EItemRemoveType UInventoryCoreComponent::GetItemRemoveType(const FItemData& InItemData) const
{
	UItemBase* ItemBase = Cast<UItemBase>(InItemData.ItemClass->GetDefaultObject());
	if (CheckRemoveType && ItemBase)
	{
		return ItemBase->RemoveType;
	}
	return EItemRemoveType::Default;
}

bool UInventoryCoreComponent::GetItemByID(FString ItemID, EInventoryPanel InSpecifiedPanel, FItemData& OutItemData) const
{
	if (InSpecifiedPanel != EInventoryPanel::NONE)
	{
		TArray<FItemData> LocalInventory{};
		int32 InventorySize = 0;
		GetInventoryAndSize(InSpecifiedPanel, LocalInventory, InventorySize);

		if (UInventoryUtilityLibrary::FindItemByID(LocalInventory, ItemID, OutItemData))
		{
			return true;
		}
		return false;
	}

	UEnum* EnumPtr = StaticEnum<EInventoryPanel>();
	if (EnumPtr)
	{
		TArray<EInventoryPanel> EnumArray;
		int32 NumEnums = EnumPtr->NumEnums();
		for (int32 i = 1; i < NumEnums; ++i) // 0은 None
		{
			EnumArray.Add(static_cast<EInventoryPanel>(EnumPtr->GetValueByIndex(i)));
		}

		for (auto Enum : EnumArray)
		{
			TArray<FItemData> CurInventory{};
			int32 CurInventorySize = 0;
			GetInventoryAndSize(Enum, CurInventory, CurInventorySize);

			if (UInventoryUtilityLibrary::FindItemByID(CurInventory, ItemID, OutItemData))
			{
				if (UInventoryUtilityLibrary::IsItemClassValid(OutItemData))
				{
					return true;
				}
			}
		}
	}
	return false;
}

bool UInventoryCoreComponent::GetAllItemByID(FString ItemID, EInventoryPanel InSpecifiedPanel,TArray<FItemData>& OutItemDataArray) const
{
	TArray<FItemData> LocalInventory{};
	int32 InventorySize = 0;
	GetInventoryAndSize(InSpecifiedPanel, LocalInventory, InventorySize);

	int32 LocalQuantity = 0;
	TArray<FItemData> DataArray;
	if (UInventoryUtilityLibrary::FindAllItemsByID(LocalInventory, ItemID, LocalQuantity, DataArray))
	{
		return true;
	}
	return false;
}

bool UInventoryCoreComponent::GetItemsQuantityInInventory(FItemData InItemData, int32& ItemsQuantity) const
{
	if (UInventoryUtilityLibrary::IsItemClassValid(InItemData))
	{
		EInventoryPanel LocalPanel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(InItemData);
		TArray<FItemData> LocalInventory{};
		int32 InventorySize = 0;
		GetInventoryAndSize(LocalPanel, LocalInventory, InventorySize);

		int32 LocalQuantity = 0;
		TArray<FItemData> DataArray;
		if (UInventoryUtilityLibrary::FindAllItemsByID(LocalInventory, InItemData.ID, LocalQuantity, DataArray))
		{
			ItemsQuantity = LocalQuantity;
			return true;
		}
	}
	return false;
}

TArray<FItemData> UInventoryCoreComponent::GetCombinedInventories() const
{
	TArray<FItemData> LocalInventory{};
	for (const auto& CurInventory : InventoryPanelArray)
	{
		LocalInventory.Append(CurInventory);
	}
	return LocalInventory;
}

bool UInventoryCoreComponent::GetItemByData(const FItemData& InItemData, FItemData& OutItemData) const
{
	EInventoryPanel LocalPanel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(InItemData);

	TArray<FItemData> LocalInventory{};
	int32 InventorySize = 0;
	GetInventoryAndSize(LocalPanel, LocalInventory, InventorySize);

	if (int32 Index = UInventoryUtilityLibrary::FindItemIndex(LocalInventory, InItemData))
	{
		OutItemData = LocalInventory[Index];
		return true;
	}
	return false;
}

TArray<FItemData> UInventoryCoreComponent::GetItemsOfSpecifiedType(EItemType Type) const
{
	TArray<FItemData> CombinedInventories = GetCombinedInventories();
	return UInventoryUtilityLibrary::GetAllItemsOfType(CombinedInventories, Type);
}

int32 UInventoryCoreComponent::GetAmountOfEmptySlots(EInventoryPanel Panel) const
{
	TArray<FItemData> LocalInventory{};
	int32 InventorySize = 0;
	GetInventoryAndSize(Panel, LocalInventory, InventorySize);

	return UInventoryUtilityLibrary::FindAmountOfEmptySlots(LocalInventory);
}


void UInventoryCoreComponent::GetInventoryAndSize(EInventoryPanel Panel, TArray<FItemData>& OutInventory, int32& OutSize) const
{
	OutInventory.Empty();
	OutSize = 0;

	int32 PanelIndex = GetIndexByEnum(Panel);
	if (PanelIndex < 0 || !InventoryPanelArray.IsValidIndex(PanelIndex) || !PanelsToUse.Contains(Panel))
	{
		UE_LOG(LogTemp,Warning,TEXT("GetInventoryAndSize : %d"),PanelIndex);
		return;
	}
	OutInventory = InventoryPanelArray[PanelIndex];
	OutSize = OutInventory.Num();
}

int UInventoryCoreComponent::GetIndexByEnum(EInventoryPanel Panel) const
{
	if (Panel == EInventoryPanel::NONE)
	{
		return -1;
	}

	const int32 Index = static_cast<int32>(Panel);

	if (InventoryPanelArray.IsValidIndex(Index))
	{
		return Index;
	}

	return -1;
}

EInventoryPanel UInventoryCoreComponent::GetEnumByIndex(int32 Index) const
{
	const int32 EnumCount = StaticEnum<EInventoryPanel>()->NumEnums() - 1;

	if (Index < 0 || Index >= EnumCount)
	{
		return EInventoryPanel::NONE;
	}

	return static_cast<EInventoryPanel>(Index);
}

bool UInventoryCoreComponent::GetInventoryPanel(int32 Index, TArray<FItemData>& ItemArray) const
{
	if (InventoryPanelArray.IsValidIndex(Index))
	{
		ItemArray = InventoryPanelArray[Index];
		return true;
	}
	return false;
}

#pragma endregion

#pragma region NetworkFunction

void UInventoryCoreComponent::UpdateViewersInventory(EInventoryPanel Panel)
{
	for (const auto& Viewer : CurrentViewers)
	{
		TArray<FItemData> LocalInventory{};
		int32 InventorySize = 0;
		GetInventoryAndSize(Panel, LocalInventory, InventorySize);

		if (UInventoryComponent* InventoryComponent = Viewer->GetOwner()->FindComponentByClass<UInventoryComponent>())
		{
			GetInventoryAndSize(Panel, LocalInventory, InventorySize);
			InventoryComponent->Client_UpdateItems(this, Panel, LocalInventory);
		}
	}
}

void UInventoryCoreComponent::UpdateAddedItemInInventory(UInventoryCoreComponent* Inventory,const FItemData& InItemData)
{
	if (nullptr == Inventory) return;

	EInventoryPanel LocalPanel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(InItemData);
	TArray<FItemData> LocalInventory{};
	int32 InventorySize = 0;
	Inventory->GetInventoryAndSize(LocalPanel, LocalInventory, InventorySize);

	if (Inventory->UseInventorySize)
	{
		if (LocalInventory.IsValidIndex(InItemData.Index))
		{
			LocalInventory[InItemData.Index] = InItemData;
		}

		Inventory->ApplyChangesToInventoryArray(LocalPanel, LocalInventory);
		SwitchActivePanel(LocalPanel);
		Inventory->OnRefreshInventory.Broadcast(LocalPanel);
		Inventory->OnAddedToInventoryArray.Broadcast(InItemData);
		Inventory->OnHighlightInventorySlot.Broadcast(InItemData.Index);
	}
	else
	{
		LocalInventory.Add(InItemData);

		Inventory->ApplyChangesToInventoryArray(LocalPanel, LocalInventory);
		SwitchActivePanel(LocalPanel);
		Inventory->OnRefreshInventory.Broadcast(LocalPanel);
		Inventory->OnAddedToInventoryArray.Broadcast(InItemData);
		Inventory->OnHighlightInventorySlot.Broadcast(InItemData.Index);
	}
}

void UInventoryCoreComponent::UpdateViewersItem(FItemData ItemData, bool Remove)
{
	for (const auto& Viewer : CurrentViewers)
	{
		if (UInventoryComponent* InventoryComponent = Viewer->GetOwner()->FindComponentByClass<UInventoryComponent>())
		{
			if (Remove)
			{
				InventoryComponent->Client_UpdateRemovedItem(this, ItemData);
			}
			else
			{
				InventoryComponent->Client_UpdateAddedItem(this, ItemData);
			}
		}
	}
}

void UInventoryCoreComponent::UpdateRemovedItemInInventory(UInventoryCoreComponent* Inventory,const FItemData& InItemData)
{
	if (nullptr == Inventory) return;

	EInventoryPanel LocalPanel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(InItemData);
	TArray<FItemData> LocalInventory{};
	int32 InventorySize = 0;
	Inventory->GetInventoryAndSize(LocalPanel, LocalInventory, InventorySize);


	if (Inventory->UseInventorySize)
	{
		FItemData LocalItemData{};
		LocalItemData.Index = InItemData.Index;

		if (LocalInventory.IsValidIndex(LocalItemData.Index))
		{
			LocalInventory[LocalItemData.Index] = InItemData;
		}
		Inventory->ApplyChangesToInventoryArray(LocalPanel, LocalInventory);
		Inventory->OnRemovedFromInventoryArray.Broadcast(InItemData);
		Inventory->OnRefreshInventory.Broadcast(LocalPanel);
	}
	else
	{
		LocalInventory.Remove(InItemData);
		Inventory->ApplyChangesToInventoryArray(LocalPanel, LocalInventory);
		Inventory->OnRemovedFromInventoryArray.Broadcast(InItemData);
		Inventory->OnRefreshInventory.Broadcast(LocalPanel);
	}
}


void UInventoryCoreComponent::Server_TransferItemFromEquipment_Implementation(UEquipmentComponent* Sender,FItemData ItemData, FItemData InSlotData, EInputMethod Method)
{
	bool IsTransfer = TransferItemFromEquipment(Sender, ItemData, InSlotData, Method);
	FString Message{};
	Client_TransferItemReturnValue(IsTransfer, Message);
}

void UInventoryCoreComponent::Server_ConfirmationPopupAccepted_Implementation(UInventoryCoreComponent* Receiver,
                                                                             UInventoryCoreComponent* Sender,
                                                                             FItemData ItemData, FItemData InSlotData,
                                                                             EInputMethod Method,
                                                                             EItemDestination Initiator,
                                                                             EItemDestination Destination,
                                                                             AActor* OwningPlayer)
{
	Receiver->ConfirmationPopupAccepted(Sender,ItemData,InSlotData,Method,Initiator,Destination,OwningPlayer);
}

void UInventoryCoreComponent::Server_TransferItemFromInventory_Implementation(UInventoryCoreComponent* Receiver,
                                                                             UInventoryCoreComponent* Sender,
                                                                             FItemData ItemData, FItemData InSlotData,
                                                                             EInputMethod Method,
                                                                             AActor* OwningPlayer)
{
	bool IsTransfer = Receiver->TransferItemFromInventory(Sender, ItemData, InSlotData, Method, OwningPlayer);
	FString Message{};
	Client_TransferItemReturnValue(IsTransfer, Message);
}


void UInventoryCoreComponent::Server_MoveItemToSlot_Implementation(UInventoryCoreComponent* Inventory, EInventoryPanel Panel, int32 FromPosition, int32 ToPosition)
{
	// GetItemBySlot	: Is this index in the inventory
	// IsItemClassValid : Is Item Calss valid
	// if UseInventorySize is false, ToItemData`index may be -1 
	if (Inventory)
	{
		FItemData FromItemData;
		FItemData ToItemData;
		if(GetItemBySlot(Panel,FromPosition,FromItemData) && GetItemBySlot(Panel, ToPosition,ToItemData))
		{
			if(UInventoryUtilityLibrary::IsItemClassValid(FromItemData) && UInventoryUtilityLibrary::IsItemClassValid(ToItemData)) // Swap
			{
				Inventory->SwapItemsInInventory(FromItemData, ToItemData);
			}
			else if(UInventoryUtilityLibrary::IsItemClassValid(FromItemData) &&	!UInventoryUtilityLibrary::IsItemClassValid(ToItemData) && UseInventorySize) // Move 
			{
				Inventory->MoveItemInInventory(FromItemData,ToItemData.Index);
			}
		}
	}
}

void UInventoryCoreComponent::Server_SplitItemsInInventory_Implementation(UInventoryCoreComponent* Receiver,
                                                                         UInventoryCoreComponent* Sender,
                                                                         FItemData ItemData, FItemData InSlotData,
                                                                         FItemData StackableLeft,
                                                                         EInputMethod Method,
                                                                         EItemDestination Initiator,
                                                                         EItemDestination Destination,
                                                                         AActor* OwningPlayer)
{
	Receiver->SplitItemsInInventory(Sender,ItemData,InSlotData,StackableLeft,Method,Initiator,Destination,OwningPlayer);
}

void UInventoryCoreComponent::Server_SortInventory_Implementation(UInventoryCoreComponent* Inventory, ESortMethod Method, EInventoryPanel SinglePanel,                                                                bool EveryPanel)
{
	Inventory->SortInventory(Method, SinglePanel, EveryPanel);
}


void UInventoryCoreComponent::Server_RemoveItemFromInventory_Implementation(UInventoryCoreComponent* Inventory, FItemData ItemData)
{
	Inventory->RemoveItemFromInventoryArray(ItemData);
}

void UInventoryCoreComponent::Server_AddItemToInventory_Implementation(UInventoryCoreComponent* Inventory, FItemData ItemData, int32 SlotIndex)
{
	Inventory->AddItemToInventoryArray(ItemData, SlotIndex);
}

void UInventoryCoreComponent::Server_UpdateItems_Implementation(AActor* Actor)
{
	if(Actor)
	{
		UInventoryComponent* InventoryComponent = Actor->FindComponentByClass<UInventoryComponent>();
		if (InventoryComponent)
		{
			for (auto USePanel : InventoryComponent->PanelsToUse)
			{
				TArray<FItemData> LocalInventory{};
				int32 InventorySize = 0;
				GetInventoryAndSize(USePanel, LocalInventory, InventorySize);

				Client_UpdateItems(InventoryComponent, USePanel, LocalInventory);
			}
		}
	}
}

void UInventoryCoreComponent::AddViewer_Implementation(APlayerState* PlayerState, UInventoryCoreComponent* Inventory)
{
	if (PlayerState && Inventory)
	{
		Inventory->CurrentViewers.AddUnique(PlayerState);
	}
}

void UInventoryCoreComponent::RemoveViewer_Implementation(APlayerState* PlayerState, UInventoryCoreComponent* Inventory)
{
	if (PlayerState && Inventory && Inventory->CurrentViewers.Contains(PlayerState))
	{
		Inventory->CurrentViewers.Remove(PlayerState);
	}
}

void UInventoryCoreComponent::Client_UpdateRemovedItem_Implementation(UInventoryCoreComponent* Inventory,
                                                                     FItemData ItemData)
{
	UpdateRemovedItemInInventory(Inventory,ItemData);
}


void UInventoryCoreComponent::Client_TransferItemReturnValue_Implementation(bool Success, const FString& FailureMessage)
{
	
}

void UInventoryCoreComponent::Client_UpdateAddedItem_Implementation(UInventoryCoreComponent* Inventory,  FItemData ItemData)
{
	UpdateAddedItemInInventory(Inventory,ItemData);
}

void UInventoryCoreComponent::Client_UpdateItems_Implementation(UInventoryCoreComponent* Inventory, EInventoryPanel Panel, const TArray<FItemData>& ItemDatas)
{
	if(Inventory)
	{
		ApplyChangesToInventoryArray(Panel,ItemDatas);
		OnRefreshInventory.Broadcast(Panel);
	}
}


void UInventoryCoreComponent::Multicast_SetOwnerGold_Implementation(float NewGold)
{
	OwnerGold = NewGold;
}

void UInventoryCoreComponent::Multicast_SetCurrentWeight_Implementation(float NewWeight)
{
	CurrentInventoryWeight = NewWeight;
}

#pragma endregion

#pragma region Function Internals


void UInventoryCoreComponent::PopulateInventory() // BuildInitialInventory
{
	for (const auto& DT : AllItemsFromDataTables)
	{
		if (nullptr == DT) return;

		TArray<FName> RowNames;
		UDataTableFunctionLibrary::GetDataTableRowNames(DT, RowNames);

		for (const auto& Name : RowNames)
		{
			FItemData LocalItemData = *DT->FindRow<FItemData>(Name, "");
			int32 LocalEmptySlotIndex = -1;

			if (GetEmptyInventorySlot(LocalItemData,LocalEmptySlotIndex) && LocalEmptySlotIndex >= 0)
			{
				AddItemToInventoryArray(LocalItemData, LocalEmptySlotIndex);
				float Weight = UInventoryUtilityLibrary::CalculateStackedItemWeight(LocalItemData);
				AddWeightToInventory(Weight);
			}
		}
	}
}

void UInventoryCoreComponent::AddInventoryRow() // BuildInitialInventory
{
	for (const auto& SingleDTItem : SingleDTItems)
	{
		const UDataTable* DT = SingleDTItem.TableAndRow.DataTable;
		if(nullptr == DT) continue;
		
		FName RowName = SingleDTItem.TableAndRow.RowName;
		
		FItemData* FoundItemData = DT->FindRow<FItemData>(RowName, "");
		if (FoundItemData)
		{
			FoundItemData->Quantity = FMath::Clamp(SingleDTItem.Quantity, 1, SingleDTItem.Quantity);

			int32 LocalEmptySlotIndex = -1;
			if (GetEmptyInventorySlot(*FoundItemData,LocalEmptySlotIndex))
			{
				AddItemToInventoryArray(*FoundItemData, LocalEmptySlotIndex);
				float Weight = UInventoryUtilityLibrary::CalculateStackedItemWeight(*FoundItemData);
				AddWeightToInventory(Weight);
			}
		}
	}
}

void UInventoryCoreComponent::RefreshInventoryOnAdd(FItemData& ItemData, int32 SlotIndex, EInventoryPanel LocalPanel)
{
	if (UKismetSystemLibrary::IsStandalone(GetWorld()))
	{
		// IF STANDALONE: Switch Active Panel and Refresh Inventory
		SwitchActivePanel(LocalPanel);
		OnRefreshInventory.Broadcast(LocalPanel);
		OnAddedToInventoryArray.Broadcast(ItemData);
		OnHighlightInventorySlot.Broadcast(SlotIndex);
	}
	else
	{
		// CLIENT: Update Item
		UpdateViewersItem(ItemData, false);
	}
}
#pragma endregion


void UInventoryCoreComponent::DebugLogActiveItems() const
{
	UE_LOG(LogTemp, Log, TEXT("=== Active Items Debug ==="));
	
	for (int32 i = 0; i < InventoryPanelArray.Num(); ++i)
	{
		for(int32 j=0; j< InventoryPanelArray[i].Num(); ++j)
		{
			FItemData CurrentItemData = InventoryPanelArray[i][j];
			UE_LOG(LogTemp,Warning,TEXT("Panel: %d,Index: %d,ID: %s,Quantity: %d"),i,j,*CurrentItemData.ID, CurrentItemData.Quantity);
		}
	}
	UE_LOG(LogTemp, Log, TEXT("=== End Debug ==="));
}