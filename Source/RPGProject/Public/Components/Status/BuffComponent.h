#pragma once

#include "CoreMinimal.h"
#include "Inventory/Components/EquipmentComponent.h"
#include "Components/ActorComponent.h"
#include "Item/Data/ItemDataStructs.h"
#include "BuffComponent.generated.h"


class AAbstractConsumable;
class ABaseCharacter;
class UStatsComponent;
class UEquipmentComponent;

// 버프 타입
UENUM(BlueprintType)
enum class EBuffType : uint8
{
	Temporary       UMETA(DisplayName = "Temporary"),         // 시간 제한 버프
	Permanent       UMETA(DisplayName = "Permanent"),         // 영구 버프
	Consumable      UMETA(DisplayName = "Consumable"),        // 소모품 버프
	Equipment       UMETA(DisplayName = "Equipment"),         // 장비 버프
	Skill           UMETA(DisplayName = "Skill"),             // 스킬 버프
	Debuff          UMETA(DisplayName = "Debuff")             // 디버프
};

// 버프 스택 타입
UENUM(BlueprintType)
enum class EBuffStackType : uint8
{
	None            UMETA(DisplayName = "None"),              // 스택 불가
	Stack           UMETA(DisplayName = "Stack"),             // 효과 중첩
	Refresh         UMETA(DisplayName = "Refresh"),           // 지속시간 갱신
	Replace         UMETA(DisplayName = "Replace")            // 대체
};

USTRUCT(BlueprintType)
struct FBuffInfo
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Categories = "Buff"))
    FGameplayTag BuffTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText BuffName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText BuffDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UTexture2D> BuffIcon;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBuffType BuffType = EBuffType::Temporary;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBuffStackType StackType = EBuffStackType::None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxStacks = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 30.0f; // -1이면 영구

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bShowInUI = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsBeneficial = true; // false면 디버프

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Priority = 0; // 높을수록 우선순위

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TMap<FGameplayTag, float> StatModifiers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTagContainer GrantedTags;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTagContainer BlockedTags;

    FBuffInfo()
    {
        BuffTag = FGameplayTag();
        BuffName = FText::GetEmpty();
        BuffDescription = FText::GetEmpty();
        BuffIcon = nullptr;
        BuffType = EBuffType::Temporary;
        StackType = EBuffStackType::None;
        MaxStacks = 1;
        Duration = 30.0f;
        bShowInUI = true;
        bIsBeneficial = true;
        Priority = 0;
    }
};

USTRUCT(BlueprintType)
struct FActiveBuff
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FBuffInfo BuffInfo;

    UPROPERTY(BlueprintReadOnly)
    int32 CurrentStacks = 1;

    UPROPERTY(BlueprintReadOnly)
    float RemainingDuration = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    float StartTime = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<AActor> Instigator = nullptr;

    UPROPERTY(BlueprintReadOnly)
    TObjectPtr<AAbstractConsumable> SourceConsumable = nullptr;

    FActiveBuff()
    {
        CurrentStacks = 1;
        RemainingDuration = 0.0f;
        StartTime = 0.0f;
        Instigator = nullptr;
        SourceConsumable = nullptr;
    }

    FActiveBuff(const FBuffInfo& InBuffInfo, AActor* InInstigator = nullptr)
        : BuffInfo(InBuffInfo)
        , CurrentStacks(1)
        , RemainingDuration(InBuffInfo.Duration)
        , Instigator(InInstigator)
        , SourceConsumable(nullptr)
    {
        StartTime = 0.0f; // 실제 적용 시 설정
    }

    bool IsExpired(float CurrentTime) const
    {
        return BuffInfo.Duration > 0.0f && CurrentTime >= StartTime + BuffInfo.Duration;
    }

    float GetRemainingTime(float CurrentTime) const
    {
        if (BuffInfo.Duration <= 0.0f) return -1.0f; // 영구
        return FMath::Max(0.0f, StartTime + BuffInfo.Duration - CurrentTime);
    }

    float GetElapsedTime(float CurrentTime) const
    {
        return CurrentTime - StartTime;
    }

    float GetProgressPercentage(float CurrentTime) const
    {
        if (BuffInfo.Duration <= 0.0f) return 1.0f;
        return FMath::Clamp(GetElapsedTime(CurrentTime) / BuffInfo.Duration, 0.0f, 1.0f);
    }
};

