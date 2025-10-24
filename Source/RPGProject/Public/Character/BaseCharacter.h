#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UActionStateCoordinator;

namespace CharacterConstants
{
	static constexpr float DEFAULT_DEATH_TIME = 3.0f;
	static constexpr float DEFAULT_HITSTOP_DURATION = 0.1f;
}

class UStatsComponent;
class UBuffComponent;
class UActionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterDeath, ABaseCharacter*, DeadCharacter);

UCLASS()
class RPGPROJECT_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ABaseCharacter(const FObjectInitializer& ObjectInitializer);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStatsComponent> StatsComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBuffComponent> BuffComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UActionComponent> ActionComponent;

	UPROPERTY(BlueprintAssignable)
	FOnCharacterDeath OnCharacterDeath;
	
protected:
	virtual void BeginPlay() override;
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void BeginDestroy() override;

	UPROPERTY(EditAnywhere, Category = "Audio|FootStep")
	TObjectPtr<USoundBase> FootstepSound;

	UPROPERTY(EditAnywhere, Category = "VFX|FootStep")
	TObjectPtr<UParticleSystem> FootstepEffect;
	
	UPROPERTY(EditAnywhere, Category = "Audio|Hit")
	TObjectPtr<USoundBase> HitSound;
	
	UPROPERTY(EditAnywhere, Category = "VFX|Hit")
	TObjectPtr<UParticleSystem> HitParticles;

#pragma region Death
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UAnimMontage> DeathMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	bool bIsDead = false;

	FTimerHandle DeathTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Death", meta = (AllowPrivateAccess = "true"))
	float DeathTime = CharacterConstants::DEFAULT_DEATH_TIME;

	UFUNCTION(BlueprintCallable)
	virtual void FinishDeath();

	UFUNCTION()
	virtual void DestroyCharacter();

	virtual void Die();

#pragma endregion 

#pragma region Movement
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetMaxWalkSpeed(float Speed) const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetJumpVelocity(float Velocity) const;
	
	UFUNCTION(BlueprintCallable, Category = "Movement")
	void SetGravityScale(float Scale) const;

#pragma endregion
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GameplayTags", meta=(AllowPrivateAccess="true"))
	FGameplayTagContainer CharacterTags;

	FTimerHandle HitStopTimer;
	
public:
#pragma region Montage
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayMontage(UAnimMontage* Montage) const;
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayMontageSection(UAnimMontage* Montage, FName SectionName = "") const;
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayMontageNextSection(UAnimMontage* Montage, FName SectionName = "") const;
	
	//UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayMontageWithEndDelegate(UAnimMontage* Montage, FOnMontageEnded& Delegate) const;
	
	//UFUNCTION(BlueprintCallable, Category = "Animation")
	void PlayMontageWithBlendingOutDelegate(UAnimMontage* Montage, FOnMontageBlendingOutStarted& Delegate) const;
	
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void SetCurrentMontageRate(float NewRate = 1.f) const;

#pragma endregion
	
#pragma region Tag
	
	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	void AddCharacterTag(const FGameplayTag& TagToAdd);

	UFUNCTION(BlueprintCallable, Category = "GameplayTags")
	void RemoveCharacterTag(const FGameplayTag& TagToRemove);

	UFUNCTION(BlueprintPure, Category = "GameplayTags")
	FGameplayTag FindCharacterTagByName(FName TagNameToFind) const;

	UFUNCTION(BlueprintPure, Category = "GameplayTags")
	bool HasCharacterTag(const FGameplayTag& TagToCheck) const;

	UFUNCTION(BlueprintPure, Category = "GameplayTags")
	bool HasAnyCharacterTags(const FGameplayTagContainer& TagsToCheck) const;

	UFUNCTION(BlueprintPure, Category = "GameplayTags")
	bool HasAllCharacterTags(const FGameplayTagContainer& TagsToCheck) const;

	UFUNCTION(BlueprintPure, Category = "GameplayTags")
	FGameplayTagContainer GetCharacterTags() const { return CharacterTags; }

#pragma endregion

#pragma region Combat
	
	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ApplyHitStop(float Duration);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EndHitStop();

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsDead() const { return bIsDead; }

#pragma endregion

#pragma region Audio_VFX
	
	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayFootstepSound();

	UFUNCTION(BlueprintCallable, Category = "VFX")
	void SpawnFootstepEffect();

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayHitSound();

	UFUNCTION(BlueprintCallable, Category = "VFX")
	void SpawnHitEffect(const FVector& Location, const FVector& Normal = FVector::UpVector);

#pragma endregion
};
