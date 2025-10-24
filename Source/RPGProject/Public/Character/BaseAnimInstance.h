#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "BaseAnimInstance.generated.h"

UENUM(BlueprintType)
enum class ELocomotionState : uint8
{
	Idle,
	Walking,
	Jogging,
	Running,
	Crouching,
	CrouchWalking,
	Jumping,
	InAir,
	Landing,
	TurnInPlace
};

UENUM(BlueprintType)
enum class EMovementState : uint8
{
	Idle,       
	Walk,       
	Jog,        
	Run         
};

UENUM(BlueprintType)
enum class ETurnDirection : uint8
{
	Left,       
	Right,       
	Forward,      
	Backward    
};

USTRUCT(BlueprintType)
struct FMovementStateSpeed
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	float IdleSpeed = 5.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	float WalkSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	float JogSpeed = 400.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
	float RunSpeed = 600.f;
};

class UCharacterMovementComponent;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	UBaseAnimInstance(const FObjectInitializer& ObjectInitializer);
	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta=(AllowPrivateAccess="true"))
	ACharacter* OwnerCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta=(AllowPrivateAccess="true"))
	UCharacterMovementComponent* OwnerMovement;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	float GroundForwardSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	float GroundRightSpeed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	bool bShouldMove;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	bool bWasMoving;	
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	bool bIsInAir;					// 공중 상태

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	bool bWasInAir;                  // 착지 감지용

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	float FallSpeed;                 // 낙하 속도 (착지 강도)
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	bool bIsAccelerating;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	float AcceleratingBlend = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	bool bIsRun;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	bool bIsCrouching;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	bool bIsGuarding;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	float StanceTransitionAlpha;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess="true"))
	bool bRoll;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	bool bIsDead;

	UPROPERTY(BlueprintReadOnly, Category = "TurnInPlace")
	ETurnDirection TurnDirection;
	
	UPROPERTY(BlueprintReadOnly, Category = "State")
	EMovementState MovementState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "State")
	FMovementStateSpeed MovementStateSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TurnInPlace", meta=(AllowPrivateAccess="true"))
	float RootYawOffset;             // 루트 본 회전 오프셋
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TurnInPlace", meta=(AllowPrivateAccess="true"))
	float TurnInPlaceThreshold = 90.0f;  // 회전 시작 각도
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "TurnInPlace", meta=(AllowPrivateAccess="true"))
	bool bShouldTurnInPlace;

	UPROPERTY(BlueprintReadOnly, Category = "Blending")
	float UpperBodyOverrideWeight;   // 상체 오버라이드 가중치
    
	UPROPERTY(BlueprintReadOnly, Category = "Blending")
	float AdditiveAlpha;   

	UPROPERTY(BlueprintReadOnly, Category = "IK")
	FVector LeftHandIKLocation;      // 왼손 IK
    
	UPROPERTY(BlueprintReadOnly, Category = "IK")
	FVector RightHandIKLocation;     // 오른손 IK

	UPROPERTY(BlueprintReadOnly, Category = "IK")
	float HandIKAlpha;

public:
	UFUNCTION(BlueprintCallable)
	void UpdateTurnInPlace(float DeltaSeconds);
	UFUNCTION(BlueprintCallable)
	void UpdateMovementState();

	UFUNCTION(BlueprintCallable)
	void GetGroundSpeed();
	UFUNCTION(BlueprintCallable)
	void GetAirSpeed();
	UFUNCTION(BlueprintCallable)
	void GetShouldMove();
	UFUNCTION(BlueprintCallable)
	void GetIsFalling();
	UFUNCTION(BlueprintCallable)
	void GetIsCrouching();
};
