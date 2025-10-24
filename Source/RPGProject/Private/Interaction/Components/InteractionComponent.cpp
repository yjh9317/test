

#include "Interaction/Components/InteractionComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Header/CollisionType.h"
#include "Header/DebugHeader.h"
#include "Interaction/Components/InteractableComponent.h"
#include "Interface/ObjectInteraction.h"


UInteractionComponent::UInteractionComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}


void UInteractionComponent::InitializeInteraction_Implementation(APlayerController* PlayerController)
{
	ControllerRef = PlayerController;
	if(UWorld* World = GetWorld())
	{
		World->GetTimerManager().ClearTimer(InteractionTimer);
		World->GetTimerManager().SetTimer(
			InteractionTimer,
			this,
			&UInteractionComponent::ClientUpdateInteraction,
			InteractionDelay,
			true,
			1.f);
	}
	
}

void UInteractionComponent::ClientUpdateInteraction()
{
	UpdateInteraction();
}

void UInteractionComponent::UpdateInteraction()
{
	UpdateCurrentInteractable();
	
	APawn* ControllerPawn = ControllerRef ? ControllerRef->GetPawn() : nullptr;
	if (!ControllerPawn) return;
	ControlledPawnRef = ControllerPawn;

	UInteractableComponent* CameraTraceResult = FindInteractableViaTrace();
	UInteractableComponent* ForwardTraceResult = FindInteractableViaForwardTrace();
	

	// DebugDrawInteractionStatus(
	// 	CameraTraceResult && ForwardTraceResult && CameraTraceResult == ForwardTraceResult,
	// 	CameraTraceResult, 
	// 	ForwardTraceResult
	// );

	if (!CurrentInteractable)
	{
		if (CameraTraceResult && ForwardTraceResult && 
			CameraTraceResult == ForwardTraceResult)
		{
			AssignInteractionToLocal(CameraTraceResult);
		}
	}
	// 현재 상호작용 중인 객체가 있는 경우
	else
	{
		// 상호작용 유지 조건 체크
		bool bCameraDetectsCurrent = (CameraTraceResult == CurrentInteractable);
		bool bForwardDetectsCurrent = (ForwardTraceResult == CurrentInteractable);
		
		// 둘 중 하나라도 현재 객체를 감지하면 유지
		if (bCameraDetectsCurrent || bForwardDetectsCurrent)
		{
			// 상호작용 유지 (아무것도 하지 않음)
			return;
		}
		
		// 둘 다 현재 객체를 감지하지 못하는 경우
		// (둘 다 nullptr이거나 다른 객체를 감지)
		RemoveInteractionFromCurrent();
		
		// 만약 두 트레이스가 새로운 동일한 객체를 감지했다면 즉시 새 상호작용 시작
		if (CameraTraceResult && ForwardTraceResult && 
			CameraTraceResult == ForwardTraceResult)
		{
			AssignInteractionToLocal(CameraTraceResult);
		}
	}
}

void UInteractionComponent::RemoveInteractionFromCurrent()
{
	if(!CurrentInteractable) return;

	CurrentInteractable->ToggleHighlight(false,ControllerRef);
	if(IObjectInteraction* ObjectInteraction = Cast<IObjectInteraction>(GetOwner()))
	{
		ObjectInteraction->EndInteractionWithObject(CurrentInteractable);
		CurrentInteractable = nullptr;
		OnRemovedCurrentInteractable.Broadcast();
	}
}

void UInteractionComponent::AssignInteractionToLocal(UInteractableComponent* InteractableComponent)
{
	if(InteractableComponent == CurrentInteractable)
	{
		return;		
	}
	RemoveInteractionFromCurrent();

	InteractableComponent->ToggleHighlight(true,ControllerRef);
	CurrentInteractable = InteractableComponent;
	OnNewInteractableAssigned.Broadcast(CurrentInteractable);
}

UInteractableComponent* UInteractionComponent::FindInteractableViaForwardTrace()
{
	if(!ControlledPawnRef || !GetWorld())
	{
		return false;
	}

	FVector PawnLocation = ControlledPawnRef->GetActorLocation();
	FVector PawnForward = ControlledPawnRef->GetActorForwardVector();
	FVector StartLocation = PawnLocation;
	FVector EndLocation = PawnLocation + PawnForward * OwnerTraceLength;

	TArray<AActor*> IgnoredActors{ControlledPawnRef};
	TArray<FHitResult> OutHits{};
	
	bool bIsHit = PerformInteractionTrace(OutHits,StartLocation,EndLocation,IgnoredActors);

	UInteractableComponent* FoundInteractableComp = nullptr;
	
	if (!bIsHit)
	{
		return nullptr;
	}

	for (const FHitResult& HitResult : OutHits)
	{
		AActor* HitActor = HitResult.GetActor();
		if (!HitActor) continue;

		FoundInteractableComp = HitActor->FindComponentByClass<UInteractableComponent>();
		if (FoundInteractableComp && IsValidInteractable(FoundInteractableComp))
		{
			return FoundInteractableComp;
		}
	}

	return nullptr;
}

void UInteractionComponent::UpdateCurrentInteractable()
{
	if(nullptr == CurrentInteractable)
	{
		return;
	}
	
	if(IInteractableInterface* InteractableInterface = Cast<IInteractableInterface>(CurrentInteractable->GetOwner()))
	{
		if(!InteractableInterface->Execute_CanBeInteractedWith(CurrentInteractable->GetOwner()))
		{
			RemoveInteractionFromCurrent();
		}
	}
}

