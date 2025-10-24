// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/InteractableInterface.h"
#include "BaseInteractable.generated.h"

class UInteractableComponent;
class UWidgetComponent;
class UBoxComponent;

UCLASS()
class RPGPROJECT_API ABaseInteractable : public AActor, public IInteractableInterface
{
	GENERATED_BODY()
	
public: 
	ABaseInteractable();

	//~ Begin IInteractableInterface
	virtual void Initialize_Implementation() override;
	virtual void ClientPreInteraction_Implementation(AActor* Interactor) override;
	virtual void Interaction_Implementation(AActor* Interactor) override;
	virtual void ClientStartInteraction_Implementation(AActor* Interactor) override;
	virtual void EndInteraction_Implementation(AActor* Interactor) override;
	virtual void ClientEndInteraction_Implementation(AActor* Interactor) override;
	virtual void RemoveInteraction_Implementation() override {};
	//~ End IInteractableInterface

protected:
	//~ Begin AActor Overrides
	virtual void BeginPlay() override;
	//~ End AActor Overrides

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UBoxComponent> InteractableArea;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UWidgetComponent> InteractableWidget;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UInteractableComponent> InteractableComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Components")
	TObjectPtr<UStaticMeshComponent> StaticMesh;
private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USceneComponent> Scene;


};
