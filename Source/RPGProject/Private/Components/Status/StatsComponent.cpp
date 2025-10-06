
#include "Components/Status/StatsComponent.h"

#include "Character/BaseCharacter.h"
#include "Item/ItemBase.h"
#include "Net/UnrealNetwork.h"

UStatsComponent::UStatsComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
	
	SetIsReplicatedByDefault(true);

	// 기본 스탯 설정
	SetupDefaultStats();
}


void UStatsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// DOREPLIFETIME(UStatsComponent, Attributes);
	// DOREPLIFETIME(UStatsComponent, MaxValueAttributes);
	// DOREPLIFETIME(UStatsComponent, CurrentLevel);
	// DOREPLIFETIME(UStatsComponent, CurrentExperience);
}

void UStatsComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ABaseCharacter>(GetOwner());
	
	if (GetOwnerRole() == ROLE_Authority)
	{
		InitializeStats();
	}
}



void UStatsComponent::BeginDestroy()
{
	// 델리게이트 정리
	OnStatChanged.Clear();
	OnStatModified.Clear();
	OnStatMaxValueChanged.Clear();
	RefreshStats.Clear();
	OnGainedExperience.Clear();
	OnLevelUp.Clear();
	OnLevelChanged.Clear();
	OnCurrentExpChanged.Clear();

	// 타이머 정리
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(StatRecalculationTimer);
	}

	Super::BeginDestroy();
}

void UStatsComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 만료된 수정자 정리
	CleanupExpiredModifiers();

	// 스탯 재계산이 필요한 경우
	if (bNeedsStatRecalculation)
	{
		RecalculateAllStats();
		bNeedsStatRecalculation = false;
	}
}

#pragma region Core_Stat_Functions

float UStatsComponent::GetStatValue(const FGameplayTag& StatTag) const
{
	const float* Value = Attributes.Find(StatTag);
	return Value ? *Value : 0.0f;
}

float UStatsComponent::GetMaxStatValue(const FGameplayTag& StatTag) const
{
	const float* Value = MaxValueAttributes.Find(StatTag);
	return Value ? *Value : 0.0f;
}

float UStatsComponent::GetStatPercentage(const FGameplayTag& StatTag) const
{
	float CurrentValue = GetStatValue(StatTag);
	float MaxValue = GetMaxStatValue(StatTag);
	
	if (MaxValue <= 0.0f)
	{
		return 0.0f;
	}
	
	return FMath::Clamp(CurrentValue / MaxValue, 0.0f, 1.0f);
}

void UStatsComponent::SetStatValue(const FGameplayTag& StatTag, float Value)
{
	if (!StatTag.IsValid())
	{
		return;
	}

	float ClampedValue = ClampStatValue(StatTag, Value);
	float OldValue = GetStatValue(StatTag);

	if (GetOwnerRole() == ROLE_Authority)
	{
		Attributes.Add(StatTag, ClampedValue);
		OnStatValueChanged(StatTag, OldValue, ClampedValue);

		// 클라이언트에 변경사항 전파
		ClientStatValueChanged(StatTag, ClampedValue);
	}
	else
	{
		// 클라이언트에서 서버로 요청
		ServerSetStatValue(StatTag, Value);
	}
}

void UStatsComponent::SetMaxStatValue(const FGameplayTag& StatTag, float MaxValue)
{
	if (!StatTag.IsValid() || MaxValue < 0.0f)
	{
		return;
	}

	MaxValueAttributes.Add(StatTag, MaxValue);
	
	// 현재 값이 최대값을 초과하면 조정
	float CurrentValue = GetStatValue(StatTag);
	if (CurrentValue > MaxValue)
	{
		SetStatValue(StatTag, MaxValue);
	}

	OnStatMaxValueChanged.Broadcast(StatTag);
}

void UStatsComponent::AddStatValue(const FGameplayTag& StatTag, float AddValue)
{
	if (!StatTag.IsValid() || AddValue == 0.0f)
	{
		return;
	}

	float CurrentValue = GetStatValue(StatTag);
	SetStatValue(StatTag, CurrentValue + AddValue);
}

