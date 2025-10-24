// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu/Actor/MenuSystemActor.h"

// Sets default values
AMenuSystemActor::AMenuSystemActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMenuSystemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AMenuSystemActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

