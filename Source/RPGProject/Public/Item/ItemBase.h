
#pragma once

#include "CoreMinimal.h"
#include "Inventory/Components/InventoryCoreComponent.h"
#include "Item/Interactable/BaseInteractable.h"
#include "Item/Consumable/BaseConsumable.h"
#include "Item/Equippable/BaseEquippable.h"
#include "Item/Data/ItemDataStructs.h"
#include "ItemBase.generated.h"


UCLASS(BlueprintType,Blueprintable)
class RPGPROJECT_API UItemBase : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly,Category = "Item Info")
	FText Name;
	
	UPROPERTY(EditDefaultsOnly,Category = "Item Info")
	FText Description;

	UPROPERTY(EditDefaultsOnly,Category = "Item Info")
	FText GreaterLootText;

	UPROPERTY(EditDefaultsOnly,Category = "Item Details")
	TSoftObjectPtr<UTexture2D> Image;
	
	UPROPERTY(EditDefaultsOnly,Category = "Item Details")
	EItemType Type;
	
	UPROPERTY(EditDefaultsOnly,Category = "Item Details")
	EItemRarity Rarity;
	
	UPROPERTY(EditDefaultsOnly,Category = "Item Details")
	EInventoryPanel InventoryPanel = EInventoryPanel::P1;
	
	UPROPERTY(EditDefaultsOnly,Category = "Item Details")
	EWeaponType WeaponType;
	
	UPROPERTY(EditDefaultsOnly,Category = "Item Details")
	EWeaponSubtype WeaponSubtype;

	UPROPERTY(EditDefaultsOnly,Category = "Item Stats")
	float Weight;
	
	UPROPERTY(EditDefaultsOnly,Category = "Item Stats")
	float Value;

	UPROPERTY(EditDefaultsOnly,Category = "Item Stats")
	int32 RequiredLevel;

	UPROPERTY(EditDefaultsOnly,Category = "Item Stats")
	TArray<FItemStatRow> Stats;
	
	UPROPERTY(EditDefaultsOnly,Category = "Stacking")
	bool Stackable;
	
	UPROPERTY(EditDefaultsOnly,Category = "Item Durability")
	bool UseDurability;

	UPROPERTY(EditDefaultsOnly, Category= "Item Usage")
	EItemUseType UseType;
	
	UPROPERTY(EditDefaultsOnly, Category= "Item Usage")
	EItemRemoveType RemoveType;

	UPROPERTY(EditDefaultsOnly, Category= "Item Usage")
	FText TextDocument;

	UPROPERTY(EditDefaultsOnly, Category= "Item Usage")
	FString CraftingIDToUnlock;

	UPROPERTY(EditDefaultsOnly, Category= "Loot Randomization")
	int32 DropPercentage;
	
	UPROPERTY(EditDefaultsOnly, Category= "Loot Randomization")
	TSet<TSubclassOf<UItemBase>> LootItemClass;

	UPROPERTY(EditDefaultsOnly, Category= "Loot Randomization")
	int32 MinRandQuantity = 1;
	
	UPROPERTY(EditDefaultsOnly, Category= "Loot Randomization")
	int32 MaxRandQuantity = 100;
	
	UPROPERTY(EditDefaultsOnly, Category= "Loot Randomization")
	int32 MinRandDurability = 1;
	
	UPROPERTY(EditDefaultsOnly, Category= "Loot Randomization")
	int32 MaxRandDurability = 100;

	// Equippable Class
	UPROPERTY(EditDefaultsOnly, Category= "Item Equip")
	TSoftClassPtr<ABaseEquippable> EquippableClass;

	UPROPERTY(EditDefaultsOnly, Category= "Item Equip")
	TSoftObjectPtr<USkeletalMesh> SkeletalMesh;

	// Consumable Buff
	UPROPERTY(EditDefaultsOnly, Category= "Item Consumable Buff")
	TSoftClassPtr<ABaseConsumable> ConsumableClass;

	UPROPERTY(EditDefaultsOnly, Category= "Item Interactable")
	TSoftClassPtr<ABaseInteractable> InteractableClass;
};


