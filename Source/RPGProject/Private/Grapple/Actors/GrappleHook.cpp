// Fill out your copyright notice in the Description page of Project Settings.


#include "Grapple/Actors/GrappleHook.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AGrappleHook::AGrappleHook()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	SetRootComponent(StaticMeshComponent);
	
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 0.f;
	ProjectileMovementComponent->MaxSpeed = 5000.f;

	InitialLifeSpan = 15.0f;
}

// Called when the game starts or when spawned
void AGrappleHook::BeginPlay()
{
	Super::BeginPlay();

	StartLoc = GetActorLocation();
}

// Called every frame
void AGrappleHook::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(FVector::Distance(StartLoc, GetActorLocation()) >= fMaxDist)
	{
		Destroy();
	}
}

void AGrappleHook::SetVelocity(const FVector& Direction, const float Speed)
{
	ProjectileMovementComponent->InitialSpeed = Speed;
	ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;
}