void UStatsComponent::RemoveStatValue(const FGameplayTag& StatTag, float RemoveValue)
{
	AddStatValue(StatTag, -RemoveValue);
}

bool UStatsComponent::HasStat(const FGameplayTag& StatTag) const
{
	return Attributes.Contains(StatTag);
}

TArray<FGameplayTag> UStatsComponent::GetAllStatTags() const
{
	TArray<FGameplayTag> StatTags;
	Attributes.GetKeys(StatTags);
	return StatTags;
}

#pragma endregion

#pragma region Stat_Modifiers

void UStatsComponent::AddStatModifier(const FGameplayTag& StatTag, const FStatModifier& Modifier)
{
	if (!StatTag.IsValid() || !Modifier.ModifierTag.IsValid())
	{
		return;
	}

	FStatModifier NewModifier = Modifier;
	NewModifier.StartTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;

	TArray<FStatModifier>& Modifiers = StatModifiers.FindOrAdd(StatTag);
	
	// 같은 태그의 수정자가 있으면 교체
	int32 ExistingIndex = Modifiers.IndexOfByPredicate([&](const FStatModifier& Mod) {
		return Mod.ModifierTag == Modifier.ModifierTag;
	});

	if (ExistingIndex != INDEX_NONE)
	{
		Modifiers[ExistingIndex] = NewModifier;
	}
	else
	{
		Modifiers.Add(NewModifier);
	}

	// 스탯 재계산 예약
	bNeedsStatRecalculation = true;
	SetComponentTickEnabled(true);
}

void UStatsComponent::RemoveStatModifier(const FGameplayTag& StatTag, const FGameplayTag& ModifierTag)
{
	TArray<FStatModifier>* Modifiers = StatModifiers.Find(StatTag);
	if (!Modifiers)
	{
		return;
	}

	int32 RemovedCount = Modifiers->RemoveAll([&](const FStatModifier& Mod) {
		return Mod.ModifierTag == ModifierTag;
	});

	if (RemovedCount > 0)
	{
		bNeedsStatRecalculation = true;
		SetComponentTickEnabled(true);
	}
}

void UStatsComponent::RemoveAllStatModifiers(const FGameplayTag& StatTag)
{
	if (StatModifiers.Remove(StatTag) > 0)
	{
		bNeedsStatRecalculation = true;
		SetComponentTickEnabled(true);
	}
}

TArray<FStatModifier> UStatsComponent::GetStatModifiers(const FGameplayTag& StatTag) const
{
	const TArray<FStatModifier>* Modifiers = StatModifiers.Find(StatTag);
	return Modifiers ? *Modifiers : TArray<FStatModifier>();
}

void UStatsComponent::AddTemporaryStatBonus(const FGameplayTag& StatTag, float BonusValue, float Duration, bool bIsMultiplier)
{
	FStatModifier Modifier;
	Modifier.ModifierTag = FGameplayTag::RequestGameplayTag(
		FName(*FString::Printf(TEXT("Modifier.Temp.%s"), *StatTag.ToString()))
	);
	Modifier.ModifierValue = BonusValue;
	Modifier.Duration = Duration;
	Modifier.bIsMultiplier = bIsMultiplier;

	AddStatModifier(StatTag, Modifier);
}

void UStatsComponent::AddPermanentStatBonus(const FGameplayTag& StatTag, float BonusValue, bool bIsMultiplier)
{
	AddTemporaryStatBonus(StatTag, BonusValue, -1.0f, bIsMultiplier);
}

#pragma endregion

#pragma region Item_Integration

