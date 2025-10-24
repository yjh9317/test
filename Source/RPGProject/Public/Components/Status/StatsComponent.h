// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Item/Data/ItemDataStructs.h"
#include "StatsComponent.generated.h"

class UDataAsset_StatConfig;
class UDataTable;
struct FItemData;
class ABaseCharacter;

UENUM(BlueprintType)
enum class EStatModifierType : uint8
{
	Flat,        // +10 Attack
	Percentage,  // +20% Speed
	Override     // = 100 (최종값 강제)
};

USTRUCT(BlueprintType)
struct FStatModifier
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag ModifierTag;  // 고유 ID (아이템 ID, 버프 ID)

	UPROPERTY()
	EStatModifierType ModifierType = EStatModifierType::Flat;

	UPROPERTY()
	float ModifierValue = 0.0f;

	UPROPERTY()
	float Duration = -1.0f;  // -1 = 영구

	UPROPERTY()
	float StartTime = 0.0f;

	UPROPERTY()
	int32 Priority = 0;  // 적용 순서 (낮을수록 먼저)

	FStatModifier() = default;

	FStatModifier(const FGameplayTag& InTag, float InValue, 
		EStatModifierType InType = EStatModifierType::Flat, float InDuration = -1.0f)
		: ModifierTag(InTag), ModifierType(InType), ModifierValue(InValue), Duration(InDuration)
	{}

	bool IsExpired(float CurrentTime) const
	{
		return Duration > 0.0f && (CurrentTime >= StartTime + Duration);
	}

	float GetRemainingTime(float CurrentTime) const
	{
		if (Duration <= 0.0f) return -1.0f;
		return FMath::Max(0.0f, StartTime + Duration - CurrentTime);
	}
};

// FFastArraySerializerItem : 이것을 상속받으면, 각 항목은 네트워크 복제를 위한 **고유 ID(ReplicationID)**와 **변경 이력(ReplicationKey)**을 내부적으로 갖게 됨
// 시스템은 이 값들을 비교해서 어떤 항목이 변경되었는지 빠르게 파악

// MarkItemDirty : 배열 항목의 내용이 변경되었을 때 호출하는 함수
// 이 함수를 호출하면 해당 항목의 ReplicationKey가 변경되어, 서버가 클라이언트에게 "이 항목만" 업데이트하라고 알려줄 수 있음

USTRUCT(BlueprintType)
struct FStatEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()
public:
	UPROPERTY()
	FGameplayTag StatTag;

	UPROPERTY()
	float BaseValue = 0.0f;  // 기본값 (변하지 않음)

	UPROPERTY()
	float CurrentValue = 0.0f;  // 최종 계산된 값

	UPROPERTY()
	float MaxValue = 0.0f;

	UPROPERTY()
	TArray<FStatModifier> Modifiers;

	FStatEntry() = default;

	FStatEntry(const FGameplayTag& InTag, float InCurrent, float InMax)
		: StatTag(InTag), CurrentValue(InCurrent), MaxValue(InMax)
	{}

	void RecalculateValue()
	{
		float FinalValue = BaseValue;
		float FlatBonus = 0.0f;
		float PercentBonus = 1.0f;

		// Priority로 정렬
		Modifiers.Sort([](const FStatModifier& A, const FStatModifier& B)
		{
			return A.Priority < B.Priority;
		});

		// Modifier 적용
		for (const FStatModifier& Mod : Modifiers)
		{
			switch (Mod.ModifierType)
			{
			case EStatModifierType::Flat:
				FlatBonus += Mod.ModifierValue;
				break;
			case EStatModifierType::Percentage:
				PercentBonus *= (1.0f + Mod.ModifierValue);
				break;
			case EStatModifierType::Override:
				FinalValue = Mod.ModifierValue;
				return;  // Override는 다른 Modifier 무시
			}
		}

		// 최종 계산: (Base + Flat) * Percentage
		FinalValue = (FinalValue + FlatBonus) * PercentBonus;
		CurrentValue = FMath::Max(0.0f, FinalValue);
	}
};

