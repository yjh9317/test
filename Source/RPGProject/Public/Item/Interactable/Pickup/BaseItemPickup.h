// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Interactable/BaseInteractable.h"
#include "BaseItemPickup.generated.h"

class UInventoryCoreComponent;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API ABaseItemPickup : public ABaseInteractable
{
	GENERATED_BODY()
	
public:
	ABaseItemPickup();

	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	
	virtual void RemoveInteraction_Implementation() override;
	virtual void Interaction_Implementation(AActor* Interactor) override;
	virtual void EndInteraction_Implementation(AActor* Interactor) override;
	virtual void ClientStartInteraction_Implementation(AActor* Interactor) override;
	virtual void ClientEndInteraction_Implementation(AActor* Interactor) override;
	virtual bool CanBeInteractedWith_Implementation() override;

	UFUNCTION(NetMulticast,Reliable)
	void MulticastSetPhysicsSimulation();
	
	void SetPhysicsSimulationAndCollision();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInventoryCoreComponent> InventoryCoreComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Actor | Interactor")
	AActor* interactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Actor | Interactor")
	AActor* localInteractor;
};
