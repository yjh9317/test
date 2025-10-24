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

	//~ Begin IInteractable Interface
	virtual void RemoveInteraction_Implementation() override;
	virtual void Interaction_Implementation(AActor* Interactor) override;
	virtual void EndInteraction_Implementation(AActor* Interactor) override;
	virtual void ClientStartInteraction_Implementation(AActor* Interactor) override;
	virtual void ClientEndInteraction_Implementation(AActor* Interactor) override;
	virtual bool CanBeInteractedWith_Implementation() override;
	//~ End IInteractable Interface

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UInventoryCoreComponent> InventoryCoreComponent;

protected:
	//~ Begin AActor Overrides
	virtual void BeginPlay() override;
	virtual void Destroyed() override;
	//~ End AActor Overrides

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetPhysicsSimulation();

private:
	void SetPhysicsSimulationAndCollision();
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Interactor", meta=(AllowPrivateAccess="true"))
	TObjectPtr<AActor> interactor;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="State|Interactor", meta=(AllowPrivateAccess="true"))
	TObjectPtr<AActor> localInteractor;
};