USTRUCT(BlueprintType)
struct FStatArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FStatEntry> Items;

	UPROPERTY(NotReplicated)
	TObjectPtr<UStatsComponent> OwnerComponent;

	// FFastArraySerializer 필수 함수 : 네트워크용 데이터로 직렬화/변환
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FStatEntry, FStatArray>(
			Items, DeltaParms, *this);
	}

	// 아이템 추가/변경 시 호출됨
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
};

template<>
struct TStructOpsTypeTraits<FStatArray> : public TStructOpsTypeTraitsBase2<FStatArray>
{
	enum
	{
		WithNetDeltaSerializer = true,
	};
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChanged, const FGameplayTag&, StatTag, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStatModified, const FGameplayTag&, StatTag, float, OldValue, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatMaxValueChanged, const FGameplayTag&, StatTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRefreshStats);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RPGPROJECT_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatsComponent();
	void OnStatsReplicated();
	
	UPROPERTY(BlueprintAssignable)
	FOnStatChanged OnStatChanged;

	UPROPERTY(BlueprintAssignable)
	FOnStatModified OnStatModified;

	UPROPERTY(BlueprintAssignable)
	FOnStatMaxValueChanged OnStatMaxValueChanged;

	UPROPERTY(BlueprintAssignable)
	FOnRefreshStats RefreshStats;

protected:
	UPROPERTY(Replicated)
	FStatArray StatArray;

	UPROPERTY(Transient)
	TMap<FGameplayTag, int32> StatIndexCache;

	UPROPERTY(Transient)
	TMap<FGameplayTag, FTimerHandle> ModifierTimers;

	UPROPERTY(EditDefaultsOnly,Category= "Stat | Data")
	TObjectPtr<UDataAsset_StatConfig> DataAsset_StatConfig;
	
	UPROPERTY(Transient)
	TObjectPtr<ABaseCharacter> OwnerCharacter;
	
private:
	FTimerHandle StatRecalculationTimer;
	bool bNeedsStatRecalculation = false;

public:

#pragma region Core_Stat_Functions

	float GetBaseStatValue(const FGameplayTag& StatTag) const;
	float GetStatValue(const FGameplayTag& StatTag) const;
	float GetMaxStatValue(const FGameplayTag& StatTag) const;
	float GetStatPercentage(const FGameplayTag& StatTag) const;
	void SetStatValue(const FGameplayTag& StatTag, float Value);
	void SetMaxStatValue(const FGameplayTag& StatTag, float MaxValue);
	bool HasStat(const FGameplayTag& StatTag) const;
	TArray<FGameplayTag> GetAllStatTags() const;

#pragma endregion
	
	void AddStatModifier(const FGameplayTag& StatTag, const FStatModifier& Modifier);
	void RemoveStatModifier(const FGameplayTag& StatTag, const FGameplayTag& ModifierTag);
	void RemoveAllModifiers(const FGameplayTag& StatTag);
	TArray<FStatModifier> GetStatModifiers(const FGameplayTag& StatTag) const;
	
	void AddItemStatBonus(const FGameplayTag& StatTag, const FGameplayTag& ItemID,float BonusValue, EStatModifierType ModifierType = EStatModifierType::Flat);
	void RemoveItemStatBonus(const FGameplayTag& StatTag, const FGameplayTag& ItemID);
	void AddTemporaryStatBuff(const FGameplayTag& StatTag, const FGameplayTag& BuffID,float BonusValue, float Duration,EStatModifierType ModifierType = EStatModifierType::Flat);
	void RemoveStatBuff(const FGameplayTag& StatTag, const FGameplayTag& BuffID);
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

private:
	void InitializeStats();
	float ClampStatValue(const FGameplayTag& StatTag, float Value) const;
	void OnStatValueChanged(const FGameplayTag& StatTag, float OldValue, float NewValue);
	void RebuildStatCache();
	// void RecalculateStat(const FGameplayTag& StatTag);
	void CleanupExpiredModifiers();
	void SetupModifierTimer(const FGameplayTag& StatTag, float Duration);
	void OnModifierExpired(FGameplayTag StatTag);
};
