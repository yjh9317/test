
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LevelingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API ULevelingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULevelingComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
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