void UStatsComponent::AddItemStats(const FItemData& ItemData)
{
	// ItemData에서 스탯 정보를 추출하여 적용
	// 이 부분은 FItemData 구조에 따라 구현이 달라질 수 있습니다
	
	// 예시: ItemData에 StatBonuses가 있다고 가정
	/*
	for (const auto& StatBonus : ItemData.StatBonuses)
	{
		FStatModifier Modifier;
		Modifier.ModifierTag = FGameplayTag::RequestGameplayTag(
			FName(*FString::Printf(TEXT("Item.%s"), *ItemData.ItemName.ToString()))
		);
		Modifier.ModifierValue = StatBonus.Value;
		Modifier.bIsMultiplier = StatBonus.bIsMultiplier;
		Modifier.Duration = -1.0f; // 영구

		AddStatModifier(StatBonus.StatTag, Modifier);
	}
	*/
}

void UStatsComponent::RemoveItemStats(const FItemData& ItemData)
{
	// 아이템 관련 수정자 제거
	FGameplayTag ItemModifierTag = FGameplayTag::RequestGameplayTag(
		FName(*FString::Printf(TEXT("Item.%s"), *ItemData.ID))
	);

	for (auto& StatModifierPair : StatModifiers)
	{
		StatModifierPair.Value.RemoveAll([&](const FStatModifier& Modifier) {
			return Modifier.ModifierTag == ItemModifierTag;
		});
	}

	bNeedsStatRecalculation = true;
	SetComponentTickEnabled(true);
}

#pragma endregion

#pragma region Leveling_System

float UStatsComponent::GetExperienceRequiredForLevel(int32 Level) const
{
	if (Level <= 1)
	{
		return 0.0f;
	}

	return BaseExperienceRequired * FMath::Pow(ExperienceMultiplier, Level - 2);
}

float UStatsComponent::GetExperienceRequiredForNextLevel() const
{
	return GetExperienceRequiredForLevel(CurrentLevel + 1);
}

float UStatsComponent::GetExperiencePercentageToNextLevel() const
{
	if (CurrentLevel >= MaxLevel)
	{
		return 1.0f;
	}

	float RequiredExp = GetExperienceRequiredForNextLevel();
	if (RequiredExp <= 0.0f)
	{
		return 1.0f;
	}

	return FMath::Clamp(CurrentExperience / RequiredExp, 0.0f, 1.0f);
}

void UStatsComponent::AddExperience(float ExperienceAmount)
{
	if (ExperienceAmount <= 0.0f || CurrentLevel >= MaxLevel)
	{
		return;
	}

	if (GetOwnerRole() == ROLE_Authority)
	{
		CurrentExperience += ExperienceAmount;
		OnGainedExperience.Broadcast(CurrentExperience);
		CheckForLevelUp();
	}
	else
	{
		ServerAddExperience(ExperienceAmount);
	}
}

void UStatsComponent::SetLevel(int32 NewLevel)
{
	NewLevel = FMath::Clamp(NewLevel, 1, MaxLevel);
	
	if (NewLevel != CurrentLevel)
	{
		int32 OldLevel = CurrentLevel;
		CurrentLevel = NewLevel;
		
		// 레벨에 맞는 경험치로 조정
		CurrentExperience = 0.0f;
		
		OnLevelUpInternal(NewLevel);
		OnLevelChanged.Broadcast();
	}
}

bool UStatsComponent::CanLevelUp() const
{
	return CurrentLevel < MaxLevel && 
		   CurrentExperience >= GetExperienceRequiredForNextLevel();
}

#pragma endregion

#pragma region Utility_Functions

void UStatsComponent::InitializeStats()
{
	// 초기 스탯 설정
	for (const FInitialStatData& StatData : InitialStats)
	{
		if (StatData.StatTag.IsValid())
		{
			Attributes.Add(StatData.StatTag, StatData.BaseValue);
			MaxValueAttributes.Add(StatData.StatTag, StatData.MaxValue);
		}
	}

	// 클라이언트에 초기화 알림
	if (GetOwnerRole() == ROLE_Authority)
	{
		ClientInitializeStats();
	}

	RefreshStats.Broadcast();
}

