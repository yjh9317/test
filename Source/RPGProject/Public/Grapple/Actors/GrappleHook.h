// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GrappleHook.generated.h"

class UProjectileMovementComponent;
UCLASS()
class RPGPROJECT_API AGrappleHook : public AActor
{
	GENERATED_BODY()
	
public:
	friend class UGrappleComponent;
	// Sets default values for this actor's properties
	AGrappleHook();

protected:
	friend class UGrappleComponent;
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMeshComponent;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovementComponent;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess = "true"),Category="Init")
	float InitialMaxSpeed;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess = "true"),Category="Init")
	FVector InitVelocity;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess = "true"),Category="Init")
	float InitSpeed;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess = "true"),Category="Init")
	FVector StartLoc;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,meta=(AllowPrivateAccess = "true"),Category="Init")
	float fMaxDist = 1500.f;
public:
	void SetVelocity(const FVector& Direction,const float Speed);
};
