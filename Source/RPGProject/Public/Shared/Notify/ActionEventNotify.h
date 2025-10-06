// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Shared/Action/Data/ActionData.h"
#include "ActionEventNotify.generated.h"

class UMeleeAttackAction;
class UActionComponent;

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UActionEventNotify : public UAnimNotify
{
	GENERATED_BODY()

  
public:
	UPROPERTY(EditAnywhere)
	EActionEvent EventType;

	UPROPERTY(EditAnywhere)
	FGameplayTag EventActionTag;
    
	UPROPERTY(EditAnywhere)
	float EventValue = 1.0f;
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
	// UActionComponent* GetCachedActionComponent(AActor* Owner);
	// UMeleeAttackAction* GetCachedMeleeAction(UActionComponent* ActionComp);
};
