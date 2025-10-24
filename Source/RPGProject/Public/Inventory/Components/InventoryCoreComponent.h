
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item/Data/ItemDataStructs.h"
#include "Types/InputEnums.h"
#include "Inventory/Data/InventoryDatas.h"
#include "InventoryCoreComponent.generated.h"


class UEquipmentComponent;


DECLARE_MULTICAST_DELEGATE_OneParam(FOnRefreshInventory,EInventoryPanel Panel);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSwitchInventoryActivePanel,EInventoryPanel Panel);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRemovedFromInventoryArray,const FItemData& InItemData);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnAddedToInventoryArray,const FItemData& InItemData);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHighlightInventorySlot,int32 Slot);
DECLARE_MULTICAST_DELEGATE(FOnCurrentWeightChanged);
DECLARE_MULTICAST_DELEGATE(FOnMaxWeightChanged);
DECLARE_MULTICAST_DELEGATE(FOnOwnerGoldChanged);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UInventoryCoreComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryCoreComponent(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;

	FOnRefreshInventory OnRefreshInventory;
	FOnSwitchInventoryActivePanel OnSwitchedActivePanel;
	FOnRemovedFromInventoryArray OnRemovedFromInventoryArray;
	FOnAddedToInventoryArray OnAddedToInventoryArray;
	FOnHighlightInventorySlot OnHighlightInventorySlot;
	FOnCurrentWeightChanged OnCurrentWeightChanged;
	FOnMaxWeightChanged OnMaxWeightChanged;
	FOnOwnerGoldChanged OnOwnerGoldChanged;

	virtual void InitializeInventory(APlayerController* PC);
protected:
	// virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Default")
	TArray<APlayerState*> CurrentViewers;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
	TArray<UDataTable*> AllItemsFromDataTables;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
	TArray<FItemStack> SingleDTItems;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Data")
	TArray<FRandomizedLootTable> RandomizedItemsData;

	// Inventory
	TArray<TArray<FItemData>> InventoryPanelArray;

	UPROPERTY(VisibleAnywhere)
	TArray<FItemData> InventoryArrayForDebug;
	
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "1", ClampMax = "100", UIMin = "1", UIMax = "100"))
	int32 InventoryDefaultSize = 30;
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "4", ClampMax = "10", UIMin = "4", UIMax = "10"))
	int32 InventoryPanelSize = 4;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Data")
	TSet<EInventoryPanel> PanelsToUse{EInventoryPanel::P1};

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Variables")
	EInventoryPanel ActivePanel;
	// Weight
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	bool UseInventoryWeight;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	float MaxInventoryWeight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Variables")
	float CurrentInventoryWeight;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	bool RefreshOnClosingWeight;
	// gold
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	bool CheckOwnerGold;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Variables")
	float OwnerGold;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	bool CheckRemoveType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	bool UseInventorySize;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	ESortMethod DefaultSortingMethod;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	float ValueModifier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	bool SortInitialItems;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	TArray<EItemType> CanSortTypes {EItemType::Armor, EItemType::Weapon,EItemType::Shield,EItemType::Potion,EItemType::Food,EItemType::Ingredient,EItemType::Recipe,EItemType::QuestItem,EItemType::Other};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	TArray<EItemRarity> CanSortRarities{EItemRarity::Legendary,EItemRarity::Epic,EItemRarity::Superior,EItemRarity::Common,EItemRarity::Consumable,EItemRarity::Usable};
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables")
	TArray<ECraftingSubcategory> CanSortSubCategories{ECraftingSubcategory::ChestPlates,ECraftingSubcategory::Gloves,ECraftingSubcategory::Necklaces,ECraftingSubcategory::HeadArmor,ECraftingSubcategory::Legs,
	ECraftingSubcategory::Boots,ECraftingSubcategory::Swords,ECraftingSubcategory::Hammers,ECraftingSubcategory::Axes,ECraftingSubcategory::Shields};
	
protected:
	int GetIndexByEnum(EInventoryPanel Panel) const;
	EInventoryPanel GetEnumByIndex(int32 Index) const;
