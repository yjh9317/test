// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Item/Data/ItemDataStructs.h"
#include "Net/UnrealNetwork.h"
#include "StatsComponent.generated.h"

class UDataTable;
struct FItemData;
class ABaseCharacter;

// 스탯 상수 정의
namespace StatConstants
{
    static constexpr float MIN_STAT_VALUE = 0.0f;
    static constexpr float DEFAULT_MAX_HEALTH = 100.0f;
    static constexpr float DEFAULT_MAX_MANA = 100.0f;
    static constexpr float DEFAULT_MAX_STAMINA = 100.0f;
}

// 스탯 초기화 데이터
USTRUCT(BlueprintType)
struct FInitialStatData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Character.Stat"))
    FGameplayTag StatTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0"))
    float BaseValue = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = "0.0"))
    float MaxValue = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsPercentage = false;

    FInitialStatData()
    {
        StatTag = FGameplayTag();
        BaseValue = 0.0f;
        MaxValue = 0.0f;
        bIsPercentage = false;
    }

    FInitialStatData(const FGameplayTag& InStatTag, float InBaseValue, float InMaxValue = 0.0f, bool bInIsPercentage = false)
        : StatTag(InStatTag)
        , BaseValue(InBaseValue)
        , MaxValue(InMaxValue > 0.0f ? InMaxValue : InBaseValue)
        , bIsPercentage(bInIsPercentage)
    {
    }
};

// 스탯 수정자 (임시 보너스/페널티)
USTRUCT(BlueprintType)
struct FStatModifier
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag ModifierTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ModifierValue = 0.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsMultiplier = false; // true면 곱셈, false면 덧셈

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = -1.0f; // -1이면 영구

    UPROPERTY(BlueprintReadOnly)
    float StartTime = 0.0f;

    FStatModifier()
    {
        ModifierTag = FGameplayTag();
        ModifierValue = 0.0f;
        bIsMultiplier = false;
        Duration = -1.0f;
        StartTime = 0.0f;
    }

    bool IsExpired(float CurrentTime) const
    {
        return Duration > 0.0f && CurrentTime >= StartTime + Duration;
    }

    float GetRemainingTime(float CurrentTime) const
    {
        if (Duration <= 0.0f) return -1.0f;
        return FMath::Max(0.0f, StartTime + Duration - CurrentTime);
    }
};

// 델리게이트 선언
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStatChanged, const FGameplayTag&, StatTag, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnStatModified, const FGameplayTag&, StatTag, float, OldValue, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStatMaxValueChanged, const FGameplayTag&, StatTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRefreshStats);

// 레벨 관련 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGainedExperience, float, NewEXP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUp, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCurrentExpChanged);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RPGPROJECT_API UStatsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStatsComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	// === 델리게이트 ===
	UPROPERTY(BlueprintAssignable)
	FOnStatChanged OnStatChanged;

	UPROPERTY(BlueprintAssignable)
	FOnStatModified OnStatModified;

	UPROPERTY(BlueprintAssignable)
	FOnStatMaxValueChanged OnStatMaxValueChanged;

	UPROPERTY(BlueprintAssignable)
	FOnRefreshStats RefreshStats;

	// 레벨 관련 델리게이트
	UPROPERTY(BlueprintAssignable)
	FOnGainedExperience OnGainedExperience;

	UPROPERTY(BlueprintAssignable)
	FOnLevelUp OnLevelUp;

	UPROPERTY(BlueprintAssignable)
	FOnLevelChanged OnLevelChanged;

	UPROPERTY(BlueprintAssignable)
	FOnCurrentExpChanged OnCurrentExpChanged;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Leveling")
	float CurrentExperience = 0.0f;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Leveling")
	int32 CurrentLevel = 1;
protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	// === 스탯 데이터 ===
	UPROPERTY(EditDefaultsOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	TArray<FInitialStatData> InitialStats;
	
	TMap<FGameplayTag, float> Attributes;
	TMap<FGameplayTag, float> MaxValueAttributes;

	// === 스탯 수정자 ===
	TMap<FGameplayTag, TArray<FStatModifier>> StatModifiers;

	// === 레벨링 시스템 ===
	UPROPERTY(EditDefaultsOnly, Category = "Leveling", meta = (ClampMin = "1"))
	int32 MaxLevel = 100;

	UPROPERTY(EditDefaultsOnly, Category = "Leveling")
	float BaseExperienceRequired = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Leveling", meta = (ClampMin = "1.0"))
	float ExperienceMultiplier = 1.2f;

private:
	UPROPERTY(Transient)
	TObjectPtr<ABaseCharacter> OwnerCharacter;

	// 스탯 재계산 타이머
	FTimerHandle StatRecalculationTimer;
	bool bNeedsStatRecalculation = false;

	// 내부 함수들
	void RecalculateAllStats();
	void RecalculateStat(const FGameplayTag& StatTag);
	float CalculateStatWithModifiers(const FGameplayTag& StatTag, float BaseValue) const;
	void CleanupExpiredModifiers();

public:

#pragma region Core_Stat_Functions

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetStatValue(const FGameplayTag& StatTag) const;

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetMaxStatValue(const FGameplayTag& StatTag) const;

	UFUNCTION(BlueprintPure, Category = "Stats")
	float GetStatPercentage(const FGameplayTag& StatTag) const;

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetStatValue(const FGameplayTag& StatTag, float Value);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void SetMaxStatValue(const FGameplayTag& StatTag, float MaxValue);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void AddStatValue(const FGameplayTag& StatTag, float AddValue);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	void RemoveStatValue(const FGameplayTag& StatTag, float RemoveValue);

	UFUNCTION(BlueprintCallable, Category = "Stats")
	bool HasStat(const FGameplayTag& StatTag) const;

	UFUNCTION(BlueprintPure, Category = "Stats")
	TArray<FGameplayTag> GetAllStatTags() const;

#pragma endregion

#pragma region Stat_Modifiers

	UFUNCTION(BlueprintCallable, Category = "Stats|Modifiers")
	void AddStatModifier(const FGameplayTag& StatTag, const FStatModifier& Modifier);

	UFUNCTION(BlueprintCallable, Category = "Stats|Modifiers")
	void RemoveStatModifier(const FGameplayTag& StatTag, const FGameplayTag& ModifierTag);

	UFUNCTION(BlueprintCallable, Category = "Stats|Modifiers")
	void RemoveAllStatModifiers(const FGameplayTag& StatTag);

	UFUNCTION(BlueprintPure, Category = "Stats|Modifiers")
	TArray<FStatModifier> GetStatModifiers(const FGameplayTag& StatTag) const;

	UFUNCTION(BlueprintCallable, Category = "Stats|Modifiers")
	void AddTemporaryStatBonus(const FGameplayTag& StatTag, float BonusValue, float Duration, bool bIsMultiplier = false);

	UFUNCTION(BlueprintCallable, Category = "Stats|Modifiers")
	void AddPermanentStatBonus(const FGameplayTag& StatTag, float BonusValue, bool bIsMultiplier = false);

#pragma endregion

#pragma region Item_Integration

	UFUNCTION(BlueprintCallable, Category = "Stats|Items")
	void AddItemStats(const FItemData& ItemData);

	UFUNCTION(BlueprintCallable, Category = "Stats|Items")
	void RemoveItemStats(const FItemData& ItemData);

#pragma endregion

#pragma region Leveling_System

	UFUNCTION(BlueprintPure, Category = "Stats|Leveling")
	int32 GetCurrentLevel() const { return CurrentLevel; }

	UFUNCTION(BlueprintPure, Category = "Stats|Leveling")
	float GetCurrentExperience() const { return CurrentExperience; }

	UFUNCTION(BlueprintPure, Category = "Stats|Leveling")
	float GetExperienceRequiredForLevel(int32 Level) const;

	UFUNCTION(BlueprintPure, Category = "Stats|Leveling")
	float GetExperienceRequiredForNextLevel() const;

	UFUNCTION(BlueprintPure, Category = "Stats|Leveling")
	float GetExperiencePercentageToNextLevel() const;

	UFUNCTION(BlueprintCallable, Category = "Stats|Leveling")
	void AddExperience(float ExperienceAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats|Leveling")
	void SetLevel(int32 NewLevel);

	UFUNCTION(BlueprintCallable, Category = "Stats|Leveling")
	bool CanLevelUp() const;

#pragma endregion

#pragma region Utility_Functions

	UFUNCTION(BlueprintCallable, Category = "Stats|Utility")
	void InitializeStats();

	UFUNCTION(BlueprintCallable, Category = "Stats|Utility")
	void FixAnyMiscalculations();

	UFUNCTION(BlueprintCallable, Category = "Stats|Utility")
	void ResetAllStats();

	UFUNCTION(BlueprintCallable, Category = "Stats|Utility")
	void RestoreStatToMax(const FGameplayTag& StatTag);

	UFUNCTION(BlueprintCallable, Category = "Stats|Utility")
	void RestoreAllStatsToMax();

	UFUNCTION(BlueprintPure, Category = "Stats|Utility")
	bool IsStatAtMax(const FGameplayTag& StatTag) const;

	UFUNCTION(BlueprintPure, Category = "Stats|Utility")
	bool IsStatEmpty(const FGameplayTag& StatTag) const;

#pragma endregion

#pragma region Health_Helpers

	UFUNCTION(BlueprintPure, Category = "Stats|Health")
	float GetHealth() const;

	UFUNCTION(BlueprintPure, Category = "Stats|Health")
	float GetMaxHealth() const;

	UFUNCTION(BlueprintPure, Category = "Stats|Health")
	float GetHealthPercentage() const;

	UFUNCTION(BlueprintCallable, Category = "Stats|Health")
	void SetHealth(float NewHealth);

	UFUNCTION(BlueprintCallable, Category = "Stats|Health")
	void AddHealth(float HealthAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats|Health")
	void RemoveHealth(float HealthAmount);

	UFUNCTION(BlueprintPure, Category = "Stats|Health")
	bool IsAlive() const;

	UFUNCTION(BlueprintPure, Category = "Stats|Health")
	bool IsAtFullHealth() const;

#pragma endregion

#pragma region Mana_Helpers

	UFUNCTION(BlueprintPure, Category = "Stats|Mana")
	float GetMana() const;

	UFUNCTION(BlueprintPure, Category = "Stats|Mana")
	float GetMaxMana() const;

	UFUNCTION(BlueprintPure, Category = "Stats|Mana")
	float GetManaPercentage() const;

	UFUNCTION(BlueprintCallable, Category = "Stats|Mana")
	void SetMana(float NewMana);

	UFUNCTION(BlueprintCallable, Category = "Stats|Mana")
	void AddMana(float ManaAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats|Mana")
	void RemoveMana(float ManaAmount);

	UFUNCTION(BlueprintPure, Category = "Stats|Mana")
	bool HasEnoughMana(float RequiredMana) const;

#pragma endregion

#pragma region Stamina_Helpers

	UFUNCTION(BlueprintPure, Category = "Stats|Stamina")
	float GetStamina() const;

	UFUNCTION(BlueprintPure, Category = "Stats|Stamina")
	float GetMaxStamina() const;

	UFUNCTION(BlueprintPure, Category = "Stats|Stamina")
	float GetStaminaPercentage() const;

	UFUNCTION(BlueprintCallable, Category = "Stats|Stamina")
	void SetStamina(float NewStamina);

	UFUNCTION(BlueprintCallable, Category = "Stats|Stamina")
	void AddStamina(float StaminaAmount);

	UFUNCTION(BlueprintCallable, Category = "Stats|Stamina")
	void RemoveStamina(float StaminaAmount);

	UFUNCTION(BlueprintPure, Category = "Stats|Stamina")
	bool HasEnoughStamina(float RequiredStamina) const;

#pragma endregion

#pragma region Network_Functions
	
	UFUNCTION(Server, Reliable, Category = "Stats|Network")
	void ServerAddStatValue(const FGameplayTag& StatTag, float Value);
	
	UFUNCTION(Server, Reliable, Category = "Stats|Network")
	void ServerRemoveStatValue(const FGameplayTag& StatTag, float Value);
	
	UFUNCTION(Server, Reliable, Category = "Stats|Network")
	void ServerSetStatValue(const FGameplayTag& StatTag, float Value);
	
	UFUNCTION(Client, Reliable, Category = "Stats|Network")
	void ClientStatValueChanged(const FGameplayTag& StatTag, float StatValue);
	
	UFUNCTION(Client, Reliable, Category = "Stats|Network")
	void ClientInitializeStats();

	UFUNCTION(Server, Reliable, Category = "Stats|Network")
	void ServerAddExperience(float ExperienceAmount);

#pragma endregion

protected:
	// === 내부 유틸리티 ===
	float ClampStatValue(const FGameplayTag& StatTag, float Value) const;
	void OnStatValueChanged(const FGameplayTag& StatTag, float OldValue, float NewValue);
	void CheckForLevelUp();
	void OnLevelUpInternal(int32 NewLevel);

	// === 네트워크 콜백 ===
	UFUNCTION()
	void OnRep_Attributes();

	UFUNCTION()
	void OnRep_MaxValueAttributes();

	UFUNCTION()
	void OnRep_CurrentLevel();

	UFUNCTION()
	void OnRep_CurrentExperience();

	// === 초기화 함수 ===
	void SetupDefaultStats();
};
