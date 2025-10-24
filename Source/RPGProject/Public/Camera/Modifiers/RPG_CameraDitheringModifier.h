// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/Modifiers/RPG_CameraModifier.h"
#include "Camera/Data/RPG_CameraData.h"
#include "DrawDebugHelpers.h"
#include "Camera/Modifiers/RPG_CameraModifier.h"
#include "RPG_CameraDitheringModifier.generated.h"


UENUM()
enum class EDitherType : uint8
{
	None,
	BlockingLOS,
	OverlappingCamera
};

USTRUCT(BlueprintType)
struct FDitheredActorState
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Camera Dithering")
	TObjectPtr<AActor> Actor;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "RPG Camera Dithering")
	float CurrentOpacity = 1.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "RPG Camera Dithering")
	float CollisionTime = 0.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "RPG Camera Dithering")
	bool bIsDitheringIn = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "RPG Camera Dithering")
	bool bIsDitheringOut = false;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "RPG Camera Dithering")
	EDitherType DitherType = EDitherType::None;

	bool IsValid() const { return Actor != nullptr && DitherType != EDitherType::None; }

	void StartDithering(AActor* InActor, EDitherType InDitherType);

	void Invalidate();

	friend bool operator==(FDitheredActorState const& lhs, FDitheredActorState const& rhs)
	{
		return lhs.Actor == rhs.Actor;
	}

	void ComputeOpacity(float DeltaTime, float DitherInTime, float DitherOutTime, float DitherMin);
};
/**
 * 
 */
UCLASS()
class RPGPROJECT_API URPG_CameraDitheringModifier : public URPG_CameraModifier
{
	GENERATED_BODY()
	
public:
	URPG_CameraDitheringModifier();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Dithering")
	FCameraDitheringSettings DitheringSettings;
protected:
	virtual bool ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV) override;

	virtual void ApplyDithering(float DeltaTime, FDitheredActorState& DitherState);

private:
#if ENABLE_DRAW_DEBUG
	void RPGDebugDithering(FDitheredActorState& DitherState, float DeltaTime, float DitherMin);
#endif

protected:

	/** Material Parameter Collection for everything dithering-related */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RPG Dithering")
	TSoftObjectPtr<class UMaterialParameterCollection> DitheringMPC;

	UPROPERTY(Transient)
	TSoftObjectPtr<class UMaterialParameterCollectionInstance> DitheringMPCInstance;

	UPROPERTY(Transient)
	TArray<FDitheredActorState> DitheredActorStates;
};