bool UInteractionComponent::PerformInteractionTrace(TArray<FHitResult>& OutHits, const FVector& StartLocation,
	const FVector& EndLocation ,const TArray<AActor*>& IgnoredActors, float CapsuleRadius, float CapsuleHalfHeight)
{
	// Collision Query Parameters 설정
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.bTraceComplex = false;
	CollisionQueryParams.bReturnPhysicalMaterial = false;

	for(const auto& Actor : IgnoredActors)
	{
		CollisionQueryParams.AddIgnoredActor(Actor);
	}
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypesToQuery;
	ObjectTypesToQuery.Add(UEngineTypes::ConvertToObjectType(ECO_Interactable));

	bool bIsHit = UKismetSystemLibrary::CapsuleTraceMultiForObjects(
		GetWorld(),
		StartLocation,
		EndLocation,
		CapsuleRadius,
		CapsuleHalfHeight,
		ObjectTypesToQuery,
		false,
		IgnoredActors,
		DebugTrace,
		OutHits,
		true,
		TraceColor,
		TraceHitColor,
		DebugDrawTime
	);
    
	return bIsHit;
}

UInteractableComponent* UInteractionComponent::FindInteractableViaTrace()
{
	UCameraComponent* PlayerCamera = ControlledPawnRef->FindComponentByClass<UCameraComponent>();
	if (!PlayerCamera) return nullptr;

	const FVector PawnLocation = ControlledPawnRef->GetActorLocation();
	const FVector CameraForward = PlayerCamera->GetForwardVector();
	const FVector StartLocation = PawnLocation + FVector(0.f, 0.f, InteractionDistance);
	const FVector EndLocation = StartLocation + CameraForward * CameraTraceLength;

	TArray<FHitResult> OutHits;
	const TArray<AActor*> IgnoredActors{ ControlledPawnRef };
	if (!PerformInteractionTrace(OutHits, StartLocation, EndLocation, IgnoredActors,35.f,0.f))
	{
		return nullptr;
	}

	for (const FHitResult& Hit : OutHits)
	{
		AActor* HitActor = Hit.GetActor();
		if (!IsValid(HitActor)) continue;

		UInteractableComponent* FoundComponent  = HitActor->FindComponentByClass<UInteractableComponent>();
    
		if (FoundComponent && IsValidInteractable(FoundComponent))
		{
			return FoundComponent;
		}
	}

	return nullptr;
}

bool UInteractionComponent::IsValidInteractable(UInteractableComponent* Component) const
{
	if(!Component)
	{
		//DebugHeader::LogWarning(TEXT("IsValidInteractable: Component is not valid."));
		return false;
	}
	AActor* OwnerActor = Component->GetOwner();
	if (!OwnerActor || !ControlledPawnRef)
	{
		return false;
	}

	bool bInterface = OwnerActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass());
	bool bCanInteraction = IInteractableInterface::Execute_CanBeInteractedWith(OwnerActor);
	if (!bInterface || !bCanInteraction)
	{
		return false;
	}

	UBoxComponent* BoxArea = Component->InteractableArea;
	if (!IsValid(BoxArea))
	{
		return false;
	}
	
	return true;
}


#if WITH_EDITOR
	void UInteractionComponent::DebugDrawInteractionStatus(bool bBothConditionsMet, 
		UInteractableComponent* CameraTraceResult, 
		UInteractableComponent* ForwardTraceResult) const
	{
		if (DebugTrace != EDrawDebugTrace::None && GEngine)
		{
			FString CurrentStatus = CurrentInteractable ? 
				FString::Printf(TEXT("Current: %s"), *CurrentInteractable->GetOwner()->GetName()) : 
				TEXT("Current: None");
		
			FString CameraStatus = CameraTraceResult ? 
				FString::Printf(TEXT("Camera: %s"), *CameraTraceResult->GetOwner()->GetName()) : 
				TEXT("Camera: None");
		
			FString ForwardStatus = ForwardTraceResult ? 
				FString::Printf(TEXT("Forward: %s"), *ForwardTraceResult->GetOwner()->GetName()) : 
				TEXT("Forward: None");
		
			FString DebugMessage = FString::Printf(
				TEXT("%s | %s | %s | Both Met: %s"),
				*CurrentStatus,
				*CameraStatus,
				*ForwardStatus,
				bBothConditionsMet ? TEXT("YES") : TEXT("NO")
			);
		
			// 상태에 따라 색상 결정
			FColor MessageColor = FColor::Yellow;
			if (CurrentInteractable)
			{
				// 상호작용 중
				bool bMaintained = (CameraTraceResult == CurrentInteractable) || 
								  (ForwardTraceResult == CurrentInteractable);
				MessageColor = bMaintained ? FColor::Green : FColor::Orange;
			}
			else if (bBothConditionsMet)
			{
				// 새로운 상호작용 가능
				MessageColor = FColor::Cyan;
			}
			else
			{
				// 상호작용 불가
				MessageColor = FColor::Red;
			}
		
			GEngine->AddOnScreenDebugMessage(-1, 0.1f, MessageColor, DebugMessage);
		}
#endif
}