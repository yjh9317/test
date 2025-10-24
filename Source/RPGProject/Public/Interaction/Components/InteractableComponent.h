// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/Data/InteractableDefinitions.h"
#include "Components/ActorComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/WidgetComponent.h"
#include "Interface/InteractableInterface.h"
#include "InteractableComponent.generated.h"


class UInteractionPromptWidget;
class UInputAction;
class ATargetPoint;
class UWidgetComponent;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateNewValue,float,NewValue);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UInteractableComponent : public UActorComponent,public IInteractableInterface
{
	GENERATED_BODY()

public:
	UInteractableComponent(const FObjectInitializer& ObjectInitializer);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	FOnUpdateNewValue OnUpdateHoldingValue;
	FOnUpdateNewValue OnUpdateMashingValue;
	
public:
	virtual void BeginPlay() override;
	// On Interaction
	UFUNCTION(BlueprintCallable)
	void AssociatedActorInteraction(AActor* Interactor);
	UFUNCTION(BlueprintCallable)
	void CheckForInteractionWithAssociate(AActor* Interactor);
	UFUNCTION()
	void OnInteraction(AActor* Interactor);
	UFUNCTION()
	void ClientInteraction(AActor* Interactor);

	UFUNCTION(NetMulticast,Reliable)
	void OnRemoveInteraction();
	
	UFUNCTION()
	void AssociatedActorEndInteraction();

	UFUNCTION()
	void ClientRemoveInteraction();
	
	// Before Interaction
	void ToggleHighlight(bool Highlight,AActor* Interactor);
	void SetupInteractableReferences(UBoxComponent* Area,UWidgetComponent* Widget,TSet<UPrimitiveComponent*> HighlightableObjects);
	
	void DurationPress();
	void IsKeyDown();
	void MultiplePress();
	void OnPreInteraction(AActor* Interactor);
	
	// After
	void OnEndInteraction(AActor* Interactor);
	void Reinitialize();
	void ReInitializeAssociatedActors();
	void OnClientEndInteraction(AActor* Interactor);
	
	// Interaction
	void ToggleIsInteractable(bool Condition);
	void ToggleInteractionWidget(bool Condition);
	void RemoveInteractionByResponse();
	void ChangeInteractableValue(bool Increment);
	void ToggleCanBeReInitialized(bool Condition);
	void SetWidgetLocalOwner(APlayerController* OwningPlayer);
	UFUNCTION(Server,Reliable)
	void Server_SetWidgetLocalOwner(APlayerController* OwningPlayer);
	
	EOperationStatus HoldingInput();
	EOperationStatus MashingInput(int32 MashingCount);
	void ResetMashingProgress();
	
	bool IsTargetInteractableValue();
	bool GetPressedKeyByAction(UInputAction* Action,FKey& OutKey);
public:
	// Movabale
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,Category="Movable")
	float TravelDuration;

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly,Category= "Movable|Timeline")
	float TimelinePosition;

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly,Category= "Movable|Timeline")
	TEnumAsByte<ETimelineDirection::Type> TimelineDirection;

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly,Category= "Interactable")
	UInputAction* InteractionAction;
	// Interaction
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly,Category= "Interactable")
	bool DestroyAfterPickup = false;
	UPROPERTY()
	TArray<UPrimitiveComponent*> ObjectsToHighlight;
	UPROPERTY()
	TMap<AActor*,int32> AssociatedInteractableActors;
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly,Category= "Interactable")
	float MaxKeyTimeDown = 1.f;

	FTimerHandle MashingTimer;
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly,Category= "Interactable")
	float MashingKeyRetriggerableTime = 0.2f;
	float CurrentMashingValue = 0.f;
		
	
	FTimerHandle KeyDownTimer;
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly,Category= "Interactable")
	EInteractionInputType InputType;
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly,Category= "Interactable")
	EInteractionResponse InteractionResponse;
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly,Category= "Interactable")
	int32 InteractableValue;
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly,Category= "Interactable")
	int32 InteractableTargetValue;
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly,Category= "Interactable")
	int32 InteractableLimitValue;
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly,Category= "Interactable")
	bool CheckForAssociatedActors;

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly,Category= "Interactable")
	bool CanBeReInitialized;
	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly,Category= "Interactable")
	bool RemoveAssociatedInteractableOnComplete;

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly,Category= "Interactable")
	bool bIsInteractable = true;

	UPROPERTY(EditDefaultsOnly , BlueprintReadOnly,Category= "Interactable")
	float DurationInputDelay = 0.05f;

	UPROPERTY()
	FKey PressedInteractionKey;
	UPROPERTY(EditDefaultsOnly)
	bool ShowLootBar;

	bool AlreadyInteracted;
	FText DefaultInteractionText;

	
	UPROPERTY(BlueprintReadOnly,Category="Reference")
	TObjectPtr<UInteractionPromptWidget> InteractionWidgetRef;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Class")
	TSubclassOf<UInteractionPromptWidget> InteractionWidgetClass;

	// Component
	UPROPERTY(BlueprintReadOnly,Category="Component")
	TObjectPtr<UBoxComponent> InteractableArea;
	UPROPERTY(BlueprintReadOnly,Category="Widget")
	TObjectPtr<UWidgetComponent> InteractionWidget;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Default")
	TObjectPtr<AActor> CurrentInteractor;
};
