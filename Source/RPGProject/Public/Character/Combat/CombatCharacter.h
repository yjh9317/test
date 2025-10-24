// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "HitBox/HitBoxComponent.h"
#include "CombatCharacter.generated.h"

class UHitBoxComponent;
class UTargetSystemComponent;


// 전투 관련 델리게이트들
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterParried, AActor*, Attacker);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnCombatStateChanged, FGameplayTag, OldState, FGameplayTag, NewState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponEquipped, FGameplayTag, WeaponType);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponUnequipped);


UCLASS()
class RPGPROJECT_API ACombatCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	ACombatCharacter(const FObjectInitializer& ObjectInitializer);

	// === 전투 컴포넌트 ===
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UHitBoxComponent> HitBoxComponent;

	// === 전투 델리게이트 ===
	UPROPERTY(BlueprintAssignable)
	FOnCharacterParried OnCharacterParried;

	UPROPERTY(BlueprintAssignable)
	FOnCombatStateChanged OnCombatStateChanged;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponEquipped OnWeaponEquipped;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponUnequipped OnWeaponUnequipped;

protected:
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	// === 전투 설정 ===
	UPROPERTY(EditDefaultsOnly, Category = "Combat|Settings")
	float BaseAttackRange = 200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Settings")
	float BaseAttackDamage = 25.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Settings")
	float BlockAngle = 90.0f; // 방어 가능한 각도

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Settings")
	float ParryWindow = 0.3f; // 패리 가능한 시간 윈도우

public:
	#pragma region Combat_Interface

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual bool CanAttack() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual bool CanBlock() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual bool CanDodge() const;

	UFUNCTION(BlueprintCallable, Category = "Combat")
	virtual bool CanParry() const;

	UFUNCTION(BlueprintPure, Category = "Combat")
	virtual bool IsAttacking() const;

	UFUNCTION(BlueprintPure, Category = "Combat")
	virtual bool IsBlocking() const;


#pragma endregion

#pragma region Combat_Actions

	UFUNCTION(BlueprintCallable, Category = "Combat|Actions")
	virtual void StartAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat|Actions")
	virtual void EndAttack();

	UFUNCTION(BlueprintCallable, Category = "Combat|Actions")
	virtual void StartBlock();

	UFUNCTION(BlueprintCallable, Category = "Combat|Actions")
	virtual void EndBlock();

	UFUNCTION(BlueprintCallable, Category = "Combat|Actions")
	virtual bool TryParry();

	UFUNCTION(BlueprintCallable, Category = "Combat|Actions")
	virtual void Dodge(const FVector& Direction);

#pragma endregion

#pragma region Damage_System

	UFUNCTION(BlueprintCallable, Category = "Combat|Damage")
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
		class AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat|Damage")
	void OnDamageReceived(float DamageAmount, AActor* DamageCauser);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat|Damage")
	void OnDamageBlocked(float DamageAmount, AActor* Attacker);

	UFUNCTION(BlueprintImplementableEvent, Category = "Combat|Damage")
	void OnSuccessfulParry(AActor* Attacker);

#pragma endregion


// #pragma region Weapon_System
//
// 	UFUNCTION(BlueprintPure, Category = "Combat|Weapon")
// 	bool IsWeaponEquipped() const;
//
// 	UFUNCTION(BlueprintPure, Category = "Combat|Weapon")
// 	FGameplayTag GetEquippedWeaponType() const;
//
// 	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
// 	virtual void EquipWeapon(FGameplayTag WeaponType);
//
// 	UFUNCTION(BlueprintCallable, Category = "Combat|Weapon")
// 	virtual void UnequipWeapon();
//
// #pragma endregion

#pragma region Combat_Stats

	UFUNCTION(BlueprintPure, Category = "Combat|Stats")
	float GetAttackRange() const { return BaseAttackRange; }

	UFUNCTION(BlueprintPure, Category = "Combat|Stats")
	float GetAttackDamage() const { return BaseAttackDamage; }

	UFUNCTION(BlueprintPure, Category = "Combat|Stats")
	float GetBlockAngle() const { return BlockAngle; }

	UFUNCTION(BlueprintCallable, Category = "Combat|Stats")
	void ModifyAttackDamage(float Multiplier);

	UFUNCTION(BlueprintCallable, Category = "Combat|Stats")
	void ModifyAttackRange(float Multiplier);

#pragma endregion

private:
	// 내부 상태 추적
	bool bIsAttacking = false;
	bool bIsBlocking = false;
	bool bCanParry = false;

	// 타이머 핸들
	FTimerHandle ParryWindowTimer;
	FTimerHandle CombatStateTimer;

	// 내부 함수들
	void OnParryWindowStart();
	void OnParryWindowEnd();
	bool IsAngleWithinBlockRange(const FVector& AttackDirection) const;
	void OnHitBoxHit(const FHitBoxDamageEvent& HitEvent);
};
