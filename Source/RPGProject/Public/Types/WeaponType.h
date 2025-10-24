#pragma once

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial UMETA(DisplayName = "Initial"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_PickedUp UMETA(DisplayName = "Picked Up"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	EWS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EBowState : uint8
{
	EBS_Default UMETA(DisplayName = "Default"),
	EBS_Drawing UMETA(DisplayName = "Drawing"),
	EBS_Firing UMETA(DisplayName = "Firing"),
	EBS_MAX UMETA(DisplayName = "MAX")
};



// USTRUCT()
// struct FWeaponCraftingInfo
// {
// 	GENERATED_STRUCTED_BODY()
//
// 	UPROPERTY(EditAnywhere, Category= "Crafting")
// 	TArray<FString> RequiredMaterials;  // 필요한 재료 목록
//
// 	UPROPERTY(EditAnywhere, Category= "Crafting")
// 	FString CraftedBy;  // 제작자 이름
//
// 	UPROPERTY(EditAnywhere, Category= "Crafting")
// 	bool bIsCrafted = false;  // 제작된 무기 여부
// };