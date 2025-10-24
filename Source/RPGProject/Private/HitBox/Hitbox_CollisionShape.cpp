// Fill out your copyright notice in the Description page of Project Settings.


#include "HitBox/Hitbox_CollisionShape.h"

#include "PhysicsEngine/PhysicsSettings.h"

FCollisionShape UHitbox_CollisionShape::GetShape() const
{
	return FCollisionShape::MakeSphere(32.f);
}

FVector UHitbox_CollisionShape::GetShapeExtentVector() const
{
	return FVector(32.f, 32.f, 32.f);
}

bool UHitbox_CollisionShape::SweepMultiByChannel(const UObject* WorldContextObject, TArray<FHitResult>& OutHits,
	const FVector& Pos, const FQuat& Rot, const ETraceTypeQuery TraceChannel, const bool bTraceComplex,
	const TArray<AActor*>& ActorsToIgnore, const bool bIgnoreSelf)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
		return false;

	const ECollisionChannel CollisionChannel = UEngineTypes::ConvertToCollisionChannel(TraceChannel);

	// Setup Collision Query Params
	FCollisionQueryParams Params(TEXT("SweepMultiByChannel"), SCENE_QUERY_STAT_ONLY(KismetTraceUtils), bTraceComplex);
	Params.bReturnPhysicalMaterial = true;
	Params.bReturnFaceIndex = !UPhysicsSettings::Get()->bSuppressFaceRemapTable; // Ask for face index, as long as we didn't disable globally
	Params.AddIgnoredActors(ActorsToIgnore);
	if (bIgnoreSelf)
	{
		const AActor* IgnoreActor = Cast<AActor>(WorldContextObject);
		if (IgnoreActor)
		{
			Params.AddIgnoredActor(IgnoreActor);
		}
		else
		{
			// find owner
			const UObject* CurrentObject = WorldContextObject;
			while (CurrentObject)
			{
				CurrentObject = CurrentObject->GetOuter();
				IgnoreActor = Cast<AActor>(CurrentObject);
				if (IgnoreActor)
				{
					Params.AddIgnoredActor(IgnoreActor);
					break;
				}
			}
		}
	}

	return World->SweepMultiByChannel(OutHits, Pos, Pos + 0.01f, Rot, CollisionChannel, GetShape(), Params);
}

bool UHitbox_CollisionShape::SweepPathMultiByChannel(const UObject* WorldContextObject, TArray<FHitResult>& OutHits,
	const FVector& StartPos, const FVector& EndPos, const FQuat& Rot, const ETraceTypeQuery TraceChannel,
	const bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, const bool bIgnoreSelf)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
		return false;

	const ECollisionChannel CollisionChannel = UEngineTypes::ConvertToCollisionChannel(TraceChannel);

	// Setup Collision Query Params
	FCollisionQueryParams Params(TEXT("SweepPathMultiByChannel"), SCENE_QUERY_STAT_ONLY(KismetTraceUtils), bTraceComplex);
	Params.bReturnPhysicalMaterial = true;
	Params.bReturnFaceIndex = !UPhysicsSettings::Get()->bSuppressFaceRemapTable; // Ask for face index, as long as we didn't disable globally
	Params.AddIgnoredActors(ActorsToIgnore);
	if (bIgnoreSelf)
	{
		const AActor* IgnoreActor = Cast<AActor>(WorldContextObject);
		if (IgnoreActor)
		{
			Params.AddIgnoredActor(IgnoreActor);
		}
		else
		{
			// find owner
			const UObject* CurrentObject = WorldContextObject;
			while (CurrentObject)
			{
				CurrentObject = CurrentObject->GetOuter();
				IgnoreActor = Cast<AActor>(CurrentObject);
				if (IgnoreActor)
				{
					Params.AddIgnoredActor(IgnoreActor);
					break;
				}
			}
		}
	}

	return World->SweepMultiByChannel(OutHits, StartPos, EndPos, Rot, CollisionChannel, GetShape(), Params);
}

void UHitbox_CollisionBox::DrawDebugShape(const UObject* WorldContextObject, const FVector& Location,
	const FQuat& Rotation, const FColor Color, const float Duration, const float Thickness)
{
	// Draw Box in given world
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
		return;
	
	DrawDebugBox(World, Location, Extent / 2.f, Rotation, Color, false, Duration, 0, Thickness);
}

void UHitbox_CollisionCapsule::DrawDebugShape(const UObject* WorldContextObject, const FVector& Location,
                                                   const FQuat& Rotation, const FColor Color, const float Duration, const float Thickness)
{
	// Draw Capsule in given world
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
		return;
	DrawDebugCapsule(World, Location, HalfHeight, Radius, Rotation, Color, false, Duration, 0, Thickness);
}
