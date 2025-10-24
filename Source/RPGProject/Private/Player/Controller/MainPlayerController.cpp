
#include "Player/Controller/MainPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "RPGGameplayTags.h"
#include "Blueprint/UserWidget.h"
#include "Shared/Components/CustomInputComponent.h"
#include "Shared/Data/DataAsset_InputConfig.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/PlayerState.h"
#include "Header/DebugHeader.h"
#include "Interaction/Components/InteractableComponent.h"
#include "Player/Components/PlayerWidgetComponent.h"
#include "Interaction/Components/InteractionComponent.h"
#include "Inventory/Components/EquipmentComponent.h"
#include "Inventory/Components/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Capture/PlayerCaptureActor.h"
#include "Player/Character/MainCharacter.h"
#include "Player/Components/InputBufferComponent.h"
#include "UI/HUD/MainHUD.h"

AMainPlayerController::AMainPlayerController()
{
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComp"));
	InteractionComp = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	PlayerWidgetComponent = CreateDefaultSubobject<UPlayerWidgetComponent>(TEXT("PlayerWidgetComponent"));
	InputBufferComponent = CreateDefaultSubobject<UInputBufferComponent>(TEXT("InputBufferComponent"));
}


void AMainPlayerController::BeginPlay()
{
	Super::BeginPlay();
	MainCharacter = Cast<AMainCharacter>(GetOwner());
	MainHUD = Cast<AMainHUD>(GetHUD());

	InteractionComp->OnNewInteractableAssigned.AddLambda([this](UInteractableComponent* Comp)
	{
		if (Comp)
		{
			CurrentInteractable = Comp;
		}
	});
	InteractionComp->OnRemovedCurrentInteractable.AddLambda([this]()
	{
		CurrentInteractable = nullptr;	
	});
	
	if(PlayerWidgetComponent)
	{
		PlayerWidgetComponent->InitializeComponentData(this,MainCharacter);
	}
}

void AMainPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	checkf(InputConfigDataAsset,TEXT("Forgot to assign a valid data asset as input config"));

	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	check(Subsystem);

	Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext,0);

	UCustomInputComponent* CustomInputComponent = CastChecked<UCustomInputComponent>(InputComponent);
	if(CustomInputComponent)
	{
		CustomInputComponent->BindNativeInputAction(InputConfigDataAsset,RPGGameplayTags::Input_Inventory,ETriggerEvent::Completed,this,&ThisClass::OpenInventoryWidget);
		CustomInputComponent->BindNativeInputAction(InputConfigDataAsset,RPGGameplayTags::Input_Interact,ETriggerEvent::Started,this,&ThisClass::InputInteraction);
	}
}

void AMainPlayerController::AcknowledgePossession(APawn* P)
{
	Super::AcknowledgePossession(P);
}

void AMainPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}


void AMainPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	MainCharacter = Cast<AMainCharacter>(InPawn);
	
	if (InteractionComp)
	{
		InteractionComp->InitializeInteraction_Implementation(this);
	}

	if(InventoryComponent)
	{
		InventoryComponent->InitializeInventory(this);
	}
	
	PlayerCaptureActor = Cast<APlayerCaptureActor>(UGameplayStatics::GetActorOfClass(GetWorld(),APlayerCaptureActor::StaticClass()));
	if(PlayerCaptureActor)
	{
		PlayerCaptureActor;
	}
}

#pragma region WidgetManager

void AMainPlayerController::OpenInventoryWidget()
{
	// InventoryComponent->InputInventoryFromSwitcher();
	InventoryComponent->InputInventory();
}

void AMainPlayerController::OpenEquipmentWidget()
{

}

void AMainPlayerController::InputInteraction()
{
	if(CurrentInteractable)
	{
		InitializeInteractionWithObject(CurrentInteractable);
	}
}



void AMainPlayerController::SetActiveTab(EWidgetType NewType)
{
	if(PlayerWidgetComponent)
	{
		PlayerWidgetComponent->SetNewActiveTab(NewType);
	}
}

void AMainPlayerController::SetActiveWidget(EWidgetType NewType)
{
	if(PlayerWidgetComponent)
	{
		PlayerWidgetComponent->SetNewActiveWidget(NewType);
	}
}

