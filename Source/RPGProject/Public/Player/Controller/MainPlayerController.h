#pragma once

#include "CoreMinimal.h"
#include "Shared/Data/DataAsset_InputConfig.h"
#include "GameFramework/PlayerController.h"
#include "Interface/ComponentManager.h"
#include "Interface/ObjectInteraction.h"
#include "Interface/WidgetManager.h"
#include "Player/Components/PlayerWidgetComponent.h"
#include "MainPlayerController.generated.h"

class APlayerCaptureActor;
class UInputMappingContext;
class UInputAction;
class UAIPerceptionComponent;
class UPlayerWidgetComponent;
class UActionComponent;
class AMainCharacter;
class AWardRobe;


UCLASS()
class RPGPROJECT_API AMainPlayerController : public APlayerController, public IObjectInteraction, public IComponentManager, public IWidgetManager
{
	GENERATED_BODY()

public:
	AMainPlayerController();
	
	
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void AcknowledgePossession(APawn* P) override;
	virtual void PostInitializeComponents() override;
	virtual void OnPossess(APawn* InPawn) override;
	void OpenInventoryWidget();
	void OpenEquipmentWidget();

	void InputInteraction();
private:
	UPROPERTY()
	class AMainHUD* MainHUD;
	
	UPROPERTY(VisibleInstanceOnly,BlueprintReadOnly,meta=(AllowPrivateAccess="true"))
	AMainCharacter* MainCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CharacterData", meta = (AllowPrivateAccess = "true"))
	UDataAsset_InputConfig* InputConfigDataAsset;

	// --------------------
	// Component
	// --------------------
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInteractionComponent> InteractionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEquipmentComponent> EquipmentComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPlayerWidgetComponent> PlayerWidgetComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputBufferComponent> InputBufferComponent;
	
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UTargetSystemComponent> TargetSystemComponent;

	// --------------------
	// Variables
	// --------------------
	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<UInteractableComponent> CurrentInteractable;

	UPROPERTY(VisibleInstanceOnly)
	TObjectPtr<APlayerCaptureActor> PlayerCaptureActor;
public:
	FORCEINLINE AMainHUD* GetMainHUD() const {return MainHUD;}

#pragma region InterfaceFunction
	// Component Manger
	// virtual UInventoryComponent* GetInventoryComponent_Implementation() override;
	// virtual UEquipmentComponent* GetEquipmentComponent_Implementation() override;
	// virtual UInteractionComponent* GetInteractionComponent_Implementation() override;
	// virtual UActionComponent* GetActionComponent_Implementation() override;
	// virtual UInputBufferComponent* GetInputBufferComponent_Implementation() override;
	virtual UInventoryComponent* GetInventoryComponent() override;
	virtual UEquipmentComponent* GetEquipmentComponent() override;
	virtual UInteractionComponent* GetInteractionComponent() override;
	virtual UActionComponent* GetActionComponent() override;
	virtual UInputBufferComponent* GetInputBufferComponent() override;

	// WidgetManager
#pragma region WidgetManager
	virtual void SetActiveTab(EWidgetType NewType) override;
	virtual void SetActiveWidget(EWidgetType NewType) override;
	virtual void SwitchTabTo(EWidgetType NewType) override;
	virtual void SwitchWidgetTo(EWidgetType NewType) override;
	
	virtual void CloseActivePopup() override;
	virtual void CloseActiveWidget() override;
	virtual void OpenNewWidget(EWidgetType NewType) override;

	virtual EWidgetType GetActiveTab() const override { return PlayerWidgetComponent->GetActiveTab();}
	virtual EWidgetType GetActiveWidget() const override { return PlayerWidgetComponent->GetActiveWidget();}
	virtual EWidgetPopup GetActivePopup() const override { return PlayerWidgetComponent->GetActivePopupType();}

	virtual void StartPlayerCapture() override;;
	virtual void StopPlayerCapture() override;
#pragma endregion 
	// ObjectInteraction
	virtual void StartInteractionWithObject(UInteractableComponent* InteractableComponent) override;
	virtual void EndInteractionWithObject(UInteractableComponent* InteractableComponent) override;
	virtual void RemoveInteractionWithObject(UInteractableComponent* InteractableComponent) override;
	virtual void InitializeInteractionWithObject(UInteractableComponent* InteractableComponent) override;
	virtual AActor* GetCurrentInteractableObject() override;
#pragma endregion

private:
	// Internal
	UFUNCTION(Server,Reliable)
	void ServerStartInteractionWithObject(UInteractableComponent* InteractableComponent);
	UFUNCTION(Client,Reliable)
	void ClientStartInteractionWithObject(UInteractableComponent* InteractableComponent);
	void StartInteraction(UInteractableComponent* InteractableComp);
	
	UFUNCTION(Server,Reliable)
	void ServerEndInteractionWithObject(UInteractableComponent* InteractableComponent);
	UFUNCTION(Client,Reliable)
	void ClientEndInteractionWithObject(UInteractableComponent* InteractableComponent);
	void EndInteraction(UInteractableComponent* InteractableComponent);
	
	UFUNCTION(Client,Reliable)
	void ClientInitializeInteractionWithObject(UInteractableComponent* InteractableComponent);
	void InitializeInteraction(UInteractableComponent* InteractableComponent);
	
	UFUNCTION(Server,Reliable)
	void ServerRemoveInteractionWithObject(UInteractableComponent* InteractableComponent);
	UFUNCTION(Client,Reliable)
	void ClientRemoveInteractionWithObject(UInteractableComponent* InteractableComponent);
	
	void RemoveInteraction(UInteractableComponent* InteractableComponent);
};

