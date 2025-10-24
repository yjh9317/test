// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "ComboInputWindowNotifyState.generated.h"

class UMeleeAttackAction;
class UInputBufferComponent;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UComboInputWindowNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()
	    
public:
	UPROPERTY(EditAnywhere, Category = "Input Window")
	float PreBufferTime = 0.2f;
    
	UPROPERTY(EditAnywhere, Category = "Input Window")
	FGameplayTag AcceptedInputTag;

	UPROPERTY(EditAnywhere, Category = "Input Window")
	bool bDebugLog = true;
    
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
    
private:
	UPROPERTY()
	TWeakObjectPtr<UInputBufferComponent> CachedInputBuffer;
    
	UPROPERTY()
	TWeakObjectPtr<UMeleeAttackAction> CachedMeleeAction;

};
