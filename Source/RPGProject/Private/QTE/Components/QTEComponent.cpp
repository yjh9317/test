// Fill out your copyright notice in the Description page of Project Settings.


#include "QTE/Components/QTEComponent.h"
#include "Blueprint/UserWidget.h"
#include "QTE/UI/QTEWidget.h"

UQTEComponent::UQTEComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
}

void UQTEComponent::BeginPlay()
{
	QTEWidget = CreateWidget<UQTEWidget>(GetWorld(),QTEWidgetClass);
	if(QTEWidget)
	{
		QTEWidget->AddToViewport();
	}
}

void UQTEComponent::BeginDestroy()
{
	Super::BeginDestroy();
}

void UQTEComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
