#pragma once
#include "Components/Status/HealthBarComponent.h"
#include "Components/ProgressBar.h"
#include "UI/GenericWidgets/HealthBar.h"


void UHealthBarComponent::SetHealthPercent(float Percent)
{
	if(nullptr == HealthBarWidget)
	{
		HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
	}
	
	if(HealthBarWidget && HealthBarWidget->ProgressBar_Main)
	{
		HealthBarWidget->ProgressBar_Main->SetPercent(Percent);
	}
}

void UHealthBarComponent::BeginPlay()
{
	Super::BeginPlay();
}
