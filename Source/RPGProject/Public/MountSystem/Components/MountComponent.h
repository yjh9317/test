// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MountComponent.generated.h"


class URiderComponent;
class USpringArmComponent;
class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UMountComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMountComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	FName GetSocketName(FName NewSocketName);
	void ChangeSocketName(FName NewName);

	void SetCharacterControlRotation(const FRotator& RiderControlRotation);
	void SetCameraPosition();
	void ResetControlRotation();
	
	UPROPERTY()
	ACharacter* OwnerCharacter;
	UPROPERTY()
	URiderComponent* RiderComponent;
	UPROPERTY()
	USpringArmComponent* SpringArmComponent;
	UPROPERTY()
	UCameraComponent* CameraComponent;

protected:
	virtual void BeginPlay() override;

	

	FRotator ControlRotation;
	FVector CameraRelativeLocation;
	FVector SpringArmRelativeLocation;
	bool UseRiderSpringArmLocation;
	float MoveSpringArmOverTime;
	float MoveCameraOverTime;
	
	bool IsPossessPawn;
	FName MountPointSocket;
};
