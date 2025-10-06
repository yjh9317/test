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
	virtual void BeginPlay() override;
	//virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	virtual void Initialize_Implementation() override;
	virtual void ClientPreInteraction_Implementation(AActor* Interactor) override;
	virtual void Interaction_Implementation(AActor* Interactor) override;
	virtual void ClientStartInteraction_Implementation(AActor* Interactor) override;
	virtual void EndInteraction_Implementation(AActor* Interactor) override;
	virtual void ClientEndInteraction_Implementation(AActor* Interactor) override;
	virtual void RemoveInteraction_Implementation() override{};
	

	UPROPERTY(BlueprintReadOnly,Category="Default")
	TObjectPtr<USceneComponent> Scene;
	UPROPERTY(BlueprintReadOnly,Category="Default")
	TObjectPtr<UBoxComponent> InteractableArea;
	UPROPERTY(BlueprintReadOnly,Category="Default")
	TObjectPtr<UWidgetComponent> InteractableWidget;
	UPROPERTY(BlueprintReadOnly,Category="Default")
	TObjectPtr<UInteractableComponent> InteractableComponent;
	UPROPERTY(BlueprintReadOnly,Category="Default")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;
	UPROPERTY(BlueprintReadOnly,Category="Default")
	TObjectPtr<UStaticMeshComponent> StaticMesh;
};
