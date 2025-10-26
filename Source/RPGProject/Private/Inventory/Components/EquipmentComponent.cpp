// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/Components/EquipmentComponent.h"

#include "Components/Status/StatsComponent.h"
#include "Header/DebugHeader.h"
#include "Misc/EnumRange.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Item/Data/ItemDataStructs.h"
#include "Interface/WidgetManager.h"
#include "Inventory/Components/InventoryComponent.h"
#include "Inventory/Components/InventoryCoreComponent.h"
#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"
#include "Item/Equippable/BaseEquippable.h"
#include "Shared/Components/LevelingComponent.h"

UEquipmentComponent::UEquipmentComponent()
{

	PrimaryComponentTick.bCanEverTick = true;
}


void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UEquipmentComponent::BuildEquipment()
{
	for (EItemSlot ItemSlot : TEnumRange<EItemSlot>())
	{
		if(EquipmentData.Find(ItemSlot))
		{
			FItemData& SlotItemData = EquipmentData[ItemSlot];
			SlotItemData.EquipmentSlot = ItemSlot;
			SlotItemData.IsEquipped = true;
		}
		else
		{
			FItemData EmptyItemData;
			EmptyItemData.EquipmentSlot = ItemSlot;
			EquipmentData.Add({ItemSlot,EmptyItemData});
		}
	}
}

void UEquipmentComponent::BuildInitialEquipment()
{
	for(auto& SingleDTItem : InitialEquipment)
	{
		const UDataTable* DT = SingleDTItem.TableAndRow.DataTable;
		FName RowName = SingleDTItem.TableAndRow.RowName;

		FItemData* FoundItemData = DT->FindRow<FItemData>(RowName, "");
		if(FoundItemData)
		{
			FoundItemData->Quantity = FMath::Clamp(SingleDTItem.Quantity,1,SingleDTItem.Quantity);

			FItemData SlotItemData = AssignItemFromEquipmentSlot(*FoundItemData);
			AddItemToEquipmentArray(*FoundItemData,SlotItemData.EquipmentSlot);
			AttachItemToEquipment(*FoundItemData);

			UInventoryComponent* InventoryComponent = GetOwner()->FindComponentByClass<UInventoryComponent>();
			if(InventoryComponent)
			{
				float Weight = UInventoryUtilityLibrary::CalculateStackedItemWeight(*FoundItemData);
				InventoryComponent->AddWeightToInventory(Weight);
			}
		}
	}

	InitialEquipment.Empty();
}

void UEquipmentComponent::AddItemToEquipmentArray(FItemData InItemData, EItemSlot EquipmentSlot)
{
	if(GetOwner()->HasAuthority())
	{
		InItemData.EquipmentSlot = EquipmentSlot;
		InItemData.IsEquipped = true;

		EquipmentData.Add({EquipmentSlot, InItemData});

		if(UKismetSystemLibrary::IsStandalone(GetOwner()))
		{
			OnAddedToEquipment.Broadcast(InItemData);
		}
		else
		{
			ClientUpdateAddedItem(InItemData);
		}
	}
}

void UEquipmentComponent::RemoveItemFromEquipmentArray(const FItemData& InItemData)
{
	if(GetOwner()->HasAuthority())
	{
		EItemSlot EquipmentSlot = InItemData.EquipmentSlot;
		EquipmentData.Remove(EquipmentSlot);

		FItemData LocalItemData;
		LocalItemData.EquipmentSlot = EquipmentSlot;
		EquipmentData.Add({EquipmentSlot,LocalItemData});

		if(UKismetSystemLibrary::IsStandalone(GetOwner()))
		{
			OnAddedToEquipment.Broadcast(InItemData);
		}
		else
		{
			ClientUpdateRemovedItem(InItemData);
		}
	}
}

