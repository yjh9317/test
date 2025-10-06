// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Action/BaseAction.h"
#include "ReactionAction.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class RPGPROJECT_API UReactionAction : public UBaseAction
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reaction")
	float ReactionWindow = 0.5f;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reaction")
	FGameplayTagContainer TriggerTags; // 반응할 태그들
    
	// 블루프린트 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Reaction")
	bool ShouldReact(AActor* Instigator, const FGameplayTag& TriggerTag);
    
	UFUNCTION(BlueprintImplementableEvent, Category = "Reaction")
	void OnReactionTriggered(AActor* Instigator, const FGameplayTag& TriggerTag);
    
	// 반응 대기 활성화
	UFUNCTION(BlueprintCallable, Category = "Reaction")
	void EnableReactionWindow() {};
};
