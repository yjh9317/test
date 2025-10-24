
#include "Character/BaseAnimInstance.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Header/DebugHeader.h"
#include "Kismet/KismetMathLibrary.h"

UBaseAnimInstance::UBaseAnimInstance(const FObjectInitializer& ObjectInitializer)
{
}

void UBaseAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	OwnerCharacter = Cast<ACharacter>(TryGetPawnOwner());

	if(OwnerCharacter)
	{
		OwnerMovement = OwnerCharacter->GetCharacterMovement();
	}
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(!OwnerCharacter || !OwnerMovement) return;

	bIsAccelerating = OwnerMovement->GetCurrentAcceleration().Size() > 0;
	AcceleratingBlend = FMath::FInterpTo(AcceleratingBlend,bIsAccelerating,DeltaSeconds,10.f);
		
	GetGroundSpeed();
	GetAirSpeed();
	GetShouldMove();
	GetIsFalling();
	GetIsCrouching();
	UpdateMovementState();
	UpdateTurnInPlace(DeltaSeconds);
}

void UBaseAnimInstance::UpdateTurnInPlace(float DeltaSeconds)
{
	if (GroundSpeed > 5.0f)
	{
		RootYawOffset = 0.0f;
		return;
	}
    
	// 정지 상태에서만 Turn in Place
	float TargetYaw = OwnerCharacter->GetControlRotation().Yaw;
	float ActorYaw = OwnerCharacter->GetActorRotation().Yaw;
    
	RootYawOffset = UKismetMathLibrary::NormalizeAxis(TargetYaw - ActorYaw);
    
	if (FMath::Abs(RootYawOffset) > TurnInPlaceThreshold)
	{
		bShouldTurnInPlace = true;
		TurnDirection = RootYawOffset > 0 ? ETurnDirection::Right : ETurnDirection::Left;
	}
}

void UBaseAnimInstance::UpdateMovementState()
{
	if (GroundSpeed <= MovementStateSpeed.IdleSpeed)
		MovementState = EMovementState::Idle;
	else if (GroundSpeed <= MovementStateSpeed.WalkSpeed)
		MovementState = EMovementState::Walk;
	else if (GroundSpeed <= MovementStateSpeed.JogSpeed)
		MovementState = EMovementState::Jog;
	else if (GroundSpeed <= MovementStateSpeed.RunSpeed)
		MovementState = EMovementState::Run;
}

void UBaseAnimInstance::GetGroundSpeed()
{
	GroundForwardSpeed = FVector::DotProduct(OwnerCharacter->GetVelocity(), OwnerCharacter->GetActorForwardVector());
	GroundRightSpeed = FVector::DotProduct(OwnerCharacter->GetVelocity(), OwnerCharacter->GetActorRightVector());
	GroundSpeed = UKismetMathLibrary::VSizeXY(OwnerCharacter->GetVelocity());
}

void UBaseAnimInstance::GetAirSpeed()
{
	FallSpeed = OwnerCharacter->GetVelocity().Z;
}

void UBaseAnimInstance::GetShouldMove()
{
	bShouldMove = OwnerMovement->GetCurrentAcceleration().Size() > 0 && GroundSpeed > 5.f && !bIsInAir;
}

void UBaseAnimInstance::GetIsFalling()
{
	bIsInAir = OwnerMovement->IsFalling();
}

void UBaseAnimInstance::GetIsCrouching()
{
	bIsCrouching = OwnerMovement->IsCrouching();
}
