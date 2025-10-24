// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "HitDetectionNotify.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UHitDetectionNotify : public UAnimNotify
{
	GENERATED_BODY()
public:
	// 간단한 토글 방식
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Detection")
	bool bEnableHitDetection = true;

	// 히트 대상 리셋 옵션
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Hit Detection")
	bool bResetHitActors = false;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

#if WITH_EDITOR
	virtual FString GetNotifyName_Implementation() const override
	{
		return bEnableHitDetection ? TEXT("Hit: Enable") : TEXT("Hit: Disable");
	}
#endif
};
