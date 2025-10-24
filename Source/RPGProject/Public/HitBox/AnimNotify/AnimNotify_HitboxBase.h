// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "HitBox/Data/HitboxData.h"
#include "AnimNotify_HitboxBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class RPGPROJECT_API UAnimNotify_HitboxBase : public UAnimNotify
{
	GENERATED_BODY()
public:
	// Parent attachment socket/bone
	UPROPERTY(EditAnywhere, meta = (AnimNotifyBoneName = "true"), Category = "Attachment Setup", BlueprintReadOnly)
	FName ParentSocket = NAME_None;
	
	// UPROPERTY(EditAnywhere, Category = "Debugging", BlueprintReadWrite)
	FHitboxDebugSettings DebugSettings;
};
