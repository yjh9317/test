// Fill out your copyright notice in the Description page of Project Settings.


#include "HitBox/Actor/Hitbox_AdaptableCollisionActor.h"

// Sets default values
AHitbox_AdaptableCollisionActor::AHitbox_AdaptableCollisionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AHitbox_AdaptableCollisionActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AHitbox_AdaptableCollisionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

