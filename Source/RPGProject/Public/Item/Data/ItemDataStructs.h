#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataTable.h"
#include "Inventory/Data/InventoryDatas.h"
#include "ItemDataStructs.generated.h"

class UItemBase;

// -----------------------------------
// Buff
// -----------------------------------

UENUM(BlueprintType)
enum class EBuffEffectCategory : uint8
{
	None,
	AddInstantly,
	AddOverDuration,
	AddForDuration
};

UENUM(BlueprintType)
enum class EAdditionalBuffEffect: uint8
{
	None,
	Drunk,
	NightVision
};

// -----------------------------------
// Stat
// -----------------------------------


UENUM(BlueprintType)
enum class EStatCategory: uint8
{
	None,
	Health,
	MaxHealth,
	Stamina,
	MaxStamina,
	Armor,
	Damage,
	AttackSpeed
};

// -----------------------------------
// Level
// -----------------------------------
USTRUCT(BlueprintType)
struct FLevelingSystem: public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	int32 Level = 0;

	UPROPERTY(EditDefaultsOnly)
	float NeededExp = 0.f;
};



// -----------------------------------
// Item
// -----------------------------------

UENUM(BlueprintType)
enum class EItemSlot: uint8
{
	NONE,
	HEAD,	
	GLOVES,
	NECK,
	CHESTPLATE,
	LEGS,
	BOOTS,
	WEAPON,
	SHIELD,
	POCKET1,
	POCKET2,
	POCKET3,
	POCKET4,
};

ENUM_RANGE_BY_COUNT(EItemSlot, EItemSlot::POCKET4);


UENUM(BlueprintType)
enum class EItemRemoveType: uint8
{
	Default,
	OnConfirmation,
	CannotBeRemoved
};

UENUM(BlueprintType)
enum class EItemUseType: uint8
{
	None,
	TextDocument,
	Currency,
	CraftingRecipe
};


UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	None,
	Common,
	Superior,
	Epic,
	Legendary,
	Consumable,
	Usable
};

// UMETA( DisplayName = "")
UENUM(BlueprintType)
enum class EItemType : uint8
{
	None,
	Armor,
	Weapon,
	Shield,
	Food,
	Potion,
	QuestItem,
	Other,
	Currency,
	Ingredient,
	Recipe
};

UENUM(BlueprintType)
enum class EItemDestination: uint8
{
	None,
	InventorySlot,
	EquipmentSlot,
	VendorSlot,
	StorageSlot,
	DropBar
};

USTRUCT(BlueprintType)
struct FItemStatRow
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	FGameplayTag StatTag;
	
	UPROPERTY(EditDefaultsOnly)
	float Value;
};

USTRUCT(BlueprintType)
struct FItemStatistics
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	float BuyValue;
	
	UPROPERTY(EditAnywhere)
	float SellValue;
};

USTRUCT(BlueprintType)
struct FItemWidgetData
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere)
	FText Name;

	UPROPERTY(EditAnywhere)
	FText Description;

	UPROPERTY(EditAnywhere)
	FText InteractionText;

	UPROPERTY(EditAnywhere)
	FText UsageText;

	UPROPERTY(EditAnywhere)
	UTexture2D* Icon;
};

USTRUCT(BlueprintType)
struct FItemNumericData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	float Weight;

	UPROPERTY(EditAnywhere)
	int32 MaxStackSize;

	UPROPERTY(EditAnywhere)
	bool bIsStackable;
};

USTRUCT(BlueprintType)
struct FItemData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category= "Item Data")
	FString ID;

	UPROPERTY(EditAnywhere, Category= "Item Data")
	EItemSlot EquipmentSlot = EItemSlot::NONE;

	UPROPERTY(EditAnywhere, Category= "Item Data")
	TSubclassOf<UItemBase> ItemClass;

	UPROPERTY(EditAnywhere, Category= "Item Data")
	int32 Quantity = 0;

	UPROPERTY(EditAnywhere, Category= "Item Data")
	int32 Durability = 0;

	UPROPERTY(EditAnywhere, Category= "Item Data")
	int32 Index = -1;

	UPROPERTY(EditAnywhere, Category= "Item Data")
	bool IsEquipped = false;

	UPROPERTY(EditAnywhere, Category= "Item Data")
	bool AlreadyUsed = false;

	UPROPERTY(EditAnywhere, Category= "Item Data")
	float ValueModifier = 0.f;

	bool operator==(const FItemData& Other) const {
		return ID == Other.ID;
	}
	
	FItemData& operator=(const FItemData& Other)
	{
		if (this != &Other)
		{
			ID = Other.ID;
			EquipmentSlot = Other.EquipmentSlot;
			ItemClass = Other.ItemClass;
			Quantity = Other.Quantity;
			Durability = Other.Durability;
			Index = Other.Index;
			IsEquipped = Other.IsEquipped;
			AlreadyUsed = Other.AlreadyUsed;
			ValueModifier = Other.ValueModifier;
		}
		return *this;
	}
};

// -----------------------------------
// Weapon
// -----------------------------------

UENUM(BlueprintType)
enum class EWeaponType: uint8
{
	None,
	OneHand,
	TwoHand,
	OffHand
};

UENUM(BlueprintType)
enum class EWeaponSubtype: uint8
{
	None,
	TwoHandedSword,
	Cane,
   	Bow,
	OneHandedSword,
	Shield,
	WarHammer,
	Axe,
	Mace
};

// -----------------------------------
// Craft
// -----------------------------------

UENUM(BlueprintType)
enum class ECraftingCategory: uint8
{
	None,
	Armor,
	Weapon
};

UENUM(BlueprintType)
enum class ECraftingSubcategory: uint8
{
	None,
	HeadArmor,
	ChestPlates,
	Gloves,
	Necklaces,
	Legs,
	Boots,
	Swords,
	Hammers,
	Axes,
	Shields
};

UENUM(BlueprintType)
enum class ECraftingStation: uint8
{
	None,
	Forge
};

USTRUCT(BlueprintType)
struct FCraftingData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	FString CraftingID;

	UPROPERTY(EditAnywhere)
	TArray<FItemStack> OutputItems;

	UPROPERTY(EditAnywhere)
	TArray<FItemStack> InputItems;

	UPROPERTY(EditAnywhere)
	bool StartLocked;

	UPROPERTY(EditAnywhere)
	float CraftingTime;
	
	UPROPERTY(EditAnywhere)
	float CraftingCost;

	UPROPERTY(EditAnywhere)
	ECraftingCategory Category;

	UPROPERTY(EditAnywhere)
	ECraftingSubcategory Subcategory;

	UPROPERTY(EditAnywhere)
	ECraftingStation CraftingStation;

	UPROPERTY(EditAnywhere)
	int32 CraftingQuantity;
	
	UPROPERTY(EditAnywhere)
	int32 MaxQuantity;

	bool operator==(const FCraftingData& Other) const
	{
		return this->CraftingID == Other.CraftingID;
	}
};

