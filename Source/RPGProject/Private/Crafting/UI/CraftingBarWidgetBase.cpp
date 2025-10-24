// Fill out your copyright notice in the Description page of Project Settings.


#include "Crafting/UI/CraftingBarWidgetBase.h"
#include "Interface/ComponentManager.h"


UCraftingBarWidgetBase::UCraftingBarWidgetBase(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	
}

void UCraftingBarWidgetBase::NativeConstruct()
{
	Super::NativeConstruct();

	IComponentManager* ComponentManager = Cast<IComponentManager>(GetOwningPlayer());
	if(ComponentManager)
	{
		//StationsManager = ComponentManager->
	}
}

void UCraftingBarWidgetBase::RefreshWidgetData()
{
}

void UCraftingBarWidgetBase::OnItemSelected()
{
}

void UCraftingBarWidgetBase::PlayCraftingAnimation()
{
}

void UCraftingBarWidgetBase::AttemptCrafting()
{
}

void UCraftingBarWidgetBase::CraftingProcessStarted()
{
}

void UCraftingBarWidgetBase::StopCrafting()
{
}

void UCraftingBarWidgetBase::CraftingProcessStopped()
{
}

void UCraftingBarWidgetBase::OnSliderValueChanged(float Value)
{
	
}

void UCraftingBarWidgetBase::OnEditableTextChanged(const FText& Text)
{
}
