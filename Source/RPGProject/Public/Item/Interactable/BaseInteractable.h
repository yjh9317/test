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
	virtual void InitializeInteractable() override;
	virtual void ExecuteInteraction(AActor* Interactor) override;
	virtual void ClientPrepareInteraction() override {};
	virtual void ClientBeginInteraction(AActor* Interactor) override {};
	virtual void EndInteraction(AActor* Interactor) override {};
	virtual void ClientEndInteraction(AActor* ClientEndInteraction) override {};
	virtual void RemoveInteraction() override {};
	//~ End IInteractableInterface

	//~==============================================================================================
	// Components
	//~==============================================================================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction|Components")
	TObjectPtr<UBoxComponent> InteractableArea;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Interaction|Components")
	TObjectPtr<UWidgetComponent> InteractableWidget;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category="Interaction|Components")
	TObjectPtr<UInteractableComponent> InteractableComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction|Mesh")
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Interaction|Mesh")
	TObjectPtr<UStaticMeshComponent> StaticMesh;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Interaction|Components", 
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<USceneComponent> SceneRoot;

};