void UEquipmentComponent::AddToStackInEquipment(const FItemData& InItemData, EItemSlot EquipmentSlot)
{
	if(EquipmentData.Find(EquipmentSlot))
	{
		FItemData& SlotItemData = EquipmentData[EquipmentSlot];
		SlotItemData.Quantity += InItemData.Quantity;
		AddItemToEquipmentArray(SlotItemData,SlotItemData.EquipmentSlot);
	}
}

bool UEquipmentComponent::TryToAddToPartialStack(const FItemData& InItemData)
{
	TArray<FItemData> ItemArrays;

	for (auto It = EquipmentData.CreateIterator(); It; ++It)
	{
		FItemData& Value = It.Value();
		ItemArrays.Add(Value);
	}
	int32 SlotIndex = 0;
	if(UInventoryUtilityLibrary::HasPartialStack(ItemArrays,InItemData,SlotIndex))
	{
		AddToStackInEquipment(InItemData,ItemArrays[SlotIndex].EquipmentSlot);
		return true;
	}
	return false;	
}


void UEquipmentComponent::AttachItemToEquipment(const FItemData& InItemData)
{
	if (!UInventoryUtilityLibrary::IsItemClassValid(InItemData))
	{
		return;
	}

	EItemSlot Slot = InItemData.EquipmentSlot;
	UItemBase* ItemBase = Cast<UItemBase>(InItemData.ItemClass->GetDefaultObject());
	if (!ItemBase) 
	{
		DebugHeader::LogWarning(TEXT("UEquipmentComponent::AttachItemToEquipment : ItemBase is null for item: %s"), *InItemData.ID);
		return;
	}
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh = ItemBase->SkeletalMesh;
	OnItemAttach.Broadcast(InItemData);

	switch (Slot)
	{
	case EItemSlot::HEAD:
	case EItemSlot::GLOVES:
	case EItemSlot::NECK:
	case EItemSlot::CHESTPLATE:
	case EItemSlot::LEGS:
	case EItemSlot::BOOTS:
		{
			ServerUpdateEquipmentMesh(Slot, SkeletalMesh.ToSoftObjectPath());
		}
		break;
	case EItemSlot::WEAPON:
	case EItemSlot::SHIELD:
		{
			ServerSpawnEquippableActor(InItemData);
		}
		break;
	}
	
}

void UEquipmentComponent::DetachItemFromEquipment(const FItemData& InItemData)
{
	if(UInventoryUtilityLibrary::IsItemClassValid(InItemData))
	{
		EItemSlot Slot = InItemData.EquipmentSlot;
		
		OnItemDetach.Broadcast(InItemData);

		switch (Slot)
		{
		case EItemSlot::HEAD:
		case EItemSlot::GLOVES:
		case EItemSlot::NECK:
		case EItemSlot::CHESTPLATE:
		case EItemSlot::LEGS:
		case EItemSlot::BOOTS:
			{
				FSoftObjectPath EmptyPath{};
				ServerUpdateEquipmentMesh(Slot,EmptyPath);
			}
			break;
		case EItemSlot::WEAPON:
		case EItemSlot::SHIELD:
			{
				ServerDestroyEquippableActor(InItemData);
			}
			break;
		}
	}
}

void UEquipmentComponent::RemoveItemQuantity(const FItemData& InItemData, int32 Quantity)
{
	FItemData LocalItemData = InItemData;
	int32 CurrentQuantity = InItemData.Quantity;
	if(CurrentQuantity > Quantity)
	{
		RemoveItemFromEquipmentArray(InItemData);
		LocalItemData.Quantity = CurrentQuantity-Quantity;
		AddItemToEquipmentArray(LocalItemData,InItemData.EquipmentSlot);
	}
	else
	{
		RemoveItemFromEquipmentArray(InItemData);
		DetachItemFromEquipment(InItemData);
	}
}

