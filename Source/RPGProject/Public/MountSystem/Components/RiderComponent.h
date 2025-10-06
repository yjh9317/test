// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RiderComponent.generated.h"


class UCameraComponent;
class USpringArmComponent;
class UMountComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API URiderComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	URiderComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY()
	ACharacter* Character;
	UPROPERTY()
	UMountComponent* MountComponent;
	UPROPERTY()
	USpringArmComponent* SpringArmComponent;
	UPROPERTY()
	UCameraComponent* CameraComponent;
	
protected:

	virtual void BeginPlay() override;

	FVector CameraRelativeLocation;
	FVector CustomSpringArmLocation;
	FVector CustomCameraLocation;
	FRotator CustomCameraRotator;
	float CameraOverTime = 0.5f;

	float AimingYaw;
	
	bool IsPossessPawn;
};
