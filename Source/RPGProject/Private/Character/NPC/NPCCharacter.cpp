// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/NPC/NPCCharacter.h"
#include "Quest/Component/QuestComponent.h"
#include "Interaction/Components/InteractableComponent.h"
#include "Inventory/Components/InventoryComponent.h"

ANPCCharacter::ANPCCharacter(const FObjectInitializer& Initializer)
	: Super(Initializer)
{
	QuestComponent = CreateDefaultSubobject<UQuestComponent>(TEXT("QuestComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("VendorComponent"));
	InteractableComponent = CreateDefaultSubobject<UInteractableComponent>(TEXT("InteractableComponent"));
	
	QuestComponent->SetComponentTickEnabled(false);
	QuestComponent->SetActive(false);
    
	InventoryComponent->SetComponentTickEnabled(false);
	InventoryComponent->SetActive(false);
}

void ANPCCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	ActivateComponentsByType();
}

void ANPCCharacter::ActivateComponentsByType()
{
	QuestComponent->SetActive(false);
	InventoryComponent->SetActive(false);
    
	switch(NPCData.NPCType)
	{
	case ENPCType::Vendor:
		InventoryComponent->SetActive(true);
		InventoryComponent->SetComponentTickEnabled(true);
		break;
            
	case ENPCType::QuestGiver:
		QuestComponent->SetActive(true);
		QuestComponent->SetComponentTickEnabled(true);
		break;
         
	default:
		break;
	}
}
