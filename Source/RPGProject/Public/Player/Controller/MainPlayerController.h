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
private:
	// --------------------
	// 입력 처리
	// --------------------
	void OpenInventoryWidget();
	void OpenEquipmentWidget();
	void InputInteraction();

	// --------------------
	// 참조
	// --------------------
	UPROPERTY(Transient)
	AMainHUD* MainHUD;
	
	UPROPERTY(Transient)
	AMainCharacter* MainCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	UDataAsset_InputConfig* InputConfigDataAsset;

	// --------------------
	// 컴포넌트
	// --------------------
	UPROPERTY()
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY()
	TObjectPtr<UInteractionComponent> InteractionComp;

	UPROPERTY()
	TObjectPtr<UEquipmentComponent> EquipmentComponent;

	UPROPERTY()
	TObjectPtr<UPlayerWidgetComponent> PlayerWidgetComponent;
	
	UPROPERTY()
	TObjectPtr<UInputBufferComponent> InputBufferComponent;
	
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	//TObjectPtr<UTargetSystemComponent> TargetSystemComponent;

	// --------------------
	// 상호작용 상태
	// --------------------
	UPROPERTY(Transient)
	TObjectPtr<UInteractableComponent> CurrentInteractable;

	UPROPERTY(Transient)
	TObjectPtr<APlayerCaptureActor> PlayerCaptureActor;
	
public:
	// --------------------
	// Getter
	// --------------------
	FORCEINLINE AMainHUD* GetMainHUD() const { return MainHUD; }

	// --------------------
	// IComponentManager 인터페이스 구현
	// --------------------
	virtual UInventoryComponent* GetInventoryComponent() override;
	virtual UEquipmentComponent* GetEquipmentComponent() override;
	virtual UInteractionComponent* GetInteractionComponent() override;
	virtual UActionComponent* GetActionComponent() override;
	virtual UInputBufferComponent* GetInputBufferComponent() override;

	// --------------------
	// IWidgetManager 인터페이스 구현
	// --------------------
	virtual void SetActiveTab(EWidgetType NewType) override;
	virtual void SetActiveWidget(EWidgetType NewType) override;
	virtual void SwitchTabTo(EWidgetType NewType) override;
	virtual void SwitchWidgetTo(EWidgetType NewType) override;
	virtual void CloseActivePopup() override;
	virtual void CloseActiveWidget() override;
	virtual void OpenNewWidget(EWidgetType NewType) override;
	virtual EWidgetType GetActiveTab() const override;
	virtual EWidgetType GetActiveWidget() const override;
	virtual EWidgetPopup GetActivePopup() const override;
	virtual void StartPlayerCapture() override;
	virtual void StopPlayerCapture() override;
	
	// --------------------
	// IObjectInteraction 인터페이스 구현
	// --------------------
	virtual void StartInteractionWithObject(UInteractableComponent* InteractableComponent) override;
	virtual void EndInteractionWithObject(UInteractableComponent* InteractableComponent) override;
	virtual void RemoveInteractionWithObject(UInteractableComponent* InteractableComponent) override;
	virtual void InitializeInteractionWithObject(UInteractableComponent* InteractableComponent) override;
	virtual AActor* GetCurrentInteractableObject() override;


private:
	// --------------------
	// 네트워크 RPC - 상호작용 시작
	// --------------------
	UFUNCTION(Server, Reliable)
	void ServerStartInteractionWithObject(UInteractableComponent* InteractableComponent);
	
	UFUNCTION(Client, Reliable)
	void ClientStartInteractionWithObject(UInteractableComponent* InteractableComponent);
	
	void StartInteraction(UInteractableComponent* InteractableComp);
	
	// --------------------
	// 네트워크 RPC - 상호작용 종료
	// --------------------
	UFUNCTION(Server, Reliable)
	void ServerEndInteractionWithObject(UInteractableComponent* InteractableComponent);
	
	UFUNCTION(Client, Reliable)
	void ClientEndInteractionWithObject(UInteractableComponent* InteractableComponent);
	
	void EndInteraction(UInteractableComponent* InteractableComponent);
	
	// --------------------
	// 네트워크 RPC - 상호작용 초기화
	// --------------------
	UFUNCTION(Client, Reliable)
	void ClientInitializeInteractionWithObject(UInteractableComponent* InteractableComponent);
	
	void InitializeInteraction(UInteractableComponent* InteractableComponent);
	
	// --------------------
	// 네트워크 RPC - 상호작용 제거
	// --------------------
	UFUNCTION(Server, Reliable)
	void ServerRemoveInteractionWithObject(UInteractableComponent* InteractableComponent);
	
	UFUNCTION(Client, Reliable)
	void ClientRemoveInteractionWithObject(UInteractableComponent* InteractableComponent);
	
	void RemoveInteraction(UInteractableComponent* InteractableComponent);
};

