// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "InputTriggers.h"
#include "InputReceiverInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInputReceiverInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGPROJECT_API IInputReceiverInterface
{
	GENERATED_BODY()

public:
	/**
	 * Gameplay Tag와 함께 입력 이벤트가 발생했을 때 호출됩니다.
	 * @param InputTag 발생한 입력에 해당하는 Gameplay Tag
	 * @param TriggerEvent 입력 이벤트 종류 (Started, Triggered, Completed 등)
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Input")
	void HandleInputTagEvent(FGameplayTag InputTag, ETriggerEvent TriggerEvent);
};
