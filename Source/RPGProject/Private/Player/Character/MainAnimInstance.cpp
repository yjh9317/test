
#include "Player/Character/MainAnimInstance.h"
#include "Shared/Components/CustomMovementComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Header/DebugHeader.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/Character/MainCharacter.h"

UMainAnimInstance::UMainAnimInstance(const FObjectInitializer& ObjectInitializer)
	:UBaseAnimInstance(ObjectInitializer)
{
}

void UMainAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UMainAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	Player = Cast<AMainCharacter>(OwnerCharacter);
	if (Player)
	{
		PlayerMovement = Player->GetCustomMovementComponent();
	}
	
}

void UMainAnimInstance::GetInputInfo()
{
	InputDirectionX = Player->GetMovementDirectionInput().X;
	InputDirectionX = Player->GetMovementDirectionInput().Y;

	if(bIsAccelerating && SpeedLength > 200.f)
	{
		InputDegreeDirection = UKismetMathLibrary::Atan2(InputDirectionX,InputDirectionY);
	}

	// no input == backward
	if(InputDirectionX == 0 && InputDirectionY == 0)
	{
		InputDegreeDirection = 180.f;
	}
}

void UMainAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(!Player || !PlayerMovement) return;
	
	

	GetSpeedVariables();
	GetIsClimbing();
	GetClimbVelocity();
	GetWeaponInfo();
	GetViewRotation(DeltaSeconds);
	GetAimBlend(DeltaSeconds);
	GetInputInfo();
}

void UMainAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	
}

#pragma region Get


void UMainAnimInstance::GetSpeedVariables()
{
	FVector Velocity = Player->GetVelocity();
	Velocity.Z = 0.f;
		
	SpeedLength = FMath::FInterpTo(SpeedLength,Velocity.Length(),GetDeltaSeconds(),20.f);
	if(bIsAccelerating)
	{
		float SpeedX = FVector::DotProduct(Velocity, Player->GetActorForwardVector());
		float SpeedY = FVector::DotProduct(Velocity, Player->GetActorRightVector());
		SpeedDegree = FMath::RadiansToDegrees(FMath::Atan2(SpeedY,SpeedX));
		if(SpeedDegree > 200.f)
		{
			LastSpeedDegree = SpeedDegree;
		}
	}
}

void UMainAnimInstance::GetIsClimbing()
{
	bIsClimbing = PlayerMovement->IsClimbing();
}

void UMainAnimInstance::GetClimbVelocity()
{
	ClimbVelocity = PlayerMovement->GetUnrotatedClimbVelocity();		
}

void UMainAnimInstance::GetWeaponInfo()
{
	bWeaponEquipped = Player->IsWeaponEquipped();
	// if(Player->GetCombatComponent() && bWeaponEquipped)
	// {
	// 	CurrentWeaponTag = Player->GetCombatComponent()->EquippedWeaponTypeTag;
	// }
}


void UMainAnimInstance::GetViewRotation(float DeltaSeconds)
{
	FRotator ControlRot = Player->GetControlRotation();
	FRotator PlayerRot = Player->GetActorRotation();
	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(ControlRot,PlayerRot);

	float AimOffsetYaw  = DeltaRot.Yaw;
	float AimOffsetPitch  = DeltaRot.Pitch;
	
	float Yaw = FMath::FInterpTo(ViewRotation.Yaw,AimOffsetYaw,DeltaSeconds,10.f);
	float Pitch = FMath::FInterpTo(ViewRotation.Pitch,AimOffsetPitch,DeltaSeconds,10.f);
	ViewRotation = FRotator(Pitch,Yaw,0.f);
}

void UMainAnimInstance::GetAimBlend(float DeltaSeconds)
{
	
}

#pragma endregion