// 버프 이벤트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuffApplied, const FActiveBuff&, AppliedBuff);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuffRemoved, const FActiveBuff&, RemovedBuff);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBuffExpired, const FActiveBuff&, ExpiredBuff);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBuffStackChanged, const FGameplayTag&, BuffTag, int32, NewStacks);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBuffsRefreshed);

// 아이템 관련 델리게이트 (기존 호환성)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnBuffItemUsed, EItemSlot, UsedSlot, float, Cooldown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInitCircularAnimation, const FGameplayTag&, StatTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClearBuffsBar);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBuffComponent();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// === 델리게이트 ===
	UPROPERTY(BlueprintAssignable)
	FOnBuffApplied OnBuffApplied;

	UPROPERTY(BlueprintAssignable)
	FOnBuffRemoved OnBuffRemoved;

	UPROPERTY(BlueprintAssignable)
	FOnBuffExpired OnBuffExpired;

	UPROPERTY(BlueprintAssignable)
	FOnBuffStackChanged OnBuffStackChanged;

	UPROPERTY(BlueprintAssignable)
	FOnBuffsRefreshed OnBuffsRefreshed;

	// 기존 호환성을 위한 델리게이트들
	UPROPERTY(BlueprintAssignable)
	FOnBuffItemUsed OnBuffItemUsed;

	UPROPERTY(BlueprintAssignable)
	FOnInitCircularAnimation OnInitCircularAnimation;

	UPROPERTY(BlueprintAssignable)
	FOnClearBuffsBar OnClearBuffsBar;

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	UPROPERTY(Transient)
	TObjectPtr<ABaseCharacter> OwnerCharacter;

	UPROPERTY(Transient)
	TObjectPtr<UStatsComponent> StatsComponent;

	// === 활성 버프들 ===
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Buffs")
	TArray<FActiveBuff> ActiveBuffs;

	// === 버프 설정 ===
	UPROPERTY(EditDefaultsOnly, Category = "Buff Settings")
	int32 MaxActiveBuffs = 20;

	UPROPERTY(EditDefaultsOnly, Category = "Buff Settings")
	bool bAutoRemoveExpiredBuffs = true;

	UPROPERTY(EditDefaultsOnly, Category = "Buff Settings")
	float BuffUpdateInterval = 0.1f;

	// === 기존 호환성을 위한 변수들 ===
	UPROPERTY()
	EItemSlot UsedSlot;

	UPROPERTY()
	TMap<FGameplayTag, float> StatsDecimal;

	UPROPERTY()
	TMap<TSubclassOf<AAbstractConsumable>, TObjectPtr<AAbstractConsumable>> Buffs;

private:
	// === 내부 변수 ===
	FTimerHandle BuffUpdateTimer;
	bool bNeedsStatRecalculation = false;

	// === 내부 함수들 ===
	void UpdateActiveBuffs();
	void RemoveExpiredBuffs();
	void ApplyBuffEffects(const FActiveBuff& Buff, bool bApply = true);
	void RecalculateBuffEffects();
	FActiveBuff* FindActiveBuff(const FGameplayTag& BuffTag);
	bool CanApplyBuff(const FBuffInfo& BuffInfo) const;
	void SortBuffsByPriority();

