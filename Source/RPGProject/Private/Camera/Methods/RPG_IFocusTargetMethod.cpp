// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/Methods/RPG_IFocusTargetMethod.h"

AActor* URPG_IFocusTargetMethod::GetTargetLocation_Implementation(AActor* InOwner, FVector OwnerLocation,
	FVector ViewPointLocation, FRotator ViewPointRotation, FVector& OutTargetLocation)
{
	return nullptr;
}

UWorld* URPG_IFocusTargetMethod::GetWorld() const
{
	return UObject::GetWorld();
}
