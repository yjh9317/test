
#include "Interaction/Components/InteractableComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Header/CollisionType.h"
#include "Header/DebugHeader.h"
#include "Interaction/Data/InteractableDefinitions.h"
#include "Interface/InteractableInterface.h"
#include "Interface/ObjectInteraction.h"
#include "Inventory/UI/InteractionPromptWidget.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "UObject/ConstructorHelpers.h"

UInteractableComponent::UInteractableComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);

	static ConstructorHelpers::FClassFinder<UInteractionPromptWidget> WidgetClassFinder(
	TEXT("/Game/Blueprint/UI/Interaction/WBP_InteractionPrompt")
	);
	
	if (WidgetClassFinder.Succeeded())
	{
		InteractionWidgetClass = WidgetClassFinder.Class;
	}
	
	check(InteractionWidgetClass);
}

void UInteractableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}

void UInteractableComponent::BeginPlay()
{
	AActor* OwnerActor = GetOwner();
	IInteractableInterface* InteractableInterface = Cast<IInteractableInterface>(GetOwner());
	if(OwnerActor && OwnerActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		IInteractableInterface::Execute_Initialize(OwnerActor);
	}
}

void UInteractableComponent::AssociatedActorInteraction(AActor* Interactor)
{
	CurrentInteractor = Interactor;

	TArray<AActor*> Actors;
	AssociatedInteractableActors.GetKeys(Actors);

	for(const auto Actor : Actors)
	{
		if(Actor)
		{
			if(UInteractableComponent* InteractableComponent = Actor->FindComponentByClass<UInteractableComponent>())
			{
				InteractableComponent->CheckForInteractionWithAssociate(Interactor);	
			}
		}
	}
}

void UInteractableComponent::OnInteraction(AActor* Interactor)
{
	CurrentInteractor = Interactor;
	AlreadyInteracted = true;

	if(GetOwner()->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		IInteractableInterface::Execute_Interaction(GetOwner(),CurrentInteractor);
	}
	
	RemoveInteractionByResponse();
}

void UInteractableComponent::CheckForInteractionWithAssociate(AActor* Interactor)
{
	if(Interactor)
	{
		CurrentInteractor = Interactor;
		if(CheckForAssociatedActors && IsTargetInteractableValue())
		{
			OnInteraction(CurrentInteractor);
			
			TArray<AActor*> Actors;
			AssociatedInteractableActors.GetKeys(Actors);
			
			if(RemoveAssociatedInteractableOnComplete || InteractionResponse == EInteractionResponse::OnlyOnce)
			{
				for(const auto Actor : Actors)
				{
					if(UInteractableComponent* Comp = Actor->FindComponentByClass<UInteractableComponent>())
					{
						OnRemoveInteraction();
						Comp->InteractionResponse = EInteractionResponse::OnlyOnce;
					}
				}
			}
			else if (InteractionResponse == EInteractionResponse::Temporary)
			{
				for(const auto Actor : Actors)
				{
					if(UInteractableComponent* Comp = Actor->FindComponentByClass<UInteractableComponent>())
					{
						OnRemoveInteraction();
						Comp->ToggleCanBeReInitialized(false);
					}
				}
			}
		}
	}
}

void UInteractableComponent::ClientInteraction(AActor* Interactor)
{
	if(GetOwner()->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		IInteractableInterface::Execute_ClientStartInteraction(GetOwner(),Interactor);
	}
}

void UInteractableComponent::ToggleHighlight(bool Highlight, AActor* Interactor)
{
	if(InteractionWidget)
	{
		InteractionWidget->SetVisibility(Highlight,false);
	}
	for (UPrimitiveComponent* Component : ObjectsToHighlight)
	{
		if (Component)
		{
			Component->SetRenderCustomDepth(Highlight);
		}
	}
	APlayerController* OwningPC = Cast<APlayerController>(Interactor);
	Server_SetWidgetLocalOwner(OwningPC);
	//SetWidgetLocalOwner(OwningPC);
}

void UInteractableComponent::SetupInteractableReferences(UBoxComponent* Area, UWidgetComponent* Widget,
	TSet<UPrimitiveComponent*> HighlightableObjects)
{
	if(Area) 
	{
		InteractableArea = Area;
	}
	if(Widget)
	{
		InteractionWidget = Widget;
	}
	TArray<UPrimitiveComponent*> HighlightableObjectsArray;
	for (const auto& Object : HighlightableObjects)
	{
		HighlightableObjectsArray.Add(Object);
	}
	ObjectsToHighlight = HighlightableObjectsArray;

	for(const auto Object : ObjectsToHighlight)
	{
		Object->SetCollisionResponseToChannel(ECO_Interactable,ECR_Block);
	}
}



void UInteractableComponent::DurationPress()
{
	GetWorld()->GetTimerManager().SetTimer(
		KeyDownTimer,
		this,
		&UInteractableComponent::IsKeyDown,
		DurationInputDelay,
		true);

	FKey InteractionKey{};
	if(GetPressedKeyByAction(InteractionAction,InteractionKey))
	{
		PressedInteractionKey = InteractionKey;
	}
	else
	{
		PressedInteractionKey = EKeys::E;
	}
}