bool UEquipmentComponent::TransferItemFromInventoryToEquipment(UInventoryCoreComponent* InventoryComponent,
	FItemData InItemData,FItemData InSlotData, EInputMethod InputMethod)
{
	if(CanItemBeEquipped(InItemData))
	{
		if(InputMethod == EInputMethod::RightClick)
		{
			InSlotData = AssignItemFromEquipmentSlot(InItemData);
		}

		if(UInventoryUtilityLibrary::IsItemClassValid(InSlotData))
		{
			if(UInventoryUtilityLibrary::AreItemsTheSame(InItemData,InSlotData) &&
				UInventoryUtilityLibrary::AreItemsStackable(InItemData,InSlotData))
			{
				// Stack Items
				AddToStackInEquipment(InItemData,InSlotData.EquipmentSlot);
				InventoryComponent->RemoveItemFromInventoryArray(InItemData);
			}
			else 
			{
				// Swap Items
				RemoveItemFromEquipmentArray(InSlotData);
				InventoryComponent->AddItemToInventoryArray(InSlotData,InItemData.Index);
				AddItemToEquipmentArray(InItemData,InSlotData.EquipmentSlot);
				DetachItemFromEquipment(InSlotData);
				AttachItemToEquipment(InItemData);
				TryToUnequipAssociatedSlot(InventoryComponent,InItemData);
			}
			return true;
		}
		else // Empty
		{
			InventoryComponent->RemoveItemFromInventoryArray(InItemData);
			AddItemToEquipmentArray(InItemData,InSlotData.EquipmentSlot);
			AttachItemToEquipment(InItemData);
			TryToUnequipAssociatedSlot(InventoryComponent,InItemData);
			return true;
		}
	}
	return false;
}

void UEquipmentComponent::TransferItemFromEquipmentToEquipment(FItemData DraggedData,FItemData InSlotData)
{
	if(UInventoryUtilityLibrary::IsItemClassValid(InSlotData))
	{
		// Swap First -> Second, Second -> First
		RemoveItemFromEquipmentArray(InSlotData);
		AddItemToEquipmentArray(DraggedData,InSlotData.EquipmentSlot);
		RemoveItemFromEquipmentArray(DraggedData);
		AddItemToEquipmentArray(InSlotData,DraggedData.EquipmentSlot);
		AttachItemToEquipment(InSlotData);
		AttachItemToEquipment(DraggedData);
	}
	else
	{
		//Add Item To Empty Slot, First -> Second
		RemoveItemFromEquipmentArray(DraggedData);
		DetachItemFromEquipment(DraggedData);
		AddItemToEquipmentArray(DraggedData,InSlotData.EquipmentSlot);
		AttachItemToEquipment(DraggedData);
	}
}

bool UEquipmentComponent::CanItemBeEquipped(FItemData InItemData)
{
	if(!UInventoryUtilityLibrary::IsItemClassValid(InItemData))
	{
		return false;
	}
	
	UItemBase* ItemBase = Cast<UItemBase>(InItemData.ItemClass->GetDefaultObject());
	if(!ItemBase)
	{
		UE_LOG(LogTemp, Warning, TEXT("CanItemBeEquipped: ItemBase is null"));
		return false;
	}
	if(CheckRequiredLevel && ItemBase) 
	{
		int32 ItemRequiredLevel = ItemBase->RequiredLevel;
		ULevelingComponent* LevelingComponent = GetOwner()->FindComponentByClass<ULevelingComponent>();
		if(LevelingComponent && LevelingComponent->GetCurrentLevel() >= ItemRequiredLevel)
		{
			return true;
		}
	}
	return false;
}

bool UEquipmentComponent::GetItemByEquipmentSlot(EItemSlot Slot, FItemData& OutItemData)
{
	if(EquipmentData.Find(Slot))
	{
		if(UInventoryUtilityLibrary::IsItemClassValid(EquipmentData[Slot]))
		{
			OutItemData = EquipmentData[Slot];
			return true;
		}
	}
	return false;
}

