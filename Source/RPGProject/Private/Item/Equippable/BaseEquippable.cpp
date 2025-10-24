// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Equippable/BaseEquippable.h"

#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Particles/ParticleSystemComponent.h"

ABaseEquippable::ABaseEquippable()
{
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ParticleSystemComponent"));
	
	if(HasAuthority())
	{
		APlayerController* Controller = Cast<APlayerController>(GetOwner());
		if(Controller)
		{
			ACharacter* Character = Cast<ACharacter>(Controller->GetPawn());
			if(Character)
			{
				ParentMesh = Character->GetMesh();
			}
		}
	}
}

void ABaseEquippable::AttachToNewSocket(FName NewSocket)
{
	if(ParentMesh)
	{
		FAttachmentTransformRules TransformRules{EAttachmentRule::SnapToTarget,EAttachmentRule::SnapToTarget,EAttachmentRule::SnapToTarget,false};
		AttachToComponent(ParentMesh,TransformRules,NewSocket);
	}
}

void ABaseEquippable::ToggleCombatMode(bool CombatMode)
{
	FAttachmentTransformRules TransformRules{EAttachmentRule::SnapToTarget,EAttachmentRule::SnapToTarget,EAttachmentRule::SnapToTarget,false};
	if(CombatMode)
	{
		if(CombatSocketNames.Find(CurrentSlot))
		{
			AttachToComponent(ParentMesh,TransformRules,CombatSocketNames[CurrentSlot]);
		}
	}
	else
	{
		if(NonCombatSocketNames.Find(CurrentSlot))
		{
			AttachToComponent(ParentMesh,TransformRules,NonCombatSocketNames[CurrentSlot]);
		}
	}
}