void UStatsComponent::FixAnyMiscalculations()
{
	// 스탯 값이 최대값을 초과하지 않도록 보정
	for (auto& StatPair : Attributes)
	{
		const FGameplayTag& StatTag = StatPair.Key;
		float& CurrentValue = StatPair.Value;
		float MaxValue = GetMaxStatValue(StatTag);

		if (MaxValue > 0.0f && CurrentValue > MaxValue)
		{
			CurrentValue = MaxValue;
		}
		else if (CurrentValue < StatConstants::MIN_STAT_VALUE)
		{
			CurrentValue = StatConstants::MIN_STAT_VALUE;
		}
	}

	RecalculateAllStats();
}

void UStatsComponent::ResetAllStats()
{
	// 모든 스탯을 초기값으로 리셋
	for (const FInitialStatData& StatData : InitialStats)
	{
		if (StatData.StatTag.IsValid())
		{
			SetStatValue(StatData.StatTag, StatData.BaseValue);
		}
	}

	// 모든 수정자 제거
	StatModifiers.Empty();
	
	RefreshStats.Broadcast();
}

void UStatsComponent::RestoreStatToMax(const FGameplayTag& StatTag)
{
	float MaxValue = GetMaxStatValue(StatTag);
	if (MaxValue > 0.0f)
	{
		SetStatValue(StatTag, MaxValue);
	}
}

void UStatsComponent::RestoreAllStatsToMax()
{
	for (const auto& MaxStatPair : MaxValueAttributes)
	{
		RestoreStatToMax(MaxStatPair.Key);
	}
}

bool UStatsComponent::IsStatAtMax(const FGameplayTag& StatTag) const
{
	float CurrentValue = GetStatValue(StatTag);
	float MaxValue = GetMaxStatValue(StatTag);
	return MaxValue > 0.0f && FMath::IsNearlyEqual(CurrentValue, MaxValue, 0.1f);
}

bool UStatsComponent::IsStatEmpty(const FGameplayTag& StatTag) const
{
	return GetStatValue(StatTag) <= StatConstants::MIN_STAT_VALUE;
}

#pragma endregion

#pragma region Health_Helpers

