// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimNotify_HitboxBase.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "HitBox/Data/HitboxData.h"
#include "AnimNotifyState_HitboxBase.generated.h"

/**
 * Enum describing the different methods for tracing hitboxes in Animation Hitboxes
 */
UENUM(BlueprintType)
enum class EHitboxTraceType : uint8
{
	// Trace a hits-generating shape frame by frame at the authored location.
	TraceFrame,
	// Trace a hits-generating shape from the location in the previous frame to the authored location in the current frame. This allows to generate hits despite low FPS.
	TracePath
};
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UAnimNotifyState_HitboxBase : public UAnimNotifyState
{
	GENERATED_BODY()
	protected:
	// Draw debugs of the hitbox in editor and/or game worlds
	UFUNCTION(BlueprintImplementableEvent, Category = "Anim HitBox")
	void DrawHitShapeDebug(class USkeletalMeshComponent* MeshComp, class UHitbox_CollisionShape* CollisionShape, const FVector& Location, const FQuat& Rotation, const float FrameDeltaTime) const;

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Anim HitBox")
	void SERVER_DrawHitShapeDebug(class USkeletalMeshComponent* MeshComp, class UHitbox_CollisionShape* CollisionShape, const FVector& Location, const FQuat& Rotation, const float FrameDeltaTime) const;

	UFUNCTION(BlueprintCallable, NetMulticast, Reliable, Category = "Anim HitBox")
	void MULTICAST_DrawHitShapeDebug(class USkeletalMeshComponent* MeshComp, class UHitbox_CollisionShape* CollisionShape, const FVector& Location, const FQuat& Rotation, const float FrameDeltaTime) const;

	// Trace the collision shape against the selected trace channel
	UFUNCTION(BlueprintImplementableEvent, Category = "Anim HitBox")
	void ApplyShapeHit(int index, class USkeletalMeshComponent* MeshComp, class UHitbox_CollisionShape* CollisionShape, const FVector& Location, const FQuat& Rotation) const;

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Anim HitBox")
	void SERVER_ApplyShapeHit(int index, class USkeletalMeshComponent* MeshComp, class UHitbox_CollisionShape* CollisionShape, const FVector& Location, const FQuat& Rotation) const;

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Anim HitBox")
	void SetPreviousLoc(int index, const FVector& Location) const;
	
	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Anim HitBox")
	void SetPreviousLocValid(int index, bool Valid) const;

	UFUNCTION(BlueprintCallable, Server, Reliable, Category = "Anim HitBox")
	void SetupPreviousLocs() const;

	virtual TArray<class UHitbox_CollisionShape*> GetCollisionShapes() const { return {}; };

	UPROPERTY(EditAnywhere, Category = "Debugging", BlueprintReadWrite, Replicated)
	FHitboxDebugSettings DebugSettings;

	UPROPERTY(Category = "Debugging", BlueprintReadWrite, Replicated)
	mutable TArray<bool> ValidPreviousLocs;

	UPROPERTY(Category = "Debugging", BlueprintReadWrite, Replicated)
	mutable TArray<FVector> PreviousLocs;
};

class UHitbox_CollisionShape;

UCLASS(Abstract)
class RPGPROJECT_API UHitbox_AnimNotify : public UAnimNotify_HitboxBase
{
	GENERATED_BODY()
protected:

	virtual TArray<UHitbox_CollisionShape*> GetCollisionShapes() const
	{
		return CollisionShapes;
	};

	// Should the collision shape rotate with the bone. THE ROTATION IS APPLIED BEFORE THE ROTATION OFFSET
	UPROPERTY(EditAnywhere, Category = "HitBox", BlueprintReadOnly)
	bool bRotatesWithParent = false;

	// Collision shapes to trace against the world. Each notify detects each hit actor only once.
	UPROPERTY(EditAnywhere, Instanced, Category = "HitBox", BlueprintReadOnly, Replicated)
	TArray<UHitbox_CollisionShape*> CollisionShapes;
};