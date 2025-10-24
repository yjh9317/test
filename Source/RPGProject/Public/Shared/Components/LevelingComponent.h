
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LevelingComponent.generated.h"


/*
*
*
	UPROPERTY(EditDefaultsOnly, Category = "Leveling", meta = (ClampMin = "1"))
	int32 MaxLevel = 100;

	UPROPERTY(EditDefaultsOnly, Category = "Leveling")
	float BaseExperienceRequired = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Leveling", meta = (ClampMin = "1.0"))
	float ExperienceMultiplier = 1.2f;
*
// 레벨 관련 델리게이트
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGainedExperience, float, NewEXP);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLevelUp, int32, NewLevel);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLevelChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCurrentExpChanged);
 *
* 	// 레벨 관련 델리게이트
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

void UStatsComponent::OnRep_CurrentLevel()
{
	OnLevelChanged.Broadcast();
}

void UStatsComponent::OnRep_CurrentExperience()
{
	OnCurrentExpChanged.Broadcast();
}


void CheckForLevelUp();
	void OnLevelUpInternal(int32 NewLevel);
	UFUNCTION()
	void OnRep_CurrentLevel();

	UFUNCTION()
	void OnRep_CurrentExperience();

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

void UStatsComponent::ServerAddExperience_Implementation(float ExperienceAmount)
{
	AddExperience(ExperienceAmount);
}

UFUNCTION(Server, Reliable, Category = "Stats|Network")
	void ServerAddExperience(float ExperienceAmount);


 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API ULevelingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULevelingComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	virtual void BeginDestroy() override;
	
	UPROPERTY(ReplicatedUsing = OnRep_CurrentLevel, VisibleAnywhere , BlueprintReadOnly ,Category="Default")
	int32 CurrentLevel = 1;
	UPROPERTY(ReplicatedUsing = OnRep_MaxLevel, VisibleAnywhere , BlueprintReadOnly ,Category="Default")
	int32 MaxLevel = 1;
	UPROPERTY(ReplicatedUsing = OnRep_CurrentEXP, VisibleAnywhere , BlueprintReadOnly , Category="Default")
	float CurrentEXP = 0.f;
	UPROPERTY(ReplicatedUsing = OnRep_LevelMaxEXP, VisibleAnywhere , BlueprintReadOnly , Category="Default")
	float LevelMaxEXP;

	UFUNCTION()
	void OnRep_CurrentLevel(int32 OldLevel);

	UFUNCTION()
	void OnRep_MaxLevel(int32 OldMaxLevel);

	UFUNCTION()
	void OnRep_CurrentEXP(float OldEXP);

	UFUNCTION()
	void OnRep_LevelMaxEXP(float OldMaxEXP);

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UDataTable> LevelingDataTable;
	TMap<int32, float> LevelingData;
	
public:
	FORCEINLINE int32 GetCurrentLevel() const { return CurrentLevel; }
	FORCEINLINE float GetCurrentExp() const { return CurrentEXP; }
	FORCEINLINE float GetCurrentLevelMaxEXP() const { return LevelMaxEXP;}
	FORCEINLINE bool ExceedLevelMaxEXP() const { return CurrentEXP >= LevelMaxEXP; }
	FORCEINLINE bool IsNextLevelValid() const { return LevelingData.Find(CurrentLevel + 1) ? true : false; }
	FORCEINLINE bool IsAddLevelValid(int32 AddLevel = 1) const { return LevelingData.Find(CurrentLevel + AddLevel) ? true : false; }
	
	void SetCurrentLevel(int32 NewLevel) { CurrentLevel = NewLevel; }
	void SetCurrentEXP(float NewEXP) { CurrentEXP = NewEXP; }
	void SetCurrentLevelMaxEXP(float NewMaxEXP) { LevelMaxEXP = NewMaxEXP;}
};
