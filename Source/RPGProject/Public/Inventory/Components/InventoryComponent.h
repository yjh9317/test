#pragma once

#include "CoreMinimal.h"
#include "InventoryCoreComponent.h"
#include "Components/ActorComponent.h"
#include "Item/ItemBase.h"
#include "InventoryComponent.generated.h"


class UObjectPoolComponent;
class APlayerController;
class UInteractableComponent;
class UItemBase;

DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);
DECLARE_MULTICAST_DELEGATE(FOnTakeItem);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemLooted,const FItemData&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemUsed,const FItemData&);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RPGPROJECT_API UInventoryComponent : public UInventoryCoreComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent(const FObjectInitializer& ObjectInitializer);
	void InputInventoryFromSwitcher();
	void InputInventory();
	virtual void InitializeInventory(APlayerController* PC) override;
	void ClientInitializeInventory(APlayerController* PC);
	UPROPERTY()
	APlayerController* PlayerController;


protected:
	virtual void BeginPlay() override;
private:
	bool IsLootBarOpen;
	bool IsInteractableActorWidgetOpen;

	UPROPERTY(VisibleInstanceOnly,Category="Reference")
	TObjectPtr<UUserWidget> InventoryWidgetRef;
	
	UPROPERTY(EditDefaultsOnly,Category="Class")
	TSubclassOf<UUserWidget> InventoryWidgetClass;

	UPROPERTY(VisibleInstanceOnly,Category="Reference")
	TObjectPtr<UUserWidget> VendorWidgetRef;
	UPROPERTY(EditDefaultsOnly,Category="Class")
	TSubclassOf<UUserWidget> VendorWidgetClass;

	UPROPERTY(VisibleInstanceOnly,Category="Reference")
	TObjectPtr<UUserWidget> StorageWidgetRef;
	UPROPERTY(EditDefaultsOnly,Category="Class")
	TSubclassOf<UUserWidget> StorageWidgetClass;

	int32 DroppedIndex;


	UPROPERTY()
	TWeakObjectPtr<AActor> LootBagPtr;
	TSoftClassPtr<AActor> LootBagClass;

	// Inner
	void FinalizeTransferUpdates(UInventoryCoreComponent* Sender,const FItemData& InItemData);
public:
	FOnTakeItem OnTakeItem;
	FOnTakeItem OnTakeAllItem;
	FOnItemLooted OnItemLooted;
	FOnItemUsed OnItemUsed;
	
	virtual bool TransferItemFromInventory(UInventoryCoreComponent* Sender,FItemData& InItemData,const FItemData& InSlotData, EInputMethod Method, AActor* OwningPlayer) override;
	virtual void SplitItemsInInventory(UInventoryCoreComponent* Sender, FItemData InItemData,const FItemData& InSlotData, FItemData StackableLeft, EInputMethod Method, EItemDestination Initiator, EItemDestination Destination, AActor* OwningPlayer) override;
	virtual void ConfirmationPopupAccepted(UInventoryCoreComponent* Sender, FItemData InItemData,const FItemData& InSlotData, EInputMethod Method, EItemDestination Initiator, EItemDestination Destination, AActor* OwningPlayer) override;
	virtual void AddItemToInventoryArray(FItemData& InItemData, int32 SlotIndex) override;
	virtual void SwapItemsInInventory(FItemData FirstItemData, FItemData SecondItemData) override;
	virtual void MoveItemInInventory(FItemData SourceItemData, int32 TargetIndex) override;
	virtual void RefreshInventoryOnAdd(FItemData& ItemData, int32 SlotIndex, EInventoryPanel LocalPanel) override;
	
	void TakeItem(UInventoryCoreComponent* Sender,FItemData InItemData,AActor* OwningPlayer);
	void TakeAllItems(UInventoryCoreComponent* Sender,AActor* OwningPlayer);
	void DropItemOnTheGround(const FItemData& InItemData,EItemDestination Initiator,AActor* OwningPlayer);
	void SpawnLootBagNearThePlayer(FItemData& InItemData,AActor* OwningPlayer);
	void SpawnItemMeshNearThePlayer(FItemData& InItemData,AActor* OwningPlayer);
	bool IsCollidingWithLootBag(const FVector& Start,const FVector& End, AActor* DroppedBag);
	bool AttemptUsingTransferredItem(UInventoryCoreComponent* Sender,const FItemData& InItemData);
	UInteractableComponent* GetCurrentInteractable() const;
	void ModifyItemDurability(FItemData InItemData,int32 Amount, AActor* OwningPlayer);

#pragma region Widget
	UFUNCTION(BlueprintCallable, Category = "UI")
	void PresentModalWidget(UUserWidget* WidgetToShow, int32 ZOrder = 1);

	UFUNCTION(BlueprintCallable, Category = "UI")
	void DismissModalWidget(UUserWidget* WidgetToHide);
	
	void CreateInventoryWidget();
	void OpenInventoryWidget();
	void CloseInventoryWidget();
	
	void OpenLootBarWidget();
	void CloseLootBarWidget();
	
	void CreateVendorWidget();
	void OpenVendorWidget();
	void CloseVendorWidget();
	
	void CreateStorageWidget();
	void OpenStorageWidget();
	void CloseStorageWidget();
#pragma endregion 

#pragma region NetworkFunction
	UFUNCTION(Server,Reliable)
	void Server_TakeItem(UInventoryCoreComponent* Sender,FItemData InItemData,AActor* OwningPlayer);
	UFUNCTION(Server,Reliable)
	void Server_TakeAllItems(UInventoryCoreComponent* Sender,AActor* OwningPlayer);
	UFUNCTION(Server,Reliable)
	void Server_SpawnLootBag(FItemData InItemData,AActor* OwningPlayer);
	UFUNCTION(Server,Reliable)
	void Server_SpawnInteractable(FItemData InItemData,AActor* OwningPlayer);
	UFUNCTION(Server,Reliable)
	void Server_DropItemOnTheGround(const FItemData& InItemData,EItemDestination Initiator,AActor* OwningPlayer);
	UFUNCTION(Server,Reliable)
	void Server_ModifyItemDurability(const FItemData& InItemData,int32 Amount, AActor* OwningPlayer);
	UFUNCTION(Server,Unreliable)
	void Server_ChangeDroppedIndex(int32 InDroppedIndex);

	UFUNCTION(Client,Reliable)
	void Client_ItemLooted(const FItemData& InItemData);
	UFUNCTION(Client,Reliable)
	void Client_TakeItemReturnValue(bool Success,const FString& FailureMessage, bool RemoveInteraction);
	
	virtual void Client_TransferItemReturnValue(bool Success,const FString& FailureMessage) override;
#pragma endregion
public:
	UUserWidget* GetInventoryWidgetRef() const { return InventoryWidgetRef; }
	UUserWidget* GetStorageWidgetRef() const { return StorageWidgetRef; }
	UUserWidget* GetVendorWidgetRef() const { return VendorWidgetRef; }
};

