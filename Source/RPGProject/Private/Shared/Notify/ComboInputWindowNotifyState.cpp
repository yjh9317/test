// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/Notify/ComboInputWindowNotifyState.h"

#include "Header/DebugHeader.h"
#include "Interface/ComponentManager.h"
#include "Player/Components/InputBufferComponent.h"
#include "Shared/Action/Actions/Combat/MeleeAttackAction.h"
#include "Shared/Action/Components/ActionComponent.h"

void UComboInputWindowNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	APawn* PawnOwner = Cast<APawn>(Owner);
	if (!PawnOwner) return;
	AController* Controller = PawnOwner->GetController();
	if (!Controller) return;
	
	IComponentManager* ComponentManager = Cast<IComponentManager>(Controller);
	CachedInputBuffer = ComponentManager->GetInputBufferComponent();
	if(!ComponentManager) return;
	ComponentManager = Cast<IComponentManager>(PawnOwner);
	const UActionComponent* ActionComp = ComponentManager->GetActionComponent();
	
	if (ActionComp)
	{
		for (UBaseAction* Action : ActionComp->GetActiveActions())
		{
			if (UMeleeAttackAction* MeleeAction = Cast<UMeleeAttackAction>(Action))
			{
				CachedMeleeAction = MeleeAction;
			}
		}
	}

}

void UComboInputWindowNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime)
{
	if(CachedMeleeAction.IsValid() && CachedInputBuffer.IsValid())
	{
		if(CachedMeleeAction->IsComboWindowOpen())
		{
			return;
		}
		if(CachedInputBuffer->HasRecentInput(AcceptedInputTag,0.2f))
		{
			CachedMeleeAction->SetComboWindowOpen(true);
		}
	}
}

void UComboInputWindowNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if(CachedMeleeAction.IsValid() && CachedMeleeAction->IsComboWindowOpen())
	{
		CachedMeleeAction->TryProcessComboInput();
	}	
}

