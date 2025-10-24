// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Interactable/BaseInteractable.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"
#include "Header/DebugHeader.h"
#include "Interaction/Components/InteractableComponent.h"

ABaseInteractable::ABaseInteractable()
{
	PrimaryActorTick.bCanEverTick = false;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Default"));
	SetRootComponent(Scene);
		
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
}

void ABaseInteractable::Initialize_Implementation()
{
	InteractableComponent->SetupInteractableReferences(InteractableArea, InteractableWidget,{SkeletalMesh,StaticMesh});
}

void ABaseInteractable::ClientPreInteraction_Implementation(AActor* Interactor)
{
	IInteractableInterface::ClientPreInteraction_Implementation(Interactor);
}

void ABaseInteractable::Interaction_Implementation(AActor* Interactor)
{
	IInteractableInterface::Interaction_Implementation(Interactor);
}

void ABaseInteractable::ClientStartInteraction_Implementation(AActor* Interactor)
{
	IInteractableInterface::ClientStartInteraction_Implementation(Interactor);
}

void ABaseInteractable::EndInteraction_Implementation(AActor* Interactor)
{
	IInteractableInterface::EndInteraction_Implementation(Interactor);
}

void ABaseInteractable::ClientEndInteraction_Implementation(AActor* Interactor)
{
	IInteractableInterface::ClientEndInteraction_Implementation(Interactor);
}

