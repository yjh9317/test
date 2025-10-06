// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ANS_SpeedModifier.generated.h"

/**
 * 
 */
UCLASS(editinlinenew,Blueprintable, meta = (ShowWorldContextPin, DisplayName = "SpeedModifier"))
class RPGPROJECT_API UANS_SpeedModifier : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta = (ClampMin = "0.1", ClampMax = "3.0"), Category = SpeedModifier)
	float StartSpeed = 1.f;

	UPROPERTY(EditAnywhere,BlueprintReadWrite, meta = (ClampMin = "0.1", ClampMax = "3.0"), Category = SpeedModifier)
	float EndSpeed = 1.f;

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
