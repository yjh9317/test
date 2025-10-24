// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CableActor.h"
#include "Player/Character/MainCharacter.h"
#include "Grapple/Actors/GrappleHook.h"
#include "GrappleComponent.generated.h"

class UCharacterMovementComponent;
UENUM(BlueprintType)
enum EGrappleState
{
	ERS_ReadyToFire UMETA(DisplayName = "ReadyToFire"),
	ERS_Firing UMETA(DisplayName = "Firing"),
	ERS_AttachedToTarget UMETA(DisplayName = "AttachedToTarget"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UGrappleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGrappleComponent();

protected:
	UPROPERTY()
	AMainCharacter* MainCharacter;
	UPROPERTY()
	UCharacterMovementComponent* CharacterMovementComp;

	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Grapple")
	TEnumAsByte<EGrappleState> GrappleState;
	
	// Actor
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Grapple")
	TSubclassOf<AGrappleHook> GrappleHookActorClass;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Grapple")
	AGrappleHook* GrappleHookActor;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Grapple")
	TSubclassOf<ACableActor> GrappleCableClass;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Grapple")
	ACableActor* GrappleCable;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Grapple")
	FVector FireDirection;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Grapple")
	float GrappleSpeed = 1000.f;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Grapple")
	FVector InitHookDirection2D;
	// Movement Properties
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Grapple|Movement")
	float fAirControl = 0.2f;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Grapple|Movement")
	float fGroundFriction;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Grapple|Movement")
	float fGravityScale;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Grapple|Movement")
	float fInitialForce = 1200.f;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Grapple|Movement")
	float fApplyForce = 10000.f;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="Grapple|Movement")
	float fApplyForceDist = 250.f;

	virtual void BeginPlay() override;
private:
	UFUNCTION()
	void GrappleHookDestroyed(AActor* DestoryedActor);
	
	UFUNCTION()    
	void GrappleOnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	FVector GetFireDirection() const;
	FVector GetFireDirection2D() const;
	FVector CableStartLocation(const FVector& LocalOffset);
	void ApplyForceWhenAttachedToTarget();
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void GrappleFire(const FVector& TargetLoc, const FVector& LocalOffset);
	void CancelGrapple();
	bool IsUse() const;
	
};
