// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Interactable/Pickup/BaseItemPickup.h"

#include "Components/BoxComponent.h"
#include "Header/CollisionType.h"
#include "Interaction/Components/InteractableComponent.h"
#include "Interface/ComponentManager.h"
#include "Inventory/Components/InventoryComponent.h"
#include "Inventory/Components/InventoryCoreComponent.h"

ABaseItemPickup::ABaseItemPickup()
{
	InventoryCoreComponent = CreateDefaultSubobject<UInventoryCoreComponent>(TEXT("InventoryCoreComponent"));
	
}

void ABaseItemPickup::BeginPlay()
{
	Super::BeginPlay();
	MulticastSetPhysicsSimulation();

	StaticMesh->CanCharacterStepUpOn = ECB_No;
	InteractableArea->SetCollisionObjectType(ECO_Interactable);
}

void ABaseItemPickup::Destroyed()
{
	if(InteractableComponent)
	{
		InteractableComponent->ToggleInteractionWidget(false);

		if(InteractableComponent->ShowLootBar && localInteractor)
		{
			IComponentManager* ComponentManager = Cast<IComponentManager>(localInteractor);
			if(ComponentManager)
			{
				UInventoryComponent* LocalInventoryComp = ComponentManager->GetInventoryComponent();
				LocalInventoryComp->CloseLootBarWidget();
			}
		}
	}
}

void ABaseItemPickup::RemoveInteraction_Implementation()
{
	if(InteractableComponent)
	{
		InteractableComponent->ToggleInteractionWidget(false);

		if(InteractableComponent->ShowLootBar && localInteractor)
		{
			IComponentManager* ComponentManager = Cast<IComponentManager>(localInteractor);
			if(ComponentManager)
			{
				UInventoryComponent* LocalInventoryComp = ComponentManager->GetInventoryComponent();
				LocalInventoryComp->CloseLootBarWidget();
			}
		}
	}
}

void ABaseItemPickup::Interaction_Implementation(AActor* Interactor)
{
	if(Interactor)
	{
		interactor = Interactor;
	}
}

void ABaseItemPickup::EndInteraction_Implementation(AActor* Interactor)
{
	if(Interactor)
	{
		interactor = Interactor;
	}
}

void ABaseItemPickup::ClientStartInteraction_Implementation(AActor* Interactor)
{
	if(Interactor)
	{
		localInteractor = Interactor;
	}
	if(InteractableComponent->ShowLootBar && localInteractor)
	{
		InteractableComponent->ToggleInteractionWidget(false);
		IComponentManager* ComponentManager = Cast<IComponentManager>(localInteractor);
		if(ComponentManager)
		{
			if(UInventoryComponent* LocalInventoryComp = ComponentManager->GetInventoryComponent())
			{
				LocalInventoryComp->OpenLootBarWidget();
			}
		}
	}
	else
	{
		IComponentManager* ComponentManager = Cast<IComponentManager>(localInteractor);
		if(ComponentManager)
		{
			if(UInventoryComponent* LocalInventoryComp = ComponentManager->GetInventoryComponent())
			{
				LocalInventoryComp->Server_TakeAllItems(InventoryCoreComponent,localInteractor);
			}
		}
	}
}

void ABaseItemPickup::ClientEndInteraction_Implementation(AActor* Interactor)
{
	if(Interactor)
	{
		localInteractor = Interactor;
	}
	if(InteractableComponent->ShowLootBar && localInteractor)
	{
		IComponentManager* ComponentManager = Cast<IComponentManager>(localInteractor);
		if(ComponentManager)
		{
			if(UInventoryComponent* LocalInventoryComp = ComponentManager->GetInventoryComponent())
			{
				LocalInventoryComp->CloseLootBarWidget();
			}
		}
	}
}

bool ABaseItemPickup::CanBeInteractedWith_Implementation()
{
	if(InteractableComponent)
	{
		return InteractableComponent->bIsInteractable;
	}
	return false;
}

void ABaseItemPickup::MulticastSetPhysicsSimulation_Implementation()
{
	SetPhysicsSimulationAndCollision();
}

void ABaseItemPickup::SetPhysicsSimulationAndCollision()
{
	// StaticMesh는 Pickup 상태일 때의 Mesh
	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->SetEnableGravity(true);

	StaticMesh->SetCollisionResponseToChannel(ECC_Pawn,ECR_Ignore);
	StaticMesh->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Block);
	StaticMesh->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	// SkeletalMesh는 실제 캐릭터가 찰 때 사용할 Mesh
	SkeletalMesh->SetSimulatePhysics(false);
	SkeletalMesh->SetEnableGravity(false);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}	