public:
//
// #pragma region Core_Buff_Functions
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs")
// 	bool ApplyBuff(const FBuffInfo& BuffInfo, AActor* Instigator = nullptr);
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs")
// 	bool ApplyBuffByTag(const FGameplayTag& BuffTag, AActor* Instigator = nullptr);
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs")
// 	bool RemoveBuff(const FGameplayTag& BuffTag);
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs")
// 	void RemoveAllBuffs();
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs")
// 	void RemoveBuffsOfType(EBuffType BuffType);
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs")
// 	void RemoveDebuffs();
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs")
// 	bool RefreshBuff(const FGameplayTag& BuffTag, float AdditionalDuration = 0.0f);
//
// #pragma endregion
//
// #pragma region Buff_Queries
//
// 	UFUNCTION(BlueprintPure, Category = "Buffs|Query")
// 	bool HasBuff(const FGameplayTag& BuffTag) const;
//
// 	UFUNCTION(BlueprintPure, Category = "Buffs|Query")
// 	bool HasAnyBuffs(const FGameplayTagContainer& BuffTags) const;
//
// 	UFUNCTION(BlueprintPure, Category = "Buffs|Query")
// 	bool HasAllBuffs(const FGameplayTagContainer& BuffTags) const;
//
// 	UFUNCTION(BlueprintPure, Category = "Buffs|Query")
// 	FActiveBuff GetActiveBuff(const FGameplayTag& BuffTag) const;
//
// 	UFUNCTION(BlueprintPure, Category = "Buffs|Query")
// 	TArray<FActiveBuff> GetActiveBuffs() const { return ActiveBuffs; }
//
// 	UFUNCTION(BlueprintPure, Category = "Buffs|Query")
// 	TArray<FActiveBuff> GetBuffsOfType(EBuffType BuffType) const;
//
// 	UFUNCTION(BlueprintPure, Category = "Buffs|Query")
// 	TArray<FActiveBuff> GetBeneficialBuffs() const;
//
// 	UFUNCTION(BlueprintPure, Category = "Buffs|Query")
// 	TArray<FActiveBuff> GetDebuffs() const;
//
// 	UFUNCTION(BlueprintPure, Category = "Buffs|Query")
// 	int32 GetBuffStacks(const FGameplayTag& BuffTag) const;
//
// 	UFUNCTION(BlueprintPure, Category = "Buffs|Query")
// 	float GetBuffRemainingTime(const FGameplayTag& BuffTag) const;
//
// 	UFUNCTION(BlueprintPure, Category = "Buffs|Query")
// 	int32 GetActiveBuffCount() const { return ActiveBuffs.Num(); }
//
// #pragma endregion
//
// #pragma region Stack_Management
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs|Stack")
// 	bool AddBuffStack(const FGameplayTag& BuffTag, int32 StacksToAdd = 1);
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs|Stack")
// 	bool RemoveBuffStack(const FGameplayTag& BuffTag, int32 StacksToRemove = 1);
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs|Stack")
// 	void SetBuffStacks(const FGameplayTag& BuffTag, int32 NewStacks);
//
// 	UFUNCTION(BlueprintPure, Category = "Buffs|Stack")
// 	int32 GetMaxBuffStacks(const FGameplayTag& BuffTag) const;
//
// #pragma endregion
//
// #pragma region Duration_Management
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs|Duration")
// 	void ExtendBuffDuration(const FGameplayTag& BuffTag, float AdditionalTime);
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs|Duration")
// 	void SetBuffDuration(const FGameplayTag& BuffTag, float NewDuration);
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs|Duration")
// 	void PauseBuffDuration(const FGameplayTag& BuffTag, bool bPause);
//
// 	UFUNCTION(BlueprintPure, Category = "Buffs|Duration")
// 	float GetBuffProgress(const FGameplayTag& BuffTag) const;
//
// #pragma endregion
//
// #pragma region Consumable_Integration
//
// 	// 기존 시스템과의 호환성을 위한 함수들
// 	UFUNCTION(BlueprintCallable, Category = "Buffs|Consumables")
// 	bool TryToUseConsumableFromEquipmentSlot(EItemSlot Slot, float& Cooldown);
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs|Consumables")
// 	void AddConsumableBuffToStack(AAbstractConsumable* Buff);
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs|Consumables")
// 	bool IsBuffAlreadyInStack(TSubclassOf<AAbstractConsumable> BuffClass, AAbstractConsumable*& OutBuff);
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs|Consumables")
// 	void RemoveConsumableBuffFromStack(AAbstractConsumable* Buff);
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs|Animation")
// 	void InitializeCircularAnimation(UPARAM(meta = (Categories = "Character.Stat")) const FGameplayTag& StatTag);
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs|UI")
// 	void ClearBuffsBar();
//
// #pragma endregion
//
// #pragma region Stat_Effects
//
// 	UFUNCTION(BlueprintPure, Category = "Buffs|Stats")
// 	float GetBuffStatModifier(const FGameplayTag& StatTag) const;
//
// 	UFUNCTION(BlueprintPure, Category = "Buffs|Stats")
// 	TMap<FGameplayTag, float> GetAllBuffStatModifiers() const;
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs|Stats")
// 	void RecalculateStatModifiers();
//
// #pragma endregion
//
// #pragma region Tag_Effects
//
// 	UFUNCTION(BlueprintPure, Category = "Buffs|Tags")
// 	FGameplayTagContainer GetGrantedTags() const;
//
// 	UFUNCTION(BlueprintPure, Category = "Buffs|Tags")
// 	FGameplayTagContainer GetBlockedTags() const;
//
// 	UFUNCTION(BlueprintPure, Category = "Buffs|Tags")
// 	bool IsTagBlocked(const FGameplayTag& Tag) const;
//
// #pragma endregion
//
// #pragma region Settings
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs|Settings")
// 	void SetMaxActiveBuffs(int32 NewMax);
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs|Settings")
// 	void SetBuffUpdateInterval(float NewInterval);
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs|Settings")
// 	void SetAutoRemoveExpiredBuffs(bool bEnabled);
//
// #pragma endregion
//
// #pragma region Events
//
// 	UFUNCTION(BlueprintImplementableEvent, Category = "Buffs|Events")
// 	void OnBuffEffectApplied(const FGameplayTag& BuffTag, int32 Stacks);
//
// 	UFUNCTION(BlueprintImplementableEvent, Category = "Buffs|Events")
// 	void OnBuffEffectRemoved(const FGameplayTag& BuffTag);
//
// 	UFUNCTION(BlueprintImplementableEvent, Category = "Buffs|Events")
// 	void OnMaxBuffsReached();
//
// #pragma endregion
//
// #pragma region Utility
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs|Utility")
// 	void RefreshAllBuffs();
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs|Utility")
// 	void PurgeExpiredBuffs();
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs|Debug")
// 	void LogActiveBuffs() const;
//
// 	UFUNCTION(BlueprintCallable, Category = "Buffs|Debug")
// 	void SetDebugMode(bool bEnabled);
//
// #pragma endregion
//
// #pragma region Network_Functions
//
// 	// UFUNCTION(Server, Reliable, Category = "Buffs|Network")
// 	// void ServerApplyBuff(const FBuffInfo& BuffInfo, AActor* Instigator);
// 	//
// 	// UFUNCTION(Server, Reliable, Category = "Buffs|Network")
// 	// void ServerRemoveBuff(const FGameplayTag& BuffTag);
// 	//
// 	// UFUNCTION(NetMulticast, Reliable, Category = "Buffs|Network")
// 	// void MulticastBuffApplied(const FActiveBuff& AppliedBuff);
// 	//
// 	// UFUNCTION(NetMulticast, Reliable, Category = "Buffs|Network")
// 	// void MulticastBuffRemoved(const FGameplayTag& BuffTag);
//
// #pragma endregion
//
// protected:
// 	// === 네트워크 콜백 ===
// 	UFUNCTION()
// 	void OnRep_ActiveBuffs();
//
// 	// === 내부 유틸리티 ===
// 	bool SpawnConsumableActor(UEquipmentComponent* EquipmentComponent, float& LocalCooldown);
//
// private:
// 	// === 디버그 ===
// 	bool bDebugMode = false;
// 	void DebugLogBuff(const FString& Message) const;
};
