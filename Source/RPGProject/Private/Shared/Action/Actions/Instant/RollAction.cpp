// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/Action/Actions/Instant/RollAction.h"

#include "GameFramework/Character.h"
#include "Player/Character/MainCharacter.h"

URollAction::URollAction(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
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
	BlockedTags.AddTag(FGameplayTag::RequestGameplayTag("Character.State.Stunned"));
}

void URollAction::Initialize(AActor* InOwner)
{
	Super::Initialize(InOwner);
	
	if (IComponentManager* ComponentManager = Cast<IComponentManager>(InOwner))
	{
		CachedMotionWarpingComponent = ComponentManager->GetMotionWarpingComponent();
	}
    
	if (!CachedMotionWarpingComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("RollAction: MotionWarpingComponent not found on %s"), *InOwner->GetName());
	}
}

bool URollAction::CanExecute() const
{
	if(!Super::CanExecute() || !OwnerCharacter || !CachedMotionWarpingComponent)
	{
		return false;
	}
	return true;
}

void URollAction::OnExecute()
{
	Super::OnExecute();
}

bool URollAction::CalculateWarpLocation() const
{
	FVector CachedRollingDirection = OwnerCharacter->GetLastMovementInputVector().GetSafeNormal();
	
	return true;
}

void URollAction::InvalidateCache()
{
}

bool URollAction::IsCacheValid() const
{
	return true;
}