void UInteractableComponent::IsKeyDown()
{
	IObjectInteraction* ObjectInteraction = Cast<IObjectInteraction>(CurrentInteractor);
	if(nullptr == ObjectInteraction)
	{
		return;
	}

	AActor* CurrentInteraction = ObjectInteraction->GetCurrentInteractableObject();
	if (CurrentInteraction && CurrentInteraction == GetOwner())
	{
		const EOperationStatus InputState = HoldingInput();
		if (InputState == EOperationStatus::Complete)
		{
			GetWorld()->GetTimerManager().ClearTimer(KeyDownTimer);
			ObjectInteraction->StartInteractionWithObject(this);
		}
		else if (InputState == EOperationStatus::Reset)
		{
			GetWorld()->GetTimerManager().ClearTimer(KeyDownTimer);
			OnUpdateHoldingValue.Broadcast(0.05f);
		}
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(KeyDownTimer);
	}
}

void UInteractableComponent::MultiplePress()
{
	EOperationStatus MashHoldState = MashingInput(10);
	if(EOperationStatus::None == MashHoldState)
	{
		OnUpdateMashingValue.Broadcast(CurrentMashingValue);	
	}
	else if(EOperationStatus::Complete == MashHoldState)
	{
		OnUpdateMashingValue.Broadcast(CurrentMashingValue);
		FTimerHandle UnusedHandle;
		GetWorld()->GetTimerManager().SetTimer(UnusedHandle, [this]()
		{
			IObjectInteraction* ObjectInteraction = Cast<IObjectInteraction>(CurrentInteractor);
			if(ObjectInteraction)
			{
				ObjectInteraction->StartInteractionWithObject(this);
			}
		}, 0.2f, false);
		
	}
}

void UInteractableComponent::OnPreInteraction(AActor* Interactor)
{
	if(nullptr == Interactor) return;

	CurrentInteractor = Interactor;

	if (GetOwner()->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		IInteractableInterface::Execute_ClientPreInteraction(GetOwner(), CurrentInteractor);
	}
	
	switch (InputType)
	{
	case EInteractionInputType::Single:
		{
			// DebugHeader::LogWarning(TEXT("Single"));
			IObjectInteraction* ObjectInteraction = Cast<IObjectInteraction>(CurrentInteractor);
			if (ObjectInteraction)
			{
				ObjectInteraction->StartInteractionWithObject(this);
			}
			break;
		}
	case EInteractionInputType::Holding:
		{
			// DebugHeader::LogWarning(TEXT("Holding"));
			DurationPress();
			break;
		}
	case EInteractionInputType::MultipleAndMashing:
		{
			// DebugHeader::LogWarning(TEXT("MultipleAndMashing"));
			MultiplePress();
			break;
		}
	}
}

void UInteractableComponent::OnRemoveInteraction_Implementation()
{
	ClientRemoveInteraction();
}

void UInteractableComponent::ClientRemoveInteraction()
{
	if (GetOwner()->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		IInteractableInterface::Execute_RemoveInteraction(GetOwner());
	}
	bIsInteractable = false;
	InteractableArea = nullptr;
	if(DestroyAfterPickup)
	{
		GetOwner()->Destroy();
	}
}

void UInteractableComponent::OnEndInteraction(AActor* Interactor)
{
	if(GetOwner()->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		IInteractableInterface::Execute_EndInteraction(GetOwner(),Interactor);	
	}
}

void UInteractableComponent::AssociatedActorEndInteraction()
{
	TArray<AActor*> Keys{};
	AssociatedInteractableActors.GetKeys(Keys);

	for(auto Key : Keys)
	{
		if(Key)
		{
			if(UInteractableComponent* InteractableComponent = Key->FindComponentByClass<UInteractableComponent>())
			{
				InteractableComponent->EndInteraction(CurrentInteractor);
			}
		}
	}
}

