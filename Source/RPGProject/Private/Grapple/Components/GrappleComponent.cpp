// Fill out your copyright notice in the Description page of Project Settings.


#include "Grapple/Components/GrappleComponent.h"
#include "Grapple/Components/GrappleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "CableComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Grapple/Actors/GrappleHook.h"
#include "Player/Character/MainCharacter.h"

UGrappleComponent::UGrappleComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UGrappleComponent::BeginPlay()
{
	Super::BeginPlay();
	MainCharacter = Cast<AMainCharacter>(GetOwner());
	if(MainCharacter)
	{
		CharacterMovementComp = MainCharacter->GetCharacterMovement();
	}	
}

void UGrappleComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ApplyForceWhenAttachedToTarget();
}

void UGrappleComponent::GrappleFire(const FVector& TargetLoc, const FVector& LocalOffset)
{
	GrappleState = ERS_Firing;
	FireDirection = (TargetLoc - CableStartLocation(LocalOffset)).GetSafeNormal();
	
	if(GrappleHookActorClass)
	{
		FVector FirePosition = GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 50.f;
		GrappleHookActor = GetWorld()->SpawnActor<AGrappleHook>(GrappleHookActorClass,FirePosition,FRotator::ZeroRotator);
		GrappleHookActor->SetVelocity(FireDirection,GrappleSpeed);
		
		GrappleHookActor->StaticMeshComponent->OnComponentHit.AddDynamic(this,&UGrappleComponent::GrappleOnHit);
		GrappleHookActor->OnDestroyed.AddDynamic(this,&UGrappleComponent::GrappleHookDestroyed);

		if(GrappleCableClass)
		{
			FRotator Rot = UKismetMathLibrary::MakeRotFromX(FireDirection);
			GrappleCable = GetWorld()->SpawnActor<ACableActor>(GrappleCableClass,CableStartLocation(LocalOffset),Rot,FActorSpawnParameters{});
		
			GrappleCable->AttachToActor(GetOwner(),FAttachmentTransformRules::KeepWorldTransform);
		
			GrappleCable->CableComponent->SetAttachEndTo(GrappleHookActor,FName{});
			GrappleCable->CableComponent->EndLocation = FVector::ZeroVector;
		}	
	}
}

void UGrappleComponent::CancelGrapple()
{
	if(GrappleHookActor) GrappleHookActor->Destroy();
}


void UGrappleComponent::GrappleHookDestroyed(AActor* DestoryedActor)
{
	GrappleState = ERS_ReadyToFire;
	InitHookDirection2D = FVector::ZeroVector;
	if(GrappleCable)
	{
		GrappleCable->Destroy();
		GrappleCable = nullptr;
	}

	if(UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(MainCharacter->GetCharacterMovement()))
	{
		MovementComponent->GroundFriction = 1.f;
		MovementComponent->GravityScale = 1.f;
		MovementComponent->AirControl = 0.05f;
	}
}

void UGrappleComponent::GrappleOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	GrappleState = ERS_AttachedToTarget;

	if(MainCharacter)
	{
		if(UCharacterMovementComponent* MovementComponent = Cast<UCharacterMovementComponent>(MainCharacter->GetCharacterMovement()))
		{
			MovementComponent->GroundFriction = fGroundFriction;
			MovementComponent->GravityScale = fGravityScale;
			MovementComponent->AirControl = fAirControl;
			
			const FVector Dir = GetFireDirection();
			MovementComponent->Velocity = fInitialForce * Dir;
			InitHookDirection2D = FVector(Dir.X,Dir.Y,0.f);
		}
	}
}

FVector UGrappleComponent::CableStartLocation(const FVector& LocalOffset)
{
	return (GetOwner()->GetActorLocation() + UKismetMathLibrary::TransformDirection(
		GetOwner()->GetActorTransform(), LocalOffset));
}

void UGrappleComponent::ApplyForceWhenAttachedToTarget()
{
	if(GrappleState != ERS_AttachedToTarget) return;

	if (CharacterMovementComp)
	{
		CharacterMovementComp->AddForce(GetFireDirection() * fApplyForce);
	}
	
	if(GrappleHookActor)
	{
		const float Dist = FVector::Distance(GrappleHookActor->GetActorLocation(),GetOwner()->GetActorLocation());
		if(Dist < fApplyForceDist)
		{
			GrappleHookActor->Destroy();
			GrappleHookActor = nullptr;
		}
		if (FVector::DotProduct(InitHookDirection2D, GetFireDirection2D()) < 0.f)
		{
			GrappleHookActor->Destroy();
		}
	}
}

FVector UGrappleComponent::GetFireDirection() const
{
	if(GrappleHookActor && MainCharacter)
	{
		return (GrappleHookActor->GetActorLocation() - MainCharacter->GetActorLocation()).GetSafeNormal();  
	}
	return FVector::ZeroVector;
}

FVector UGrappleComponent::GetFireDirection2D() const
{
	if(GrappleHookActor && MainCharacter)
	{
		return (GrappleHookActor->GetActorLocation() - MainCharacter->GetActorLocation()).GetSafeNormal2D();  
	}
	return FVector::ZeroVector;
}


bool UGrappleComponent::IsUse() const
{
	return (GrappleState == ERS_Firing || GrappleState == ERS_AttachedToTarget);
}

 