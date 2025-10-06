// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Action/BaseAction.h"
#include "ChanneledAction.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UChanneledAction : public UBaseAction
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Channel")
	float ChannelDuration = 3.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Channel")
	float TickInterval = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Channel")
	bool bCanBeCancelled = true;

protected:
	FTimerHandle ChannelTickTimer;
	FTimerHandle ChannelEndTimer;
	float ChannelStartTime = 0.0f;

	// 채널링 이벤트
	virtual void OnChannelStart() {}
	virtual void OnChannelTick() {}
	virtual void OnChannelEnd(bool bWasCancelled) {}

private:
	void TickChannel() {};
	void EndChannel() {};
};
