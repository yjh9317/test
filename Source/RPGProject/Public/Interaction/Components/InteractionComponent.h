// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "InteractionComponent.generated.h"

class UInteractableComponent;
DECLARE_MULTICAST_DELEGATE_OneParam(FOnNewInteractableAssigned, UInteractableComponent* Comp);
DECLARE_MULTICAST_DELEGATE(FOnRemovedCurrentInteractable);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RPGPROJECT_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent(const FObjectInitializer& ObjectInitializer);

	FOnNewInteractableAssigned OnNewInteractableAssigned;
	FOnRemovedCurrentInteractable OnRemovedCurrentInteractable;

protected:
	UFUNCTION(Client,Reliable)
	void InitializeInteraction(APlayerController* PlayerController);

	UFUNCTION()
	void ClientUpdateInteraction();

	void UpdateInteraction();
	void RemoveInteractionFromCurrent();
	void AssignInteractionToLocal(UInteractableComponent* InteractableComponent);
	void UpdateCurrentInteractable();

	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,Category="Interaction")
	TObjectPtr<UInteractableComponent> CurrentInteractable;
	
	UPROPERTY(EditDefaultsOnly,Category="Interaction")
	float CameraTraceLength = 500.f;

	UPROPERTY(EditDefaultsOnly,Category="Interaction")
	float OwnerTraceLength = 250.f;

	UPROPERTY(EditDefaultsOnly, Category="Interaction")
	TEnumAsByte<EDrawDebugTrace::Type> DebugTrace = EDrawDebugTrace::ForOneFrame;

	UPROPERTY(EditDefaultsOnly, Category="Interaction", meta=(EditCondition="DebugTrace != EDrawDebugTrace::None"))
	FLinearColor TraceColor = FLinearColor::Red;

	UPROPERTY(EditDefaultsOnly, Category="Interaction", meta=(EditCondition="DebugTrace != EDrawDebugTrace::None"))
	FLinearColor TraceHitColor = FLinearColor::Green;

	UPROPERTY(EditDefaultsOnly, Category="Interaction")
	float DebugDrawTime = 0.1f;
	
	UPROPERTY(EditDefaultsOnly, Category="Interaction")
	float InteractionDistance = 110.f;

	UPROPERTY(EditDefaultsOnly,Category="Widget")
	TSubclassOf<UUserWidget> InteractionWidgetClass;
	
	UPROPERTY()
	TObjectPtr<APawn> ControlledPawnRef;
	UPROPERTY()
	TObjectPtr<AController> ControllerRef;

	UPROPERTY(EditDefaultsOnly,Category="Interaction")
	float InteractionDelay = 0.1f;
	
	FTimerHandle InteractionTimer;
	
private:
	bool PerformInteractionTrace(TArray<FHitResult>& OutHits, 
						   const FVector& StartLocation, 
						   const FVector& EndLocation,
						   const TArray<AActor*>& IgnoredActors,
						   float CapsuleRadius = 50.0f, 
						   float CapsuleHalfHeight = 100.0f);

	UInteractableComponent* FindInteractableViaForwardTrace();
	UInteractableComponent* FindInteractableViaTrace();
	bool IsValidInteractable(UInteractableComponent* Component) const;

	void DebugDrawInteractionStatus(bool bBothConditionsMet, 
	UInteractableComponent* CameraTraceResult, 
	UInteractableComponent* ForwardTraceResult) const;
};
