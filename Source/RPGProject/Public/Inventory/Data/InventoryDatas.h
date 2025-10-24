#pragma once

#include "CoreMinimal.h"
#include "InventoryDatas.generated.h"

UENUM()
enum class ESortMethod
{
	Quicksort,
	ByType,
	ByRarity,
	ByValue,
	ByWeight,
	ByCraftingSubcategory
};

UENUM()
enum class EInventoryPanel
{
	P1,
	P2,
	P3,
	P4,
	P5,
	P6,
	P7,
	P8,
	P9,
	NONE,
};

USTRUCT(BlueprintType)
struct FItemStack : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FDataTableRowHandle TableAndRow;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int32 Quantity;
};

USTRUCT(BlueprintType)
struct FRandomizedLootTable
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	UDataTable* DataTable;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int32 MinLootItems;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	int32 MaxLootItems;
};