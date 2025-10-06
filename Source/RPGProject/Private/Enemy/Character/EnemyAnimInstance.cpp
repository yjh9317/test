#pragma once


#include "Enemy/Character/EnemyAnimInstance.h"

UEnemyAnimInstance::UEnemyAnimInstance(const FObjectInitializer& ObjectInitializer)
	:UBaseAnimInstance(ObjectInitializer)
{
}

void UEnemyAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
}

void UEnemyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
}

void UEnemyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
}
