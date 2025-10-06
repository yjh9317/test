// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Header/UEnemyTypes.h"
#include "EnemyController.generated.h"

class UAISense;
UCLASS()
class RPGPROJECT_API AEnemyController : public AAIController
{
	GENERATED_BODY()

public:
	explicit AEnemyController(FObjectInitializer const& ObjectInitializer);

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override; 
	virtual void OnPossess(APawn* InPawn) override;

private:
	// ─────────────────────────────────────────────────────────────────────
	// AISense
	// ─────────────────────────────────────────────────────────────────────
	UPROPERTY()
	AActor* TargetActor;
	
	UPROPERTY()
	class UAISenseConfig_Sight* SightConfig;

	UPROPERTY()
	class UAISenseConfig_Hearing* HearingConfig;

	UPROPERTY()
	class UAISenseConfig_Damage* DamageConfig;

	UPROPERTY()
	class UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(BlueprintReadWrite, Category= "AI Behavior",meta=(AllowPrivateAccess="true"))
	class UBlackboardComponent* BlackboardComponent;

	UPROPERTY(BlueprintReadWrite, Category= "AI Behavior",meta=(AllowPrivateAccess="true"))
	class UBehaviorTreeComponent* BehaviorTreeComponent;

	void SetPerceptionSystem();


	UFUNCTION()
	void StartSightRangeTimer(float NewSightRange);
	
	UFUNCTION()
	bool SetSightRange(AAIController* Controller, float NewSightRange);

	UFUNCTION()
	void OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);
public:
	FORCEINLINE UBlackboardComponent* GetBlackboardComponent() const { return BlackboardComponent; }

	
};