void AMainPlayerController::SwitchTabTo(EWidgetType NewType)
{
	if(GetActiveTab() != NewType)
	{
		SetActiveTab(NewType);
	}
}

void AMainPlayerController::SwitchWidgetTo(EWidgetType NewType)
{
	if(GetActiveWidget() != NewType)
	{
		CloseActiveWidget();
		OpenNewWidget(NewType);
	}
}

void AMainPlayerController::CloseActivePopup()
{
	IWidgetManager::CloseActivePopup();
}

void AMainPlayerController::CloseActiveWidget()
{
	EWidgetType CurrentActiveWidgetType = GetActiveWidget();

	switch (CurrentActiveWidgetType)
	{
	case EWidgetType::INVENTORY:
		InventoryComponent->CloseInventoryWidget();
		break;
	case EWidgetType::CRAFTING:
		// 
		break;
	case EWidgetType::VENDOR:
		InventoryComponent->CloseVendorWidget();
		break;
	case EWidgetType::STORAGE:
		InventoryComponent->CloseStorageWidget();
		break;
	case EWidgetType::LOADGAME:
		break;
	}
}

void AMainPlayerController::OpenNewWidget(EWidgetType NewType)
{
	switch (NewType)
	{
	case EWidgetType::INVENTORY:
		InventoryComponent->OpenInventoryWidget();
		break;
	case EWidgetType::CRAFTING:
		// 
			break;
	case EWidgetType::VENDOR:
		InventoryComponent->OpenVendorWidget();
		break;
	case EWidgetType::STORAGE:
		InventoryComponent->OpenStorageWidget();
		break;
	case EWidgetType::LOADGAME:
		break;
	}
}
void AMainPlayerController::StartPlayerCapture()
{
	if(PlayerCaptureActor)
	{
		PlayerCaptureActor->StartCapture();
	}
}

void AMainPlayerController::StopPlayerCapture()
{
	if(PlayerCaptureActor)
	{
		PlayerCaptureActor->EndCapture();
	}
}

#pragma endregion 

#pragma region Component
UInventoryComponent* AMainPlayerController::GetInventoryComponent()  
{
	return InventoryComponent;
}

UEquipmentComponent* AMainPlayerController::GetEquipmentComponent()  
{
	return EquipmentComponent;
}

UInteractionComponent* AMainPlayerController::GetInteractionComponent()  
{
	return InteractionComp;
}

UActionComponent* AMainPlayerController::GetActionComponent() 
{
	if (MainCharacter)
	{
		return MainCharacter->ActionComponent;
	}
	return nullptr;
}

UInputBufferComponent* AMainPlayerController::GetInputBufferComponent()
{
	return InputBufferComponent;
}

#pragma endregion 

#pragma region ObjectInteraction

void AMainPlayerController::StartInteractionWithObject(UInteractableComponent* InteractableComp)
{
	if(!InteractableComp) return;
	ServerStartInteractionWithObject(InteractableComp);
	ClientStartInteractionWithObject(InteractableComp);
}

void AMainPlayerController::EndInteractionWithObject(UInteractableComponent* InteractableComp)
{
	if(!InteractableComp) return;
	ServerEndInteractionWithObject(InteractableComp);
	ClientEndInteractionWithObject(InteractableComp);
}

void AMainPlayerController::RemoveInteractionWithObject(UInteractableComponent* InteractableComp)
{
	if(!InteractableComp) return;
	ServerRemoveInteractionWithObject(InteractableComp);
	ClientRemoveInteractionWithObject(InteractableComp);
}

void AMainPlayerController::InitializeInteractionWithObject(UInteractableComponent* InteractableComp)
{
	if(!InteractableComp) return;
	ClientInitializeInteractionWithObject(InteractableComp);
}

AActor* AMainPlayerController::GetCurrentInteractableObject()
{
	if(CurrentInteractable)
	{
		return CurrentInteractable->GetOwner();
	}
	return nullptr;
}

#pragma endregion

#pragma region Internal

void AMainPlayerController::ServerStartInteractionWithObject_Implementation(UInteractableComponent* InteractableComp)
{
	StartInteraction(InteractableComp);
}

void AMainPlayerController::ClientStartInteractionWithObject_Implementation(UInteractableComponent* InteractableComp)
{
	if(!InteractableComp) return;
	InteractableComp->ClientInteraction(this);
}

