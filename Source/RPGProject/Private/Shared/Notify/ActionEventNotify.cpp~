// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/Notify/ActionEventNotify.h"
#include "Shared/Action/BaseAction.h"
#include "Shared/Action/Components/ActionComponent.h"
#include "Shared/Action/Actions/Combat/MeleeAttackAction.h"

void UActionEventNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp || !MeshComp->GetOwner())
		return;

	UActionComponent* ActionComp = MeshComp->GetOwner()->FindComponentByClass<UActionComponent>();
	if (!ActionComp)
		return;
	
	IActionNotifyHandler* ActionNotifyHandler = nullptr;
	for (UBaseAction* Action : ActionComp->GetActiveActions())
	{
		if(Action->GetActionTag() == EventActionTag && Action->Implements<UActionNotifyHandler>())
		{
			ActionNotifyHandler = Cast<IActionNotifyHandler>(Action);
		}
	}

	if (ActionNotifyHandler)
	{
		switch (EventType)
		{
		case EActionEvent::ActionComplete:
			ActionNotifyHandler->HandleActionEvent(EActionEvent::ActionComplete);
			break;
		}
	}
}


