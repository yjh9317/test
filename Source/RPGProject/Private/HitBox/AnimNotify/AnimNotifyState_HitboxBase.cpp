// Fill out your copyright notice in the Description page of Project Settings.


#include "HitBox/AnimNotify/AnimNotifyState_HitboxBase.h"

#include "Net/UnrealNetwork.h"

void UAnimNotifyState_HitboxBase::SERVER_DrawHitShapeDebug_Implementation(USkeletalMeshComponent* MeshComp, class UHitbox_CollisionShape* CollisionShape, const FVector& Location, const FQuat& Rotation, const float FrameDeltaTime) const
{
	MULTICAST_DrawHitShapeDebug_Implementation(MeshComp, CollisionShape, Location, Rotation, FrameDeltaTime);
}

void UAnimNotifyState_HitboxBase::MULTICAST_DrawHitShapeDebug_Implementation(USkeletalMeshComponent* MeshComp, UHitbox_CollisionShape* CollisionShape, const FVector& Location, const FQuat& Rotation, const float FrameDeltaTime) const
{
	DrawHitShapeDebug(MeshComp, CollisionShape, Location, Rotation, FrameDeltaTime);
}

void UAnimNotifyState_HitboxBase::SERVER_ApplyShapeHit_Implementation(int index, USkeletalMeshComponent* MeshComp, UHitbox_CollisionShape* CollisionShape, const FVector& Location, const FQuat& Rotation) const
{
	ApplyShapeHit(index, MeshComp, CollisionShape, Location, Rotation);
}

void UAnimNotifyState_HitboxBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(UAnimNotifyState_HitboxBase, DebugSettings);
	DOREPLIFETIME(UAnimNotifyState_HitboxBase, ValidPreviousLocs);
	DOREPLIFETIME(UAnimNotifyState_HitboxBase, PreviousLocs);
}

void UAnimNotifyState_HitboxBase::SetPreviousLoc_Implementation(int index, const FVector& Location) const
{
	if (PreviousLocs.IsEmpty() || index < 0 || index >= PreviousLocs.Num())
	{
		return;
	}

	PreviousLocs[index] = Location;
}

void UAnimNotifyState_HitboxBase::SetPreviousLocValid_Implementation(int index, bool Valid) const
{
	if (ValidPreviousLocs.IsEmpty() || index < 0 || index >= ValidPreviousLocs.Num())
	{
		return;
	}
	ValidPreviousLocs[index] = Valid;
}

void UAnimNotifyState_HitboxBase::SetupPreviousLocs_Implementation() const
{
	auto const& CollisionShapes = GetCollisionShapes();
	if (CollisionShapes.IsEmpty())
	{
		return;
	}

	PreviousLocs.Empty();
	ValidPreviousLocs.Empty();

	PreviousLocs.AddDefaulted(CollisionShapes.Num());
	ValidPreviousLocs.AddDefaulted(CollisionShapes.Num());

	check(PreviousLocs.Num() == CollisionShapes.Num());
	check(PreviousLocs.Num() == ValidPreviousLocs.Num());
}

void UHitbox_AnimNotify::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UHitbox_AnimNotify, CollisionShapes);
}