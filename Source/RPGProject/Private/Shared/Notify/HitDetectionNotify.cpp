// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/Notify/HitDetectionNotify.h"
#include "Shared/Action/BaseAction.h"
#include "Shared/Action/Components/ActionComponent.h"
#include "Shared/Action/Actions/Combat/MeleeAttackAction.h"

void UHitDetectionNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !MeshComp->GetOwner())
		return;

	UActionComponent* ActionComp = MeshComp->GetOwner()->FindComponentByClass<UActionComponent>();
	if (!ActionComp)
		return;

	// 활성 MeleeAttackAction 찾기
	for (UBaseAction* Action : ActionComp->GetActiveActions())
	{
		if (UMeleeAttackAction* MeleeAction = Cast<UMeleeAttackAction>(Action))
		{
			if (bEnableHitDetection)
			{
				// MeleeAction->EnableHitDetection();
				if (bResetHitActors)
				{
					// MeleeAction->AlreadyHitActors.Empty();
				}
			}
			else
			{
				// MeleeAction->DisableHitDetection();
			}
			break; // 첫 번째만 처리
		}
	}
}