public:
	bool GetInventoryPanel(int32 Index,TArray<FItemData>& ItemArray) const;

	// ----------------------
	// Build
	// ----------------------
	
#pragma region BuildInventory
	void BuildInventory(EInventoryPanel Panel);
	void BuildInitialInventory();
#pragma endregion

#pragma region Function
	void ApplyChangesToInventoryArray(EInventoryPanel Panel,const TArray<FItemData>& Inventory);
	virtual bool TransferItemFromInventory(UInventoryCoreComponent* Sender,FItemData& ItemData,const FItemData& InSlotData,EInputMethod Method,AActor* OwningPlayer);
	bool TransferItemFromEquipment(UEquipmentComponent* Sender,FItemData ItemData,FItemData InSlotData,EInputMethod Method);
	void SortInventory(ESortMethod Method, EInventoryPanel SinglePanel,bool bEveryPanel);
	virtual void SwapItemsInInventory(FItemData FirstItemData, FItemData SecondItemData);
	virtual void MoveItemInInventory(FItemData SourceItemData, int32 TargetIndex);
	void SwitchActivePanel(EInventoryPanel Panel);
	
	void RandomizeInitialItems();
	bool RandomizeItemParameters(const FItemData& InItemData,FItemData& OutItemData);
	virtual void SplitItemsInInventory(UInventoryCoreComponent* Sender,FItemData ItemData,const FItemData& InSlotData, FItemData StackableLeft,
		EInputMethod Method,EItemDestination Initiator,EItemDestination Destination,AActor* OwningPlayer); // add
	virtual void ConfirmationPopupAccepted(UInventoryCoreComponent* Sender,FItemData ItemData,const FItemData& InSlotData,EInputMethod Method,
		EItemDestination Initiator,EItemDestination Destination,AActor* OwningPlayer); 
	
#pragma endregion

#pragma region Function Internals
	void PopulateInventory(); // BuildInitialInventory
	void AddInventoryRow(); // BuildInitialInventory
	virtual void RefreshInventoryOnAdd(FItemData& ItemData, int32 SlotIndex, EInventoryPanel LocalPanel); // AddItemToInventoryArray
	void SortByMethod(ESortMethod Method, EInventoryPanel SinglePanel, TArray<FItemData> Inventory); // SortInventory
#pragma endregion

#pragma region Get

	void GetInventoryAndSize(EInventoryPanel Panel, TArray<FItemData>& OutInventory,int32& OutSize) const;
	bool GetEmptyInventorySlot(const FItemData& InItemData,int32& OutEmptySlot);
	bool GetItemBySlot(EInventoryPanel Panel,int32 InSlotIndex, FItemData& OutItemData) const;	
	bool HasEnoughGold(const FItemData& InItemData) const;
	bool IsInventoryOverweight() const;
	EItemRemoveType GetItemRemoveType(const FItemData& InItemData) const;
	bool GetItemByID(FString ItemID,EInventoryPanel InSpecifiedPanel,FItemData& OutItemData) const;
	bool GetAllItemByID(FString ItemID,EInventoryPanel InSpecifiedPanel,TArray<FItemData>& OutItemDataArray) const;
	bool GetItemsQuantityInInventory(FItemData InItemData,int32& ItemsQuantity) const;
	TArray<FItemData> GetCombinedInventories() const;
	bool GetItemByData(const FItemData& InItemData,FItemData& OutItemData) const;
	TArray<FItemData> GetItemsOfSpecifiedType(EItemType Type) const;
	int32 GetAmountOfEmptySlots(EInventoryPanel Panel) const;
#pragma endregion
	
#pragma region Set
	virtual void AddItemToInventoryArray(FItemData& ItemData,int32 SlotIndex = -1);
	void AddToStackInInventory(FItemData& InItemData,int32 SlotIndex);
	void AddGoldToOwner(float Gold);
	void AddWeightToInventory(float Weight);
	bool AddItemWithSpecifiedQuantity(FItemData InItemData, int32 Quantity = 1);
		
	void RemoveItemFromInventoryArray(const FItemData& InItemData);
	void RemoveGoldFromOwner(float Gold);
	void RemoveWeightFromInventory(float Weight);
	void RemoveItemQuantity(FItemData InItemData, int32 RemoveQuantity);
	void ModifyItemValue(FItemData& InItemData) const;
	
