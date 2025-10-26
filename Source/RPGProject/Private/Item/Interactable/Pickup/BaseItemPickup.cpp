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

	InteractableArea->SetCollisionObjectType(ECO_Interactable);
}

void ABaseItemPickup::Destroyed()
{
	Super::Destroyed();
	
	if (!InteractableComponent)
	{
		return;
	}

	InteractableComponent->SetInteractionWidgetVisible(false);

	if (InteractableComponent->bShowLootBar && LocalInteractor)
	{
		IComponentManager* ComponentManager = Cast<IComponentManager>(LocalInteractor);
		if (ComponentManager)
		{
			UInventoryComponent* LocalInventoryComp = ComponentManager->GetInventoryComponent();
			LocalInventoryComp->CloseLootBarWidget();
		}
	}

}

void ABaseItemPickup::RemoveInteraction()
{
	if (!InteractableComponent)
	{
		return;
	}

	InteractableComponent->SetInteractionWidgetVisible(false);

	if (InteractableComponent->bShowLootBar && LocalInteractor)
	{
		if (IComponentManager* ComponentManager = Cast<IComponentManager>(LocalInteractor))
		{
			if (UInventoryComponent* LocalInventoryComp = ComponentManager->GetInventoryComponent())
			{
				LocalInventoryComp->CloseLootBarWidget();
			}
		}
	}
}

void ABaseItemPickup::ExecuteInteraction(AActor* Interactor)
{
	if (Interactor)
	{
		ServerInteractor = Interactor;
	}
}


void ABaseItemPickup::EndInteraction(AActor* Interactor)
{
	if (Interactor)
	{
		ServerInteractor = Interactor;
	}
}

void ABaseItemPickup::ClientBeginInteraction(AActor* Interactor)
{
	if (Interactor)
	{
		LocalInteractor = Interactor;
	}

	IComponentManager* ComponentManager = Cast<IComponentManager>(LocalInteractor);
	if (!ComponentManager)
	{
		return;
	}

	UInventoryComponent* LocalInventoryComp = ComponentManager->GetInventoryComponent();
	if (!LocalInventoryComp)
	{
		return;
	}

	// Loot Bar 모드: UI 창 열기
	if (InteractableComponent && InteractableComponent->bShowLootBar)
	{
		// 상호작용 프롬프트 위젯 숨기기 (Loot UI로 대체)
		InteractableComponent->SetInteractionWidgetVisible(false);
		// Loot Bar UI 열기
		LocalInventoryComp->OpenLootBarWidget();
	}
	// 즉시 픽업 모드: 모든 아이템 자동 수집
	else
	{
		// 서버에 모든 아이템 수집 요청
		LocalInventoryComp->Server_TakeAllItems(InventoryCoreComponent, LocalInteractor);
	}
}


void ABaseItemPickup::ClientEndInteraction(AActor* Interactor)
{
	// 클라이언트 측 Interactor 저장
	if (Interactor)
	{
		LocalInteractor = Interactor;
	}

	// Loot Bar가 열려있으면 닫기
	if (InteractableComponent && InteractableComponent->bShowLootBar && LocalInteractor)
	{
		if (IComponentManager* ComponentManager = Cast<IComponentManager>(LocalInteractor))
		{
			if (UInventoryComponent* LocalInventoryComp = ComponentManager->GetInventoryComponent())
			{
				LocalInventoryComp->CloseLootBarWidget();
			}
		}
	}
}

bool ABaseItemPickup::CanBeInteractedWith()
{
	if (InteractableComponent)
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
