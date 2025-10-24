// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/Data/DataAsset_InputConfig.h"
#include "InputAction.h"
#include "Header/DebugHeader.h"

UInputAction* UDataAsset_InputConfig::FindNativeInputActionByTag(const FGameplayTag& InInputTag) const
{
	for (const FCustomInputActionConfig& InputActionConfig : NativeInputActions)
	{
		if (InputActionConfig.InputTag == InInputTag && InputActionConfig.InputAction)
		{
			return InputActionConfig.InputAction;
		}
	}
	return nullptr;
}