#pragma endregion

#pragma region NetworkFunction
	// ----------------------
	// Network Replication
	// ----------------------
	void UpdateViewersInventory(EInventoryPanel Panel);
	void UpdateAddedItemInInventory(UInventoryCoreComponent* Inventory,const FItemData& InItemData);
	void UpdateViewersItem(FItemData ItemData,bool Remove);
	void UpdateRemovedItemInInventory(UInventoryCoreComponent* Inventory,const FItemData& InItemData);

	
	UFUNCTION(Server,Reliable)
	void AddViewer(APlayerState* PlayerState,UInventoryCoreComponent* Inventory);
	
	UFUNCTION(Server,Reliable)
	void RemoveViewer(APlayerState* PlayerState,UInventoryCoreComponent* Inventory);
	
	UFUNCTION(Server,Reliable)
	void Server_RemoveItemFromInventory(UInventoryCoreComponent* Inventory,FItemData ItemData);
	
	UFUNCTION(Server,Reliable)
	void Server_AddItemToInventory(UInventoryCoreComponent* Inventory,FItemData ItemData,int32 SlotIndex);
	
	UFUNCTION(Server,Reliable)
	void Server_UpdateItems(AActor* Actor);
	
	UFUNCTION(Server,Reliable)
	void Server_TransferItemFromInventory(UInventoryCoreComponent* Receiver,UInventoryCoreComponent* Sender,FItemData ItemData,
		FItemData InSlotData,EInputMethod Method,AActor* OwningPlayer);
	
	UFUNCTION(Server,Reliable)
	void Server_MoveItemToSlot(UInventoryCoreComponent* Inventory,EInventoryPanel Panel,int32 FromPosition,int32 ToPosition);
	
	UFUNCTION(Server,Reliable)
	void Server_SplitItemsInInventory(UInventoryCoreComponent* Receiver,UInventoryCoreComponent* Sender, FItemData ItemData,
		 FItemData InSlotData, FItemData StackableLeft,EInputMethod Method, EItemDestination Initiator,EItemDestination Destination, AActor* OwningPlayer);
	
	UFUNCTION(Server,Reliable)
	void Server_SortInventory(UInventoryCoreComponent* Inventory,ESortMethod Method,EInventoryPanel SinglePanel,bool EveryPanel);
	
	UFUNCTION(Server,Reliable)
	void Server_TransferItemFromEquipment(UEquipmentComponent* Sender,FItemData ItemData,FItemData InSlotData,EInputMethod Method);
	
	UFUNCTION(Server,Reliable)
	void Server_ConfirmationPopupAccepted(UInventoryCoreComponent* Receiver,UInventoryCoreComponent* Sender,FItemData ItemData,FItemData InSlotData,
		EInputMethod Method, EItemDestination Initiator,EItemDestination Destination, AActor* OwningPlayer);
	
	UFUNCTION(Client,Reliable)
	void Client_UpdateItems(UInventoryCoreComponent* Inventory, EInventoryPanel Panel,const TArray<FItemData>& ItemDatas);
	
	UFUNCTION(Client,Reliable)
	virtual void Client_TransferItemReturnValue(bool Success,const FString& FailureMessage);
	
	UFUNCTION(Client,Reliable)
	void Client_UpdateAddedItem(UInventoryCoreComponent* Inventory,FItemData ItemData);
	
	UFUNCTION(Client,Reliable)
	void Client_UpdateRemovedItem(UInventoryCoreComponent* Inventory,FItemData ItemData);
	
	UFUNCTION(NetMulticast,Reliable)
	void Multicast_SetOwnerGold(float NewGold);
	
	UFUNCTION(NetMulticast,Reliable)
	void Multicast_SetCurrentWeight(float NewWeight);
	
#pragma endregion

	// === 디버깅 ===
	UFUNCTION(BlueprintCallable, Category = "Debug", CallInEditor)
	void DebugLogActiveItems() const;
};
