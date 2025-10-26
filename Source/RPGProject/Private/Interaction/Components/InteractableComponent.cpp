
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
	if(!OwnerActor) return;
	
	IInteractableInterface* InteractableOwner = Cast<IInteractableInterface>(OwnerActor);

	if (InteractableOwner)
	{
		InteractableOwner->InitializeInteractable();
	}
}

void UInteractableComponent::AssociatedActorInteraction(AActor* Interactor)
{
	CurrentInteractor = Interactor;

	TArray<AActor*> Actors;
	AssociatedInteractableActors.GetKeys(Actors);

	for(const auto* Actor : Actors)
	{
		if(!Actor) continue;

		if (UInteractableComponent* InteractableComponent = Actor->FindComponentByClass<UInteractableComponent>())
		{
			InteractableComponent->CheckForInteractionWithAssociate(Interactor);
		}
	}
}

void UInteractableComponent::OnInteraction(AActor* Interactor)
{
	CurrentInteractor = Interactor;
	AlreadyInteracted = true;

	AActor* OwnerActor = GetOwner();
	if(!OwnerActor) return;

	IInteractableInterface* InteractableOwner = Cast<IInteractableInterface>(OwnerActor);

	if (InteractableOwner)
	{
		InteractableOwner->ExecuteInteraction(CurrentInteractor);
	}
	
	RemoveInteractionByResponse();
}

void UInteractableComponent::CheckForInteractionWithAssociate(AActor* Interactor)
{
	if (!Interactor)
	{
		return;
	}

	CurrentInteractor = Interactor;
	
	// 연결된 액터 체크가 활성화되고 목표 값이 맞는 경우
	if (CheckForAssociatedActors && IsTargetInteractableValue())
	{
		OnInteraction(CurrentInteractor);
		
		TArray<AActor*> Actors;
		AssociatedInteractableActors.GetKeys(Actors);
		
		// OnlyOnce 또는 완료 시 제거 옵션이 활성화된 경우
		if (RemoveAssociatedInteractableOnComplete || InteractionResponse == EInteractionResponse::OnlyOnce)
		{
			for (const auto Actor : Actors)
			{
				if (UInteractableComponent* Comp = Actor->FindComponentByClass<UInteractableComponent>())
				{
					OnRemoveInteraction();
					Comp->InteractionResponse = EInteractionResponse::OnlyOnce;
				}
			}
		}
		// Temporary 응답인 경우
		else if (InteractionResponse == EInteractionResponse::Temporary)
		{
			for (const auto Actor : Actors)
			{
				if (UInteractableComponent* Comp = Actor->FindComponentByClass<UInteractableComponent>())
				{
					OnRemoveInteraction();
					Comp->ToggleCanBeReInitialized(false);
				}
			}
		}
	}
}

void UInteractableComponent::ClientInteraction(AActor* Interactor)
{
	AActor* OwnerActor = GetOwner();
	if(!OwnerActor) return;
	IInteractableInterface* InteractableOwner = Cast<IInteractableInterface>(OwnerActor);

	if (InteractableOwner)
	{
		InteractableOwner->ClientBeginInteraction(Interactor);
	}
}

void UInteractableComponent::ToggleHighlight(bool Highlight, AActor* Interactor)
{
	// 상호작용 위젯 표시/숨김
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(Highlight, false);
	}
	// 하이라이트 대상 오브젝트들의 커스텀 뎁스 설정
	for (UPrimitiveComponent* Component : ObjectsToHighlight)
	{
		if (Component)
		{
			Component->SetRenderCustomDepth(Highlight);
		}
	}
	// 위젯 소유자 설정 (서버 RPC)
	if (APlayerController* OwningPC = Cast<APlayerController>(Interactor))
	{
		Server_SetWidgetLocalOwner(OwningPC);
		//SetWidgetLocalOwner(OwningPC);
	}
}

