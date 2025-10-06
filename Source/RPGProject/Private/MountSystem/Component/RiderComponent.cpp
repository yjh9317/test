// Fill out your copyright notice in the Description page of Project Settings.


#include "MountSystem/Components/RiderComponent.h"

// Sets default values for this component's properties
URiderComponent::URiderComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void URiderComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void URiderComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

