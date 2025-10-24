// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Shared/Action/Data/ActionData.h"
#include "UObject/Interface.h"
#include "ActionNotifyHandler.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UActionNotifyHandler : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGPROJECT_API IActionNotifyHandler
{
	GENERATED_BODY()

public:
	virtual void HandleActionEvent(EActionEvent EventType, float Value = 1.0f) = 0;
	virtual bool CanHandleActionEvent(EActionEvent EventType) const = 0;
};