bool UEquipmentComponent::GetItemByID(FString ItemID, FItemData& OutItemData)
{
	TArray<FItemData> ItemArrays;

	for (auto It = EquipmentData.CreateIterator(); It; ++It)
	{
		FItemData& Value = It.Value();
		ItemArrays.Add(Value);
	}

	for(auto& CurItemData : ItemArrays)
	{
		FItemData LocalItemData{};
		if(UInventoryUtilityLibrary::FindItemByID(ItemArrays,ItemID,LocalItemData))
		{
			OutItemData = LocalItemData;
			return true;
		}
	}
	return false;
}

#pragma region Network

void UEquipmentComponent::ServerSpawnEquippableActor_Implementation(const FItemData& InItemData)
{
	SpawnAndAttachEquippableActor(InItemData);
}

void UEquipmentComponent::ServerDestroyEquippableActor_Implementation(const FItemData& InItemData)
{
	DestroyEquippableActor(InItemData);
}

void UEquipmentComponent::ClientUpdateAddedItem_Implementation(const FItemData& InItemData)
{
	UpdateAddedItem(InItemData);
}

void UEquipmentComponent::ClientUpdateRemovedItem_Implementation(const FItemData& InItemData)
{
	UpdateRemovedItem(InItemData);
}

void UEquipmentComponent::ServerRemoveItemQuantity_Implementation(const FItemData& InItemData, int32 Quantity)
{
	RemoveItemQuantity(InItemData,Quantity);
}

void UEquipmentComponent::ServerTransferItemFromInventory_Implementation(UInventoryCoreComponent* Inventory,
	FItemData InItemData, FItemData InSlotData, EInputMethod Method)
{
	TransferItemFromInventoryToEquipment(Inventory,InItemData,InSlotData,Method);
}

void UEquipmentComponent::ServerTransferItemFromEquipment_Implementation(FItemData DraggedItemData,
	FItemData InSlotData)
{
	TransferItemFromEquipmentToEquipment(DraggedItemData,InSlotData);
}

void UEquipmentComponent::ClientTransferItemReturnValue_Implementation(bool Success,const FString& Message)
{
	if(Success)
	{
		IWidgetManager* WidgetManager = Cast<IWidgetManager>(GetOwner());
		FText Text = FText::FromString(Message);
		if(WidgetManager)
		{
			// WidgetManager->DisplayMessageNotify(Text);
		}
	}
}

void UEquipmentComponent::ServerUpdateEquipmentMesh_Implementation(EItemSlot Slot, const FSoftObjectPath& Mesh)
{
	TSoftObjectPtr<USkeletalMesh> SoftMesh(Mesh);
	OnUpdateEquipmentMesh.Broadcast(Slot,SoftMesh);
}

void UEquipmentComponent::ServerToggleEquippableSocket_Implementation(bool IsInCombat, EItemSlot Slot)
{
	ToggleEquippableSocket(Slot,IsInCombat);
}

void UEquipmentComponent::ServerToggleAllEquippableSockets_Implementation(bool IsInCombat)
{
	TArray<EItemSlot> SlotArrays;
	EquippableReferences.GenerateKeyArray(SlotArrays);

	for(auto Slot : SlotArrays)
	{
		ToggleEquippableSocket(Slot,IsInCombat);
	}
}

void UEquipmentComponent::ClientInitializeEquipment_Implementation()
{
	if(!UKismetSystemLibrary::IsStandalone(this))
	{
		BuildEquipment();
	}
}

#pragma endregion 

TArray<FItemData> UEquipmentComponent::GetEquipmentItems()
{
	TArray<FItemData> ItemArrays;

	for (auto It = EquipmentData.CreateIterator(); It; ++It)
	{
		FItemData& Value = It.Value();
		ItemArrays.Add(Value);
	}
	return ItemArrays;
}

ABaseEquippable* UEquipmentComponent::GetEquippedActorReference(EItemSlot Slot)
{
	if(EquippableReferences.Find(Slot))
	{
		return EquippableReferences[Slot];
	}
	return nullptr;
}

