// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnimalMovementComponent.generated.h"

UENUM()
enum EAnimalMovementMode
{
	Walking,
	Flying,
	Swimming
};

class UCharacterMovementComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UAnimalMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAnimalMovementComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
protected:
	virtual void BeginPlay() override;


	UPROPERTY()
	ACharacter* OwnerCharacter;
	UPROPERTY()
	UCharacterMovementComponent* OwnerCharacterMovementComponent;

	float DeltaSeconds;

	/* Fly */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "Fly")
	float FlyInPlaceSpeedScale;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "Fly")
	float Max_Fly_Height;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "Fly")
	float Min_Fly_Height;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "Fly")
	float Timeline_Up_PlayRate;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "Fly")
	float Timeline_Down_PlayRate;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "Fly")
	float Timeline_Up_Reverse_PlayRate;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "Fly")
	float Timeline_Down_Reverse_PlayRate;

	/* Movement */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "Movement")
	float InputAxisLeftRight;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "Movement")
	float InputAxisForwardBackward;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "Movement")
	float LastInputAxisRightLeft;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "Movement")
	float InputAxisUpDown;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "Movement")
	bool Jumped;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "Movement")
	bool TurnInPlace;
	// EAnimalMovementMode
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "Movement")
	float FlyAngle;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "Movement")
	float NextForwardSpeed;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "Movement")
	bool IsTurningDeceleration;
	
	/* Timeline Value*/
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "Timeline Value")
	float TimelineValue_Speed;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "Timeline Value")
	float TimelineValue_Rotation;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "Timeline Value")
	float TimelineValue_Down;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "Timeline Value")
	float TimelineValue_Up;

	/* Cached */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "Cached Value")
	float CachedMaxAcceleration;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "CachedValue")
	float CachedBrakingDecelerationFlying;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "CachedValue")
	float CachedRotationRoll;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "CachedValue")
	FVector CacheInPlaceVector;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "CachedValue")
	FRotator CachedRotationLastTick;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category= "CachedValue")
	float CachedYawDelta;

	// bool IsAutoMove;
private:
	// bool CheckAutoMove();
	// void AutoMove();
	void CreateDefaultTimeline();
	void SetDefaultCharacterMovementConfig();
};
