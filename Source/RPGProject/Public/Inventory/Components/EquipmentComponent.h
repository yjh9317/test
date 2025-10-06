// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/Components/InventoryCoreComponent.h"
#include "Item/ItemBase.h"
#include "Item/Data/ItemDataStructs.h"
#include "Types/InputEnums.h"
#include "EquipmentComponent.generated.h"

class ABaseEquippable;
class UInventoryCoreComponent;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAddedToEquipment,const FItemData& Data);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRemovedFromEquipment,const FItemData& Data);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemAttach,const FItemData& Data);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemDetach,const FItemData& Data);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUpdateEquipmentMesh,EItemSlot Slot,TSoftObjectPtr<USkeletalMesh> Mesh);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEquipmentComponent();
	
	FOnAddedToEquipment OnAddedToEquipment;
	FOnRemovedFromEquipment OnRemovedFromEquipment;
	FOnItemAttach OnItemAttach;
	FOnItemDetach OnItemDetach;
	FOnUpdateEquipmentMesh OnUpdateEquipmentMesh;
protected:
	virtual void BeginPlay() override;
	
	TMap<EItemSlot,FItemData> EquipmentItemData;
	// TMap<EItemSlot,APickup>> 
public:	
	void BuildEquipment();
	void BuildInitialEquipment();
	void AddItemToEquipmentArray(FItemData InItemData,EItemSlot EquipmentSlot);
	void RemoveItemFromEquipmentArray(const FItemData& InItemData);
	void AddToStackInEquipment(const FItemData& InItemData,EItemSlot EquipmentSlot);
	bool TryToAddToPartialStack(const FItemData& InItemData);
	void AttachItemToEquipment(const FItemData& InItemData);
	void DetachItemFromEquipment(const FItemData& InItemData);
	void RemoveItemQuantity(const FItemData& InItemData,int32 Quantity);
	bool TransferItemFromInventoryToEquipment(UInventoryCoreComponent* InventoryComponent,FItemData InItemData,FItemData InSlotData,EInputMethod InputMethod);
	void TransferItemFromEquipmentToEquipment(FItemData InDraggedData,FItemData InSlotData);
	bool CanItemBeEquipped(FItemData InItemData);
	bool GetItemByEquipmentSlot(EItemSlot Slot, FItemData& OutItemData);
	bool GetItemByID(FString ItemID,FItemData& OutItemData);
	TArray<FItemData> GetEquipmentItems();
	ABaseEquippable* GetEquippedActorReference(EItemSlot Slot);
	FItemData AssignItemFromEquipmentSlot(FItemData InItemData);
	void AttachEquippableToNewSocket(EItemSlot Slot,FName SocketName);
	void ToggleEquippableSocket(EItemSlot Slot,bool CombatMode);
	void SpawnAndAttachEquippableActor(FItemData InItemData);
	void DestroyEquippableActor(FItemData Slot);
	void TryToUnequipAssociatedSlot(UInventoryCoreComponent* InventoryComponent,FItemData InItemData);

	void InitializeEquipment();

	void UpdateAddedItem(FItemData InItemData);
	void UpdateRemovedItem(FItemData InItemData);
	
#pragma region Network
	
	UFUNCTION(Server,Reliable)
	void ServerSpawnEquippableActor(const FItemData& InItemData);
	
	UFUNCTION(Server,Reliable)
	void ServerDestroyEquippableActor(const FItemData& InItemData);
	
	UFUNCTION(Client,Reliable)
	void ClientUpdateAddedItem(const FItemData& InItemData);
	
	UFUNCTION(Client,Reliable)
	void ClientUpdateRemovedItem(const FItemData& InItemData);
	
	UFUNCTION(Server,Reliable)
	void ServerRemoveItemQuantity(const FItemData& InItemData,int32 Quantity);
	
	UFUNCTION(Server,Reliable)
	void ServerTransferItemFromInventory(UInventoryCoreComponent* Inventory,FItemData InItemData,FItemData InSlotData,EInputMethod Method);
	
	UFUNCTION(Server,Reliable)
	void ServerTransferItemFromEquipment(FItemData DraggedItemData,FItemData InSlotData);
	
	UFUNCTION(Client,Reliable)
	void ClientTransferItemReturnValue(bool Success,const FString& Message); //const FString&
	
	UFUNCTION(Server,Reliable)
	void ServerUpdateEquipmentMesh(EItemSlot Slot,const FSoftObjectPath& Mesh);
	
	UFUNCTION(Server,Reliable)
	void ServerToggleEquippableSocket(bool IsInCombat,EItemSlot Slot);
	
	UFUNCTION(Server,Reliable)
	void ServerToggleAllEquippableSockets(bool IsInCombat);
	
	UFUNCTION(Client,Reliable)
	void ClientInitializeEquipment();

#pragma endregion

	bool EnableOffHand;
	
protected:
	TMap<EItemSlot,FItemData> EquipmentData;
	TMap<EItemSlot,ABaseEquippable*> EquippableReferences;
	TArray<FItemStack> InitialEquipment;
	bool CheckRequiredLevel;
public:
	const TMap<EItemSlot,FItemData>& GetEquipmentData() const {return EquipmentData;}
};
