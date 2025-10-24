// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Hitbox_CollisionShape.generated.h"

/**
 * 
 */
UCLASS(abstract, Category = "Hitbox", EditInlineNew, Blueprintable)
class RPGPROJECT_API UHitbox_CollisionShape : public UObject
{
	GENERATED_BODY()
	public:
	// Parent attachment socket/bone
	UPROPERTY(EditAnywhere, meta = (AnimNotifyBoneName = "true"), Category = "Shape Parameters", BlueprintReadOnly)
		FName ParentSocket = NAME_None;

	// Location offset to apply.
	UPROPERTY(EditAnywhere, Category = "Shape Parameters", BlueprintReadOnly)
		FVector LocationOffset;

	// Rotation offset to apply. If bRotatesWithParent is set to true, then this applied in the Bone space, not world space
	UPROPERTY(EditAnywhere, Category = "Shape Parameters", BlueprintReadOnly)
		FRotator RotationOffset;

	// Get the selected shape (C++ only)
	virtual FCollisionShape GetShape() const;

	UFUNCTION(BlueprintPure, Category = "Collision Shape")
		virtual FVector GetShapeExtentVector() const;

	// Draw this specific shape for debug purposes in the world
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Collision Shape")
		virtual void DrawDebugShape(const UObject* WorldContextObject, const FVector& Location, const FQuat& Rotation, const FColor Color, const float Duration, const float Thickness)
	{
	}

	/**
	 *  Sweep a shape against the world and return all blocking hits against a specific channel
	 *  @param  WorldContextObject	The world context object in which this call is taking place
	 *  @param  OutHits				Array of hits found between ray and the world
	 *  @param  Pos					Location of the shape
	 *  @param  Rot					Rotation of the shape
	 *  @param  TraceChannel		The 'channel' that this ray is in, used to determine which components to hit
	 *  @param	bTraceComplex		Whether we should trace against complex collision
	 *  @param  ActorsToIgnore		Actors to be ignored by ray
	 * 	@param 	bIgnoreSelf			Whether ray should ignore self/owner
	 *  @return TRUE if OutHits contains any blocking hit entries
	 */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Collision Shape")
		virtual bool SweepMultiByChannel(const UObject* WorldContextObject, TArray<struct FHitResult>& OutHits, const FVector& Pos, const FQuat& Rot, const ETraceTypeQuery TraceChannel, const bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, const bool bIgnoreSelf);

	/**
	 *  Sweep a shape against the world and return all blocking hits against a specific channel from a start position to an end position.
	 *  @param  WorldContextObject	The world context object in which this call is taking place
	 *  @param  OutHits				Array of hits found between ray and the world
	 *  @param  StartPos			Start Location of the shape
	 *  @param  EndPos				End Location of the shape
	 *  @param  Rot					Rotation of the shape
	 *  @param  TraceChannel		The 'channel' that this ray is in, used to determine which components to hit
	 *  @param	bTraceComplex		Whether we should trace against complex collision
	 *  @param  ActorsToIgnore		Actors to be ignored by ray
	 * 	@param 	bIgnoreSelf			Whether ray should ignore self/owner
	 *  @return TRUE if OutHits contains any blocking hit entries
	 */
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"), Category = "Collision Shape")
	virtual bool SweepPathMultiByChannel(const UObject* WorldContextObject, TArray<struct FHitResult>& OutHits, const FVector& StartPos, const FVector& EndPos, const FQuat& Rot, const ETraceTypeQuery TraceChannel, const bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, const bool bIgnoreSelf);
};

UCLASS()
class RPGPROJECT_API UHitbox_CollisionBox : public UHitbox_CollisionShape
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, Category = "Shape Parameters", BlueprintReadOnly, meta = (UIMin = 0.f, ClampMin = 0.f))
	FVector Extent = FVector(64.f, 8.f, 8.f);

	virtual FCollisionShape GetShape() const override
	{
		return FCollisionShape::MakeBox(Extent / 2.f);
	}

	virtual FVector GetShapeExtentVector() const override
	{
		return Extent;
	}

	virtual void DrawDebugShape(const UObject* WorldContextObject, const FVector& Location, const FQuat& Rotation, const FColor Color, const float Duration, const float Thickness) override;
};

UCLASS()
class RPGPROJECT_API UHitbox_CollisionCapsule : public UHitbox_CollisionShape
{
	GENERATED_BODY()
public:
	/**
	*	Radius of cap hemispheres and center cylinder.
	*	This cannot be more than CapsuleHalfHeight.
	*/
	UPROPERTY(EditAnywhere, Category = "Shape Parameters", BlueprintReadOnly, meta = (UIMin = 0.f, ClampMin = 0.f))
	float Radius = 32.f;

	/**
	*	Half-height, from center of capsule to the end of top or bottom hemisphere.
	*	This cannot be less than CapsuleRadius.
	*/
	UPROPERTY(EditAnywhere, Category = "Shape Parameters", BlueprintReadOnly, meta = (UIMin = 0.f, ClampMin = 0.f))
	float HalfHeight = 64.f;

	virtual FCollisionShape GetShape() const override
	{
		return FCollisionShape::MakeCapsule(Radius, HalfHeight);
	}

	virtual FVector GetShapeExtentVector() const override
	{
		return FVector(Radius, Radius, HalfHeight);
	}

	virtual void DrawDebugShape(const UObject* WorldContextObject, const FVector& Location, const FQuat& Rotation, const FColor Color, const float Duration, const float Thickness) override;
};