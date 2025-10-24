#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MainGameMode.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API AMainGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMainGameMode();

	// UENUM(BlueprintType)
	// enum class EDifficultyLevel : uint8
	// {
	// 	Easy        UMETA(DisplayName = "Easy"),
	// 	Normal      UMETA(DisplayName = "Normal"),
	// 	Hard        UMETA(DisplayName = "Hard"),
	// 	Nightmare   UMETA(DisplayName = "Nightmare")
	// };
 //    
	// UPROPERTY(BlueprintReadWrite, Category = "Difficulty")
	// EDifficultyLevel CurrentDifficulty = EDifficultyLevel::Normal;
    
	// UFUNCTION(BlueprintPure, Category = "Difficulty")
	// float GetDamageMultiplier() const;
 //    
	// UFUNCTION(BlueprintPure, Category = "Difficulty")
	// float GetExpMultiplier() const;
 //    
	// // === 세이브/로드 ===
	// UFUNCTION(BlueprintCallable, Category = "Save")
	// void SaveGame(const FString& SlotName);
 //    
	// UFUNCTION(BlueprintCallable, Category = "Save")
	// void LoadGame(const FString& SlotName);
 //    
	// // === 체크포인트 (봉화) 시스템 ===
	// UPROPERTY(BlueprintReadOnly, Category = "Checkpoint")
	// AActor* LastActivatedCheckpoint;
 //    
	// UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	// void ActivateCheckpoint(AActor* Checkpoint);
 //    
	// UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	// void RespawnAtCheckpoint(ACharacter* Character);
 //    
	// // === 소울/경험치 드롭 ===
	// UFUNCTION(BlueprintCallable, Category = "Souls")
	// void DropSouls(FVector Location, int32 Amount);
 //    
	// UPROPERTY(BlueprintReadOnly, Category = "Souls")
	// TArray<AActor*> DroppedSouls;
 //    
	// // === 월드 이벤트 ===
	// UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	// void OnBossDefeated(AActor* Boss);
 //    
	// UFUNCTION(BlueprintImplementableEvent, Category = "Events")
	// void OnPlayerDeath(ACharacter* Player);
};
