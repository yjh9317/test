
#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Player/Character/MainCharacter.h"
#include "Enemy/Data/DataAssets/EnemyDataAsset.h"
#include "Header/UEnemyTypes.h"
#include "Enemy/Controller/EnemyController.h"
#include "EnemyCharacter.generated.h"

class USphereComponent;

UCLASS()
class RPGPROJECT_API AEnemyCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemyCharacter(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	UFUNCTION(BlueprintCallable)
	void ShowHitNumber(int32 Damage,const FVector& HitLocation);


protected:
	// Controller
	UPROPERTY()
	AEnemyController* EnemyController;


private:

	UPROPERTY(EditDefaultsOnly, Category= "Attributes" ,meta = (ALlowPrivateAccess = "true"))
	UEnemyDataAsset* EnemyStats;

	UPROPERTY(VisibleAnywhere, Category= "Attributes" ,meta = (ALlowPrivateAccess = "true"))
	class UHealthBarComponent* HealthBarComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= "Combat", meta =(AllowPrivateAccess = "true"))
	float HealthBarDisplayTime;
	
	FTimerHandle HealthBarTimer;
	
	// ─────────────────────────────────────────────────────────────────────
	// Montage
	// ─────────────────────────────────────────────────────────────────────
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Combat", meta =(AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> HitNumberWidgetClass;

	UPROPERTY(VisibleAnywhere, Category="Combat", meta =(AllowPrivateAccess = "true"))
	TMap<UUserWidget*, FVector> HitNumbers;

	UPROPERTY(EditAnywhere, Category="Combat", meta =(AllowPrivateAccess = "true"))
	float HitNumberDestroyTime;
	
	// Behavior Tree
	UPROPERTY(EditAnywhere, Category="AI", meta =(AllowPrivateAccess = "true"))
	UBehaviorTree* BehaviorTree;

	UPROPERTY()
	class UAIPerceptionStimuliSourceComponent* StimulusScore;

	void SetupStimulusScore();


protected:
	virtual void EnemyInitialize();
	UFUNCTION()
	void ShowHealthBar();

	UFUNCTION()
	void HideHealthBar();

	virtual void Die() override;
	
	virtual void Hit(FHitResult& HitResult);

	UFUNCTION(BlueprintCallable)
	void StoreHitNumber(UUserWidget* HitNumber,const FVector& Location);

	UFUNCTION()
	void DestroyHitNumber(UUserWidget* HitNumber);

	void UpdateHitNumbers();

	void DoDamage(class AMainCharacter* Victim);
	void SpawnBlood(AMainCharacter* Victim, const FName& SocketName);

 	void ResetCanAttack();
public:
	FORCEINLINE FEnemyChaseParam GetChaseParam() const {return EnemyStats->ChaseParam;}
	FORCEINLINE FEnemyAttackParam GetAttackParam() const {return EnemyStats->AttackParam;}
	FORCEINLINE FEnemyPatrolParam GetPatrolParam() const {return EnemyStats->PatrolParam;}
	FORCEINLINE FEnemyDeathParam GetDeathParam() const {return EnemyStats->DeathParam;}
	
	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	void StunCharacter(AMainCharacter* Victim);

	UFUNCTION(BlueprintCallable)
	void SetMovementSpeed(EEnemyMovementSpeed MovementSpeed); 
};