FItemData UEquipmentComponent::AssignItemFromEquipmentSlot(FItemData InItemData)
{
	// TArray<EItemSlot> PocketSlot{EItemSlot::POCKET1,EItemSlot::POCKET2,EItemSlot::POCKET3,EItemSlot::POCKET4};
	// FItemData EquipmentSlotItemData{};
	// EItemSlot LocalSlot = EItemSlot::NONE;
	
	// if(PocketSlot.Contains(InItemData.EquipmentSlot))
	// {
	// 	if(UInventoryUtilityLibrary::FindEmptyEquipmentSlot(EquipmentData,PocketSlot,EItemSlot::POCKET1,LocalSlot))
	// 	{
	// 		if(GetItemByEquipmentSlot(LocalSlot,EquipmentSlotItemData))
	// 		{
	// 			return EquipmentSlotItemData;
	// 		}
	// 	}
	// }
	// else
	// {
	// 	UItemBase* ItemBase = InItemData.ItemClass.GetDefaultObject();
	// 	if(ItemBase->WeaponType == EWeaponType::OneHand && EnableOffHand) // if One Hand
	// 	{
	// 		if(!GetItemByEquipmentSlot(EItemSlot::SHIELD,EquipmentSlotItemData)) // Already One-hand weapon in shield slot
	// 		{
	// 			TArray<EItemSlot> WeaponSlot{EItemSlot::WEAPON,EItemSlot::SHIELD};
	// 			if(UInventoryUtilityLibrary::FindEmptyEquipmentSlot(EquipmentData,WeaponSlot,EItemSlot::WEAPON,LocalSlot))
	// 			{
	// 				GetItemByEquipmentSlot(LocalSlot,EquipmentSlotItemData);
	// 				return EquipmentSlotItemData;
	// 			}
	// 		}
	// 		else
	// 		{
	// 			GetItemByEquipmentSlot(EItemSlot::WEAPON,EquipmentSlotItemData);
	// 			return EquipmentSlotItemData;
	// 		}
	// 	}
	// 	else
	// 	{
	// 		GetItemByEquipmentSlot(InItemData.EquipmentSlot,EquipmentSlotItemData);
	// 		return EquipmentSlotItemData;
	// 	}
	// }
	return FItemData{};
}

void UEquipmentComponent::AttachEquippableToNewSocket(EItemSlot Slot, FName SocketName)
{
	if(ABaseEquippable* Equippable = GetEquippedActorReference(Slot))
	{
		Equippable->AttachToNewSocket(SocketName);
	}
}

void UEquipmentComponent::ToggleEquippableSocket(EItemSlot Slot,bool CombatMode)
{
	if(ABaseEquippable* Equippable = GetEquippedActorReference(Slot))
	{
		Equippable->ToggleCombatMode(CombatMode);
	}
}

void UEquipmentComponent::SpawnAndAttachEquippableActor(FItemData InItemData)
{
	AActor* Owner = GetOwner();
	if (!Owner || !Owner->HasAuthority())
	{
		return;
	}
    
	if (!UInventoryUtilityLibrary::IsItemClassValid(InItemData))
	{
		DebugHeader::LogWarning(TEXT("UEquipmentComponent::SpawnAndAttachEquippableActor : InItemData is null for item: %s"), *InItemData.ID);
		return;
	}
    
	UItemBase* ItemBase = Cast<UItemBase>(InItemData.ItemClass->GetDefaultObject());
	if (!ItemBase)
	{
		DebugHeader::LogWarning(TEXT("UEquipmentComponent::SpawnAndAttachEquippableActor : ItemBase is null for item: %s"), *InItemData.ID);
		return;
	}
    
	TSoftClassPtr<AActor> EquippableClass = ItemBase->EquippableClass;
	EItemSlot Slot = InItemData.EquipmentSlot;

	if (Slot == EItemSlot::WEAPON || Slot == EItemSlot::SHIELD)
	{
		UClass* LoadClass = UKismetSystemLibrary::LoadClassAsset_Blocking(EquippableClass);
		if (!LoadClass)
		{
			UE_LOG(LogTemp, Error, TEXT("SpawnAndAttachEquippableActor: Failed to load class"));
			return;
		}
        
		// 기존 장비 제거
		if (ABaseEquippable* CurrentEquipped = GetEquippedActorReference(Slot))
		{
			if (IsValid(CurrentEquipped))
			{
				CurrentEquipped->Destroy();
			}
		}

		UWorld* World = GetWorld();
		if (!World)
		{
			UE_LOG(LogTemp, Error, TEXT("SpawnAndAttachEquippableActor: World is null"));
			return;
		}
        
		World->SpawnActor<ABaseEquippable>(LoadClass);
	}
}

