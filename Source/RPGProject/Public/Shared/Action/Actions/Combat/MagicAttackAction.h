// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Shared/Action/BaseAction.h"
#include "MagicAttackAction.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UMagicAttackAction : public UBaseAction
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, Category = "Magic")
	float CastTime = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Magic")
	bool bRequiresChanneling = false;

	UPROPERTY(EditDefaultsOnly, Category = "Magic")
	float ManaCost = 20.0f;

	// ASpellProjectile
	UPROPERTY(EditDefaultsOnly, Category = "Magic")
	TSubclassOf<class AActor> SpellClass;

protected:
	bool bIsCasting = false;
	FTimerHandle CastTimer;

	//virtual void OnActionStarted() override
	//{

		// 	if (UEnhancedAttackingState* AttackState = 
		// 		Cast<UEnhancedAttackingState>(StateComp->GetPrimaryState(EStateCategory::Combat)))
		// 	{
		// 		AttackState->InitializeAttack(FGameplayTag::RequestGameplayTag("Weapon.Magic.Staff"));
		// 	}
		// }

		// 마나 체크
		// if (!HasEnoughMana())
		// {
		// 	StopAction();
		// 	return;
		// }

		// 캐스팅 시작
		//StartCasting();
	//}

	void StartCasting()
	{
		bIsCasting = true;
        
		// 캐스팅 애니메이션
		// PlayCastAnimation();
        
		// 캐스팅 이펙트
		// SpawnCastingEffects();
        
		// 타이머 설정
		GetWorld()->GetTimerManager().SetTimer(
			CastTimer,
			this,
			&UMagicAttackAction::OnCastComplete,
			CastTime,
			false
		);
	}

	void OnCastComplete()
	{
		if (bRequiresChanneling)
		{
			// 채널링 시작
			// StartChanneling();
		}
		else
		{
			// 즉시 발사
			// CastSpell();
		}
	}
};
