#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MainCharacter.h"
#include "Character/BaseAnimInstance.h"
#include "MainAnimInstance.generated.h"

class AMainCharacter;
class UCharacterMovementComponent;


UENUM(BlueprintType)
enum class ECharacterCombatType : uint8
{
	ECT_Unarmed         UMETA(DisplayName = "Unarmed"),          
	ECT_SwordOnly       UMETA(DisplayName = "Sword Only"),       
	ECT_SwordAndShield  UMETA(DisplayName = "Sword and Shield"), 
	ECT_TwoHandedSword  UMETA(DisplayName = "Two-Handed Sword"), 
	ECT_Bow  UMETA(DisplayName = "Bow"), 
	ECT_Cane  UMETA(DisplayName = "Cane"), 
	// ECT_ShieldOnly      UMETA(DisplayName = "Shield Only"),
	
	ECT_MAX             UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FLocomotionAnimSet
{
	GENERATED_BODY()
    
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UAnimSequence> Start;
    
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UAnimSequence> Loop;
    
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UAnimSequence> Stop;
    
	// Blend Space 사용 시
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UBlendSpace> LoopBS;
    
	bool IsValid() const
	{
		return !Loop.IsNull();
	}
};

USTRUCT(BlueprintType)
struct FWeaponAnimationRow : public FTableRowBase
{
	GENERATED_BODY()
    
	UPROPERTY(EditAnywhere)
	FName WeaponID;
    
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<UAnimSequence> Idle;
    
	UPROPERTY(EditAnywhere)
	FLocomotionAnimSet Walk;  // Start/Loop/Stop 포함
    
	UPROPERTY(EditAnywhere)
	FLocomotionAnimSet Jog;
    
	UPROPERTY(EditAnywhere)
	FLocomotionAnimSet Run;
    
	UPROPERTY(EditAnywhere)
	FLocomotionAnimSet CrouchWalk;
};

// UENUM(BlueprintType)
// enum class EMagicianState : uint8
// {
// 	Wizard UMETA(DisplayName = "Wizard"),
// 	BattleMage UMETA(DisplayName = "BattleMage"),
// };
//
// UENUM(BlueprintType)
// enum class ESwordShieldState : uint8
// {
// 	SwordShield UMETA(DisplayName = "SwordShield"),
// 	OneHanded UMETA(DisplayName = "OneHanded"),
// 	TwoHanded UMETA(DisplayName = "TwoHanded")
// };
//
// UENUM(BlueprintType)
// enum class EGreatSwordState : uint8
// {
// 	Normal UMETA(DisplayName = "Normal"),
// 	Battle UMETA(DisplayName = "Battle")
// };


UCLASS()
class RPGPROJECT_API UMainAnimInstance : public UBaseAnimInstance
{
	GENERATED_BODY()
	
public:
	friend class AMainCharacter;
	UMainAnimInstance(const FObjectInitializer& ObjectInitializer);
	
	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Input", meta=(AllowPrivateAccess="true"))
	ECharacterCombatType AnimCombatType;
	

protected:
	// About Player Variable
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AMainCharacter* Player;  

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCustomMovementComponent* PlayerMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	float SpeedDegree;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	float SpeedLength;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	float LastSpeedDegree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta=(AllowPrivateAccess="true"))
	bool bWeaponEquipped;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	bool bIsClimbing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	FVector ClimbVelocity;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	FRotator ViewRotation;

	float InputDirectionX;
	float InputDirectionY;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	bool bIsLeftClick;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	bool bIsRightClick;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	float InputDegreeDirection;

	// GameplayTag
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Input", meta=(AllowPrivateAccess="true"))
	FGameplayTag CurrentWeaponTag;


public:
	void GetInputInfo();
	// ─────────────────────────────────────────────────────────────────────
	//	Get Function
	// ─────────────────────────────────────────────────────────────────────
	UFUNCTION(BlueprintCallable)
	void GetSpeedVariables();
	UFUNCTION(BlueprintCallable)
	void GetIsClimbing();
	UFUNCTION(BlueprintCallable)
	void GetClimbVelocity();
	UFUNCTION(BlueprintCallable)
	void GetWeaponInfo();
	UFUNCTION(BlueprintCallable)
	void GetViewRotation(float DeltaSeconds);
	UFUNCTION(BlueprintCallable)
	void GetAimBlend(float DeltaSeconds);
	// ─────────────────────────────────────────────────────────────────────
	//	Set Function
	// ─────────────────────────────────────────────────────────────────────

	// ─────────────────────────────────────────────────────────────────────
	//	AnimNotify
	// ─────────────────────────────────────────────────────────────────────
	// UFUNCTION()
	// void AnimNotify_AttackEnd();
	
};