void UInteractableComponent::Reinitialize()
{
	if (InteractionResponse == EInteractionResponse::Temporary)
	{
		if (CanBeReInitialized)
		{
			if(GetOwner()->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
			{
				IInteractableInterface::Execute_Initialize(GetOwner());
			}
		}
		ToggleIsInteractable(true);
		if (InteractionWidgetRef)
		{
			InteractionWidgetRef->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void UInteractableComponent::ReInitializeAssociatedActors()
{
	TArray<AActor*> Keys{};
	AssociatedInteractableActors.GetKeys(Keys);

	for(auto Key : Keys)
	{
		if(Key)
		{
			if(UInteractableComponent* InteractableComponent = Key->FindComponentByClass<UInteractableComponent>())
			{
				InteractableComponent->ToggleCanBeReInitialized(true);
				Reinitialize();
			}
		}
	}
}


void UInteractableComponent::OnClientEndInteraction(AActor* Interactor)
{
	if(GetOwner()->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		IInteractableInterface::Execute_ClientEndInteraction(GetOwner(),Interactor);
	}
}

void UInteractableComponent::ToggleIsInteractable(bool Condition)
{
	if(bIsInteractable != Condition)
	{
		bIsInteractable = Condition;
	}
}

void UInteractableComponent::ToggleInteractionWidget(bool Condition)
{
	if(bIsInteractable && InteractionWidgetRef)
	{
		InteractionWidgetRef->SetVisibility(
			Condition ? ESlateVisibility::Hidden : ESlateVisibility::SelfHitTestInvisible);
	}
}

void UInteractableComponent::RemoveInteractionByResponse()
{
	if(InteractionResponse == EInteractionResponse::OnlyOnce || InteractionResponse == EInteractionResponse::Temporary)
	{
		OnRemoveInteraction();
	}
}

void UInteractableComponent::ChangeInteractableValue(bool Increment)
{
	if(Increment)
	{
		if(++InteractableValue > InteractableLimitValue)
		{
			InteractableValue = 0;
		}
	}
	else
	{
		if(--InteractableValue < 0)
		{
			InteractableValue = InteractableLimitValue;
		}
	}
}

void UInteractableComponent::ToggleCanBeReInitialized(bool Condition)
{
	if(CanBeReInitialized != Condition)
	{
		CanBeReInitialized = Condition;
	}
}

void UInteractableComponent::SetWidgetLocalOwner(APlayerController* OwningPlayer)
{
	if(nullptr == OwningPlayer) return;

	if(InteractionWidgetClass)
	{
		InteractionWidgetRef = CreateWidget<UInteractionPromptWidget>(OwningPlayer,InteractionWidgetClass);
		InteractionWidgetRef->InputType = InputType;
		InteractionWidgetRef->InteractableComponent = this;
		
		InteractionWidget->SetWidget(InteractionWidgetRef);
		InteractionWidget->SetOwnerPlayer(InteractionWidgetRef->GetOwningLocalPlayer());

		InteractionWidget->SetWidgetSpace(EWidgetSpace::Screen);
		InteractionWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		InteractionWidget->SetDrawAtDesiredSize(true);
	}
}

void UInteractableComponent::Server_SetWidgetLocalOwner_Implementation(APlayerController* OwningPlayer)
{
	SetWidgetLocalOwner(OwningPlayer);
}

EOperationStatus UInteractableComponent::HoldingInput()
{
	APlayerController* Controller = Cast<APlayerController>(CurrentInteractor);
	if(Controller)
	{
		float KeyDownTime = Controller->GetInputKeyTimeDown(PressedInteractionKey);

		if(KeyDownTime <= 0.0f)
   		{
   			return EOperationStatus::Reset;
   		}
		
		OnUpdateHoldingValue.Broadcast(KeyDownTime);
		
		if(KeyDownTime > MaxKeyTimeDown)
		{
			return EOperationStatus::Complete;
		}

	}
	return EOperationStatus::None;
}

EOperationStatus UInteractableComponent::MashingInput(int32 MashingCount)
{
	CurrentMashingValue += (1.f/MashingCount);
	
	// DebugHeader::LogWarning(TEXT("Mashing Progress: %f"),CurrentMashingValue);
	
	if(CurrentMashingValue >= 1.f)
	{
		CurrentMashingValue = 0.f;
		GetWorld()->GetTimerManager().ClearTimer(MashingTimer);
		OnUpdateHoldingValue.Broadcast(0.05f);
		return EOperationStatus::Complete;
	}
	
	GetWorld()->GetTimerManager().SetTimer(
		MashingTimer,
		this,
		&UInteractableComponent::ResetMashingProgress, // 타이머 만료 시 호출될 함수
		MashingKeyRetriggerableTime,
		false
	);
	return EOperationStatus::None;
}

void UInteractableComponent::ResetMashingProgress()
{
	CurrentMashingValue = 0.f;
	OnUpdateHoldingValue.Broadcast(0.05f);
	UE_LOG(LogTemp, Warning, TEXT("ResetMashingProgress"));
}

bool UInteractableComponent::IsTargetInteractableValue()
{
	TArray<AActor*> Actors;
	AssociatedInteractableActors.GetKeys(Actors);

	for(const auto Actor : Actors)
	{
		int32 Value = *AssociatedInteractableActors.Find(Actor);
		UInteractableComponent* Comp = Actor->FindComponentByClass<UInteractableComponent>();
		if(Actor && Comp)
		{
			if(Comp->InteractableValue == Value)
			{
				return true;
			}
		}
		
	}
	return false;
}

bool UInteractableComponent::GetPressedKeyByAction(UInputAction* Action,FKey& OutKey)
{
	if (APlayerController* PC = Cast<APlayerController>(CurrentInteractor))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
			if (InputSubsystem)
			{
				TArray<FKey> Keys = InputSubsystem->QueryKeysMappedToAction(Action);
				bool IsValid = false;
				int index = 0;
				for (int i = 0; i < Keys.Num(); ++i)
				{
					FKey CurrentKey = Keys[i];
					if(PC->WasInputKeyJustPressed(CurrentKey))
					{
						IsValid = true;
						index = i;
						break;	
					}
				}

				if(IsValid)
				{
					OutKey = Keys[index];
					return true;
				}
			}
		}
	}
	return false;
}
