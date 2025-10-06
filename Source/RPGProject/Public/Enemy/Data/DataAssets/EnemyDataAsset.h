#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "EnemyDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FEnemyStats
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float HP;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float Damage;
};

USTRUCT(BlueprintType)
struct FEnemyChaseParam
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float ChaseMin;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float ChaseMax;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float ChasingSpeed;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float ChaseRange;
};

USTRUCT(BlueprintType)
struct FEnemyPatrolParam
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	double PatrolRadius;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PatrolWaitMin = 3.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PatrolWaitMax = 5.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float PatrollingSpeed = 125.f;
};

USTRUCT(BlueprintType)
struct FEnemyAttackParam
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	double CombatRadius;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	double AttackRadius;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	double AcceptanceRadius;
		
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float AttackMin = 0.5f;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float AttackMax = 1.f;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float Damage;
};

USTRUCT(BlueprintType)
struct FEnemyDeathParam
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	float DeathLifeSpan = 8.f;
};

UCLASS()
class RPGPROJECT_API UEnemyDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FEnemyStats Stats;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FEnemyPatrolParam PatrolParam;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FEnemyChaseParam ChaseParam;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FEnemyAttackParam AttackParam;

	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	FEnemyDeathParam DeathParam;
};
