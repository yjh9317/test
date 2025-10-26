// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GameplayTagContainer.h"
#include "Header/UEnemyTypes.h"
#include "Perception/AIPerceptionTypes.h"
#include "EnemyController.generated.h"

class UAIPerceptionHandler;
class UBehaviorTreeComponent;
class UAISenseConfig_Damage;
class UAISenseConfig_Hearing;
class UAISenseConfig_Sight;

UCLASS()
class AEnemyController : public AAIController
{
	GENERATED_BODY()

public:
	AEnemyController(FObjectInitializer const& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	// Perception Settings
	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception|Sight")
	float SightRadius = 5000.f;
    
	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception|Sight")
	float LoseSightRadius = 5250.f;
    
	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception|Sight")
	float PeripheralVisionAngle = 90.f;
    
	UPROPERTY(EditDefaultsOnly, Category = "AI|Perception|Hearing")
	float HearingRange = 1200.f;
    
	UPROPERTY(EditDefaultsOnly, Category = "AI|Tags")
	FGameplayTag PlayerTag;
    
	UPROPERTY(EditDefaultsOnly, Category = "AI|Tags")
	FGameplayTag EnemyTag;

private:
	// Components
	UPROPERTY()
	UBlackboardComponent* BlackboardComponent;
    
	UPROPERTY()
	UBehaviorTreeComponent* BehaviorTreeComponent;
    
	UPROPERTY()
	UAIPerceptionComponent* AIPerceptionComponent;
    
	UPROPERTY()
	UAISenseConfig_Sight* SightConfig;
    
	UPROPERTY()
	UAISenseConfig_Hearing* HearingConfig;
    
	UPROPERTY()
	UAISenseConfig_Damage* DamageConfig;

	UPROPERTY()
	UAIPerceptionHandler* PerceptionHandler;

	// Timer
	FTimerHandle SightRangeTimerHandle;

	// Setup
	void SetPerceptionSystem();
    
	// Perception Callbacks
	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
  
	// Helpers
	bool IsPlayerActor(AActor* Actor) const;
	void UpdateTargetInBlackboard(AActor* NewTarget);
	void StartSightRangeTimer(float NewSightRange);
	bool SetSightRange(AAIController* Controller, float NewSightRange);
};