float UStatsComponent::GetHealth() const
{
	return GetStatValue(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Health")));
}

float UStatsComponent::GetMaxHealth() const
{
	return GetMaxStatValue(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Health")));
}

float UStatsComponent::GetHealthPercentage() const
{
	return GetStatPercentage(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Health")));
}

void UStatsComponent::SetHealth(float NewHealth)
{
	SetStatValue(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Health")), NewHealth);
}

void UStatsComponent::AddHealth(float HealthAmount)
{
	AddStatValue(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Health")), HealthAmount);
}

void UStatsComponent::RemoveHealth(float HealthAmount)
{
	RemoveStatValue(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Health")), HealthAmount);
}

bool UStatsComponent::IsAlive() const
{
	return GetHealth() > 0.0f;
}

bool UStatsComponent::IsAtFullHealth() const
{
	return IsStatAtMax(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Health")));
}

#pragma endregion

#pragma region Mana_Helpers

float UStatsComponent::GetMana() const
{
	return GetStatValue(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Mana")));
}

float UStatsComponent::GetMaxMana() const
{
	return GetMaxStatValue(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Mana")));
}

float UStatsComponent::GetManaPercentage() const
{
	return GetStatPercentage(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Mana")));
}

void UStatsComponent::SetMana(float NewMana)
{
	SetStatValue(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Mana")), NewMana);
}

void UStatsComponent::AddMana(float ManaAmount)
{
	AddStatValue(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Mana")), ManaAmount);
}

void UStatsComponent::RemoveMana(float ManaAmount)
{
	RemoveStatValue(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Mana")), ManaAmount);
}

bool UStatsComponent::HasEnoughMana(float RequiredMana) const
{
	return GetMana() >= RequiredMana;
}

#pragma endregion

#pragma region Stamina_Helpers

float UStatsComponent::GetStamina() const
{
	return GetStatValue(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Stamina")));
}

float UStatsComponent::GetMaxStamina() const
{
	return GetMaxStatValue(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Stamina")));
}

float UStatsComponent::GetStaminaPercentage() const
{
	return GetStatPercentage(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Stamina")));
}

void UStatsComponent::SetStamina(float NewStamina)
{
	SetStatValue(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Stamina")), NewStamina);
}

void UStatsComponent::AddStamina(float StaminaAmount)
{
	AddStatValue(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Stamina")), StaminaAmount);
}

void UStatsComponent::RemoveStamina(float StaminaAmount)
{
	RemoveStatValue(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Stamina")), StaminaAmount);
}

bool UStatsComponent::HasEnoughStamina(float RequiredStamina) const
{
	return GetStamina() >= RequiredStamina;
}

#pragma endregion

#pragma region Network_Functions

void UStatsComponent::ServerAddStatValue_Implementation(const FGameplayTag& StatTag, float Value)
{
	AddStatValue(StatTag, Value);
}

void UStatsComponent::ServerRemoveStatValue_Implementation(const FGameplayTag& StatTag, float Value)
{
	RemoveStatValue(StatTag, Value);
}

void UStatsComponent::ServerSetStatValue_Implementation(const FGameplayTag& StatTag, float Value)
{
	SetStatValue(StatTag, Value);
}

void UStatsComponent::ClientStatValueChanged_Implementation(const FGameplayTag& StatTag, float StatValue)
{
	float OldValue = GetStatValue(StatTag);
	Attributes.Add(StatTag, StatValue);
	OnStatValueChanged(StatTag, OldValue, StatValue);
}

void UStatsComponent::ClientInitializeStats_Implementation()
{
	RefreshStats.Broadcast();
}

void UStatsComponent::ServerAddExperience_Implementation(float ExperienceAmount)
{
	AddExperience(ExperienceAmount);
}

#pragma endregion

#pragma region Internal_Functions

float UStatsComponent::ClampStatValue(const FGameplayTag& StatTag, float Value) const
{
	float MaxValue = GetMaxStatValue(StatTag);
	if (MaxValue > 0.0f)
	{
		return FMath::Clamp(Value, StatConstants::MIN_STAT_VALUE, MaxValue);
	}
	return FMath::Max(Value, StatConstants::MIN_STAT_VALUE);
}

void UStatsComponent::OnStatValueChanged(const FGameplayTag& StatTag, float OldValue, float NewValue)
{
	if (!FMath::IsNearlyEqual(OldValue, NewValue, 0.01f))
	{
		OnStatChanged.Broadcast(StatTag, NewValue);
		OnStatModified.Broadcast(StatTag, OldValue, NewValue);
	}
}

void UStatsComponent::CheckForLevelUp()
{
	while (CanLevelUp())
	{
		CurrentExperience -= GetExperienceRequiredForNextLevel();
		CurrentLevel++;
		OnLevelUpInternal(CurrentLevel);
	}
}

void UStatsComponent::OnLevelUpInternal(int32 NewLevel)
{
	// 레벨업 시 스탯 증가 (구현에 따라 달라질 수 있음)
	float HealthIncrease = 10.0f; // 레벨당 체력 증가량
	float ManaIncrease = 5.0f;    // 레벨당 마나 증가량
	float StaminaIncrease = 5.0f; // 레벨당 스테미나 증가량

	AddStatValue(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Health")), HealthIncrease);
	AddStatValue(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Mana")), ManaIncrease);
	AddStatValue(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Stamina")), StaminaIncrease);

	// 최대값도 증가
	SetMaxStatValue(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Health")), 
		GetMaxHealth() + HealthIncrease);
	SetMaxStatValue(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Mana")), 
		GetMaxMana() + ManaIncrease);
	SetMaxStatValue(FGameplayTag::RequestGameplayTag(FName("Character.Stat.Stamina")), 
		GetMaxStamina() + StaminaIncrease);

	OnLevelUp.Broadcast(NewLevel);
	OnLevelChanged.Broadcast();
}

void UStatsComponent::RecalculateAllStats()
{
	for (auto& StatPair : Attributes)
	{
		RecalculateStat(StatPair.Key);
	}
}

void UStatsComponent::RecalculateStat(const FGameplayTag& StatTag)
{
	// 기본값 가져오기
	float BaseValue = 0.0f;
	for (const FInitialStatData& StatData : InitialStats)
	{
		if (StatData.StatTag == StatTag)
		{
			BaseValue = StatData.BaseValue;
			break;
		}
	}

	// 수정자 적용
	float FinalValue = CalculateStatWithModifiers(StatTag, BaseValue);
	
	// 값 설정 (클램핑 포함)
	float OldValue = GetStatValue(StatTag);
	Attributes.Add(StatTag, ClampStatValue(StatTag, FinalValue));
	
	OnStatValueChanged(StatTag, OldValue, GetStatValue(StatTag));
}

float UStatsComponent::CalculateStatWithModifiers(const FGameplayTag& StatTag, float BaseValue) const
{
	const TArray<FStatModifier>* Modifiers = StatModifiers.Find(StatTag);
	if (!Modifiers)
	{
		return BaseValue;
	}

	float AdditiveBonus = 0.0f;
	float MultiplicativeBonus = 1.0f;

	for (const FStatModifier& Modifier : *Modifiers)
	{
		// 만료된 수정자는 무시
		if (Modifier.IsExpired(GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f))
		{
			continue;
		}

		if (Modifier.bIsMultiplier)
		{
			MultiplicativeBonus *= (1.0f + Modifier.ModifierValue);
		}
		else
		{
			AdditiveBonus += Modifier.ModifierValue;
		}
	}

	return (BaseValue + AdditiveBonus) * MultiplicativeBonus;
}

void UStatsComponent::CleanupExpiredModifiers()
{
	if (!GetWorld())
	{
		return;
	}

	float CurrentTime = GetWorld()->GetTimeSeconds();
	bool bNeedsCleanup = false;

	for (auto& StatModifierPair : StatModifiers)
	{
		TArray<FStatModifier>& Modifiers = StatModifierPair.Value;
		int32 OriginalCount = Modifiers.Num();
		
		Modifiers.RemoveAll([CurrentTime](const FStatModifier& Modifier) {
			return Modifier.IsExpired(CurrentTime);
		});

		if (Modifiers.Num() != OriginalCount)
		{
			bNeedsCleanup = true;
		}
	}

	if (bNeedsCleanup)
	{
		bNeedsStatRecalculation = true;
	}

	// 더 이상 수정자가 없고 재계산이 필요하지 않으면 틱 비활성화
	if (StatModifiers.Num() == 0 && !bNeedsStatRecalculation)
	{
		SetComponentTickEnabled(false);
	}
}

void UStatsComponent::SetupDefaultStats()
{
	// 기본 스탯 설정
	InitialStats.Empty();
	
	// InitialStats.Add(FInitialStatData(
	// 	FGameplayTag::RequestGameplayTag(FName("Character.Stat.Health")),
	// 	StatConstants::DEFAULT_MAX_HEALTH,
	// 	StatConstants::DEFAULT_MAX_HEALTH
	// ));
	//
	// InitialStats.Add(FInitialStatData(
	// 	FGameplayTag::RequestGameplayTag(FName("Character.Stat.Mana")),
	// 	StatConstants::DEFAULT_MAX_MANA,
	// 	StatConstants::DEFAULT_MAX_MANA
	// ));
	//
	// InitialStats.Add(FInitialStatData(
	// 	FGameplayTag::RequestGameplayTag(FName("Character.Stat.Stamina")),
	// 	StatConstants::DEFAULT_MAX_STAMINA,
	// 	StatConstants::DEFAULT_MAX_STAMINA
	// ));
}


// 네트워크 콜백 함수들
void UStatsComponent::OnRep_Attributes()
{
	RefreshStats.Broadcast();
}

void UStatsComponent::OnRep_MaxValueAttributes()
{
	RefreshStats.Broadcast();
}

void UStatsComponent::OnRep_CurrentLevel()
{
	OnLevelChanged.Broadcast();
}

void UStatsComponent::OnRep_CurrentExperience()
{
	OnCurrentExpChanged.Broadcast();
}

#pragma endregion