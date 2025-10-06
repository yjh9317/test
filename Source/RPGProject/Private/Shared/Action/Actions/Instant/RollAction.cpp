// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/Action/Actions/Instant/RollAction.h"

URollAction::URollAction(const FObjectInitializer& ObjectInitializer)
{
	ActionName = FText::FromString(TEXT("Roll"));
	bCanExecuteWhileMoving = true;
	bCanExecuteInAir = false;
	bInterruptible = false;
    
	FActionCost StaminaCost;
	StaminaCost.CostAttributeTag = FGameplayTag::RequestGameplayTag("Attribute.Stamina");
	StaminaCost.CostValue = 0.f;
	ResourceCosts.Add(StaminaCost);
    
	// GrantedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Invulnerable"));
	// BlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Stunned"));
}

bool URollAction::CanExecute() const
{
	return Super::CanExecute();
}

void URollAction::OnExecute_Implementation()
{
	Super::OnExecute_Implementation();
}

void URollAction::ExecuteInstant()
{
	Super::ExecuteInstant();
}