void UEquipmentComponent::DestroyEquippableActor(FItemData Slot)
{
	if(GetOwner() && GetOwner()->HasAuthority())
	{
		if(ABaseEquippable* BaseEquippableActor = GetEquippedActorReference(Slot.EquipmentSlot))
		{
			EquippableReferences.Remove(Slot.EquipmentSlot);
			BaseEquippableActor->Destroy();
		}
	}
}

void UEquipmentComponent::TryToUnequipAssociatedSlot(UInventoryCoreComponent* InventoryComponent, FItemData InItemData)
{
	if(!InItemData.ItemClass || !InventoryComponent)
	{
		return;
	}
    
	UItemBase* ItemBase = Cast<UItemBase>(InItemData.ItemClass->GetDefaultObject());
	if(!ItemBase) return;
    
	EItemSlot AssociatedSlot = EItemSlot::NONE;
    
	if(InItemData.EquipmentSlot == EItemSlot::WEAPON) AssociatedSlot = EItemSlot::SHIELD;		
	else if(InItemData.EquipmentSlot == EItemSlot::SHIELD) AssociatedSlot = EItemSlot::WEAPON;
    
	if(EnableOffHand && AssociatedSlot != EItemSlot::NONE)
	{
		FItemData SlotItemData;
		switch (ItemBase->WeaponType)
		{
		case EWeaponType::OffHand:
		case EWeaponType::OneHand:
			{
				if(GetItemByEquipmentSlot(AssociatedSlot, SlotItemData))
				{
					if (!UInventoryUtilityLibrary::IsItemClassValid(SlotItemData))
					{
						DebugHeader::LogWarning(TEXT("TryToUnequipAssociatedSlot: SlotItemData is not valid"));
						return;
					}
                    
					UItemBase* SlotItemBase = Cast<UItemBase>(SlotItemData.ItemClass->GetDefaultObject());
					if(!SlotItemBase)
					{
						DebugHeader::LogWarning(TEXT("TryToUnequipAssociatedSlot: SlotItemBase is null"));
						return;
					}
                    
					if(SlotItemBase->WeaponType == EWeaponType::TwoHand)
					{
						
					}
				}
			}
			break;
		}
	}
}

void UEquipmentComponent::InitializeEquipment()
{
	ClientInitializeEquipment();

	if(GetOwner() && GetOwner()->HasAuthority())
	{
		BuildEquipment();
		BuildInitialEquipment();
	}
}

void UEquipmentComponent::UpdateAddedItem(FItemData InItemData)
{
	EItemSlot Slot = InItemData.EquipmentSlot;
	EquipmentData.Add({Slot,InItemData});

	OnAddedToEquipment.Broadcast(InItemData);
	OnItemAttach.Broadcast(InItemData);
}

void UEquipmentComponent::UpdateRemovedItem(FItemData InItemData)
{
	EItemSlot Slot = InItemData.EquipmentSlot;
	EquipmentData.Remove(Slot);

	FItemData EmptyItemData{};
	EmptyItemData.EquipmentSlot = Slot;
	EquipmentData.Add({Slot,EmptyItemData});
	
	OnRemovedFromEquipment.Broadcast(InItemData);
	OnItemDetach.Broadcast(InItemData);
}