void UInteractableComponent::SetupInteractableReferences(UBoxComponent* Area, UWidgetComponent* Widget,
	TSet<UPrimitiveComponent*> HighlightableObjects)
{
	if (Area) 
	{
		InteractableArea = Area;
	}
	
	if (Widget)
	{
		InteractionWidget = Widget;
	}
	
	// 하이라이트 대상 오브젝트 배열 변환 및 설정
	ObjectsToHighlight.Reserve(HighlightableObjects.Num());
	for (const auto& Object : HighlightableObjects)
	{
		if (Object)
		{
			ObjectsToHighlight.Add(Object);
			Object->SetCollisionResponseToChannel(ECO_Interactable, ECR_Block);
		}
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
	if (!ObjectInteraction)
	{
		return;
	}

	AActor* CurrentInteraction = ObjectInteraction->GetCurrentInteractableObject();
	
	// 현재 상호작용 중인 오브젝트가 이 컴포넌트의 소유자인 경우
	if (CurrentInteraction && CurrentInteraction == GetOwner())
	{
		const EOperationStatus InputState = HoldingInput();
		
		// 홀딩 완료
		if (InputState == EOperationStatus::Complete)
		{
			GetWorld()->GetTimerManager().ClearTimer(KeyDownTimer);
			ObjectInteraction->StartInteractionWithObject(this);
		}
		// 키를 뗀 경우 (리셋)
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
	
	// 진행 중
	if (EOperationStatus::None == MashHoldState)
	{
		OnUpdateMashingValue.Broadcast(CurrentMashingValue);	
	}
	// 매싱 완료
	else if (EOperationStatus::Complete == MashHoldState)
	{
		OnUpdateMashingValue.Broadcast(CurrentMashingValue);
		
		// 딜레이 후 상호작용 시작
		FTimerHandle UnusedHandle;
		GetWorld()->GetTimerManager().SetTimer(UnusedHandle, [this]()
		{
			IObjectInteraction* ObjectInteraction = Cast<IObjectInteraction>(CurrentInteractor);
			if (ObjectInteraction)
			{
				ObjectInteraction->StartInteractionWithObject(this);
			}
		}, 0.2f, false);
	}
}

void UInteractableComponent::OnPreInteraction(AActor* Interactor)
{
	if (!Interactor)
	{
		return;
	}

	CurrentInteractor = Interactor;

	IInteractableInterface* InteractableOwner = Cast<IInteractableInterface>(CurrentInteractor);
	if (InteractableOwner)
	{
		InteractableOwner->ClientPrepareInteraction();
	}
	
	switch (InputType)
	{
	case EInteractionInputType::Single:
		{
			IObjectInteraction* ObjectInteraction = Cast<IObjectInteraction>(CurrentInteractor);
			if (ObjectInteraction)
			{
				ObjectInteraction->StartInteractionWithObject(this);
			}
			break;
		}
		
	case EInteractionInputType::Holding:
		{
			DurationPress();
			break;
		}
		
	case EInteractionInputType::MultipleAndMashing:
		{
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
	AActor* OwnerActor = GetOwner();
	if(!OwnerActor) return;
	
	IInteractableInterface* InteractableOwner = Cast<IInteractableInterface>(OwnerActor);
	if (InteractableOwner)
	{
		InteractableOwner->RemoveInteraction();
	}
	// 상호작용 불가 상태로 변경
	bIsInteractable = false;
	InteractableArea = nullptr;
	
	// 픽업 후 파괴 옵션이 활성화된 경우
	if(DestroyAfterPickup)
	{
		GetOwner()->Destroy();
	}
}

void UInteractableComponent::OnEndInteraction(AActor* NewInteractor)
{
	AActor* OwnerActor = GetOwner();
	if(!OwnerActor) return;
	
	IInteractableInterface* InteractableOwner = Cast<IInteractableInterface>(OwnerActor);
	if (InteractableOwner)
	{
		InteractableOwner->EndInteraction(NewInteractor);
	}
}

void UInteractableComponent::AssociatedActorEndInteraction()
{
	TArray<AActor*> Keys;
	AssociatedInteractableActors.GetKeys(Keys);

	for (auto* Key : Keys)
	{
		if (!Key) continue;

		if (UInteractableComponent* InteractableComponent = Key->FindComponentByClass<UInteractableComponent>())
		{
			InteractableComponent->OnEndInteraction(CurrentInteractor);
		}
	}
}

void UInteractableComponent::Reinitialize()
{
	if (InteractionResponse == EInteractionResponse::Temporary)
	{
		if (CanBeReInitialized)
		{
			IInteractableInterface* InteractableOwner = Cast<IInteractableInterface>(GetOwner());
			if (InteractableOwner)
			{
				InteractableOwner->InitializeInteractable();
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
	TArray<AActor*> Keys;
	AssociatedInteractableActors.GetKeys(Keys);

	for (const auto* Key : Keys)
	{
		if (!Key) continue;

		if (UInteractableComponent* InteractableComponent = Key->FindComponentByClass<UInteractableComponent>())
		{
			InteractableComponent->ToggleCanBeReInitialized(true);
			Reinitialize();
		}
	}
}


void UInteractableComponent::OnClientEndInteraction(AActor* Interactor)
{
	IInteractableInterface* InteractableOwner = Cast<IInteractableInterface>(GetOwner());

	if (InteractableOwner)
	{
		InteractableOwner->ClientEndInteraction(Interactor);
	}
}

void UInteractableComponent::ToggleIsInteractable(bool Condition)
{
	if(bIsInteractable != Condition)
	{
		bIsInteractable = Condition;
	}
}

void UInteractableComponent::SetInteractionWidgetVisible(bool Condition)
{
	if(bIsInteractable && InteractionWidgetRef)
	{
		InteractionWidgetRef->SetVisibility(
			Condition ? ESlateVisibility::Hidden : ESlateVisibility::SelfHitTestInvisible);
	}
}

void UInteractableComponent::RemoveInteractionByResponse()
{
	// OnlyOnce 또는 Temporary 응답인 경우 상호작용 제거
	if (InteractionResponse == EInteractionResponse::OnlyOnce || 
		InteractionResponse == EInteractionResponse::Temporary)
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

	if (InteractionWidgetClass)
	{
		// 위젯 생성 및 초기화
		InteractionWidgetRef = CreateWidget<UInteractionPromptWidget>(OwningPlayer, InteractionWidgetClass);
		InteractionWidgetRef->InputType = InputType;
		InteractionWidgetRef->InteractableComponent = this;
		
		// 위젯 컴포넌트 설정
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
	if (!Controller)
	{
		return EOperationStatus::None;
	}

	float KeyDownTime = Controller->GetInputKeyTimeDown(PressedInteractionKey);

	// 키를 뗀 경우
	if (KeyDownTime <= 0.0f)
	{
		return EOperationStatus::Reset;
	}
	
	// 진행도 업데이트
	OnUpdateHoldingValue.Broadcast(KeyDownTime);
	
	// 홀딩 완료
	if (KeyDownTime > MaxKeyTimeDown)
	{
		return EOperationStatus::Complete;
	}

	return EOperationStatus::None;
}

EOperationStatus UInteractableComponent::MashingInput(int32 MashingCount)
{
	CurrentMashingValue += (1.f / MashingCount);
	
	// 매싱 완료
	if (CurrentMashingValue >= 1.f)
	{
		CurrentMashingValue = 0.f;
		GetWorld()->GetTimerManager().ClearTimer(MashingTimer);
		OnUpdateHoldingValue.Broadcast(0.05f);
		return EOperationStatus::Complete;
	}
	
	// 일정 시간 내에 재입력하지 않으면 리셋
	GetWorld()->GetTimerManager().SetTimer(
		MashingTimer,
		this,
		&UInteractableComponent::ResetMashingProgress,
		MashingKeyRetriggerableTime,
		false
	);
	
	return EOperationStatus::None;
}

void UInteractableComponent::ResetMashingProgress()
{
	CurrentMashingValue = 0.f;
	OnUpdateHoldingValue.Broadcast(0.05f);
}

bool UInteractableComponent::IsTargetInteractableValue()
{
	TArray<AActor*> Actors;
	AssociatedInteractableActors.GetKeys(Actors);

	// 연결된 액터들 중 목표 값과 일치하는 것이 있는지 체크
	for (const auto* Actor : Actors)
	{
		int32 Value = *AssociatedInteractableActors.Find(Actor);
		UInteractableComponent* Comp = Actor->FindComponentByClass<UInteractableComponent>();
		
		if (Actor && Comp && Comp->InteractableValue == Value) 
		{
			return true;
		}
	}
	return false;
}

bool UInteractableComponent::GetPressedKeyByAction(UInputAction* Action,FKey& OutKey)
{
	APlayerController* PC = Cast<APlayerController>(CurrentInteractor);
	if (!PC)
	{
		return false;
	}

	ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
	if (!LocalPlayer)
	{
		return false;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSubsystem)
	{
		return false;
	}

	// 액션에 매핑된 모든 키 가져오기
	TArray<FKey> Keys = InputSubsystem->QueryKeysMappedToAction(Action);
	
	// 방금 눌린 키 찾기
	for (int i = 0; i < Keys.Num(); ++i)
	{
		FKey CurrentKey = Keys[i];
		if (PC->WasInputKeyJustPressed(CurrentKey))
		{
			OutKey = Keys[i];
			return true;
		}
	}

	return false;
}
