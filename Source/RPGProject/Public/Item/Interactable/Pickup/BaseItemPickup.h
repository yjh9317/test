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
	virtual void RemoveInteraction() override;
	virtual void ExecuteInteraction(AActor* Interactor) override;
	virtual void EndInteraction(AActor* Interactor) override;
	virtual void ClientBeginInteraction(AActor* Interactor) override;
	virtual void ClientEndInteraction(AActor* Interactor) override;
	virtual bool CanBeInteractedWith() override;
	//~ End IInteractable Interface

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interaction|Inventory")
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

	/**
	 * 서버 측 Interactor 참조
	 * - 서버에서 상호작용 중인 플레이어 추적
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interaction|State", 
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<AActor> ServerInteractor;

	/**
	 * 클라이언트 측 Interactor 참조
	 * - 로컬 클라이언트에서 상호작용 중인 플레이어 추적
	 * - UI 표시/숨김 등 클라이언트 전용 작업에 사용
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Interaction|State", 
		meta=(AllowPrivateAccess="true"))
	TObjectPtr<AActor> LocalInteractor;
};