void AMainPlayerController::StartInteraction(UInteractableComponent* InteractableComp)
{
	// 1. 핵심 파라미터 유효성 검사 (Guard Clause)
	// 상호작용할 컴포넌트 자체가 유효하지 않으면 즉시 함수를 종료합니다.
	if (!InteractableComp)
	{
		return;
	}

	// 2. 컴포넌트의 소유 액터 가져오기
	// InteractableComp가 유효하므로 GetOwner()는 안전합니다.
	AActor* const InteractingActor = InteractableComp->GetOwner();
	if (!InteractingActor)
	{
		return;
	}
    
	// 3. 필요한 다른 컴포넌트 및 객체 가져오기
	UInventoryCoreComponent* const InventoryCore = InteractingActor->FindComponentByClass<UInventoryCoreComponent>();
	if (!InventoryCore)
	{
		return;
	}

	APlayerState* CurrentPlayerState = GetPlayerState<APlayerState>();
	if (!CurrentPlayerState)
	{
		return;
	}

	// 4. 다른 클라이언트들에게 이 인벤토리의 아이템 정보를 업데이트하도록 요청합니다.
	InventoryComponent->Server_UpdateItems(InteractingActor);
    
	// 5. 이 인벤토리를 보고 있는 '뷰어' 목록에 현재 플레이어를 추가합니다.
	InventoryComponent->AddViewer(CurrentPlayerState, InventoryCore);
	
}

void AMainPlayerController::ServerEndInteractionWithObject_Implementation(UInteractableComponent* InteractableComponent)
{
	EndInteraction(InteractableComponent);
}

void AMainPlayerController::ClientInitializeInteractionWithObject_Implementation(
	UInteractableComponent* InteractableComp)
{
	InitializeInteraction(InteractableComp);
}

void AMainPlayerController::InitializeInteraction(UInteractableComponent* InteractableComp)
{
	if(!InteractableComp)
	{
		return;
	}
	InteractableComp->OnPreInteraction(this);
}


void AMainPlayerController::ServerRemoveInteractionWithObject_Implementation(
	UInteractableComponent* InteractableComp)
{
	RemoveInteraction(InteractableComp);	
}

void AMainPlayerController::ClientRemoveInteractionWithObject_Implementation(
	UInteractableComponent* InteractableComp)
{
	if(!InteractableComp) return;
	InteractableComp->ClientRemoveInteraction();
}

void AMainPlayerController::RemoveInteraction(UInteractableComponent* InteractableComp)
{
	if(!InteractableComp)
	{
		return;
	}

	AActor* LocalActor = InteractableComp->GetOwner();
	if(!LocalActor || !LocalActor->GetIsReplicated())
	{
		return;
	}
	
	IInteractableInterface::Execute_RemoveInteraction(InteractableComp);
}
void AMainPlayerController::ClientEndInteractionWithObject_Implementation(UInteractableComponent* InteractableComp)
{
	if (!InteractableComp)
	{
		return;
	}
	
	IInteractableInterface::Execute_ClientEndInteraction(InteractableComp,this);
}

void AMainPlayerController::EndInteraction(UInteractableComponent* InteractableComp)
{
	if (!InteractableComp)
	{
		DebugHeader::LogWarning(TEXT("EndInteraction : InteractableComp is nullptr"));
		return;
	}
	AActor* const InteractingActor = InteractableComp->GetOwner();
	if (!InteractingActor)
	{
		DebugHeader::LogWarning(TEXT("EndInteraction : InteractingActor is nullptr"));
		return;
	}
	
	UInventoryCoreComponent* const InventoryCore = InteractingActor->FindComponentByClass<UInventoryCoreComponent>();
	if (!InventoryCore)
	{
		DebugHeader::LogWarning(TEXT("EndInteraction : InventoryCore is nullptr"));
		return;
	}

	APlayerState* const CurrentPlayerState = GetPlayerState<APlayerState>();
	if (!CurrentPlayerState)
	{
		DebugHeader::LogWarning(TEXT("EndInteraction : CurrentPlayerState is nullptr"));
		return;
	}
	InventoryComponent->RemoveViewer(CurrentPlayerState,InventoryCore);
	IInteractableInterface::Execute_EndInteraction(InteractableComp,this);
}
#pragma endregion 
