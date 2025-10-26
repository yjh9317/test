// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Interactable/BaseInteractable.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Header/DebugHeader.h"
#include "Interaction/Components/InteractableComponent.h"

ABaseInteractable::ABaseInteractable()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Default"));
	SetRootComponent(SceneRoot);
		
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(GetRootComponent());
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(GetRootComponent());
	
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
	
	InteractableArea =CreateDefaultSubobject<UBoxComponent>(TEXT("InteractableArea"));
	InteractableArea->SetupAttachment(StaticMesh);
	
	InteractableWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractableWidget"));
	InteractableWidget->SetupAttachment(InteractableArea);
}

void ABaseInteractable::BeginPlay()
{
	Super::BeginPlay();

	InitializeInteractable();
}

void ABaseInteractable::InitializeInteractable()
{
	if (InteractableComponent)
	{
		InteractableComponent->SetupInteractableReferences(
			InteractableArea, 
			InteractableWidget, 
			{SkeletalMesh, StaticMesh}
		);
	}
}

void ABaseInteractable::ExecuteInteraction(AActor* NewInteractor)
{
	IInteractableInterface::ExecuteInteraction(NewInteractor);
}
