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

	// 입력 진행도 업데이트 이벤트
	FOnUpdateNewValue OnUpdateHoldingValue;
	FOnUpdateNewValue OnUpdateMashingValue;
	
protected:
	virtual void BeginPlay() override;

public:
	
	// ====================
	// 상호작용 처리
	// ====================
	/** 연결된 액터들의 상호작용 시작 */
	void AssociatedActorInteraction(AActor* Interactor);
	
	/** 연결된 액터와의 상호작용 체크 */
	void CheckForInteractionWithAssociate(AActor* Interactor);
	
	/** 상호작용 실행 */
	void OnInteraction(AActor* Interactor);
	
	/** 클라이언트 측 상호작용 처리 */
	void ClientInteraction(AActor* Interactor);
	
	/** 상호작용 제거 (네트워크 멀티캐스트) */
	UFUNCTION(NetMulticast, Reliable)
	void OnRemoveInteraction();
	
	/** 클라이언트 측 상호작용 제거 */
	void ClientRemoveInteraction();
	
	/** 연결된 액터들의 상호작용 종료 */
	void AssociatedActorEndInteraction();
	
	
	// ====================
	// 상호작용 전처리
	// ====================
	
	/** 하이라이트 토글 */
	void ToggleHighlight(bool Highlight, AActor* Interactor);
	
	/** 상호작용 참조 설정 */
	void SetupInteractableReferences(UBoxComponent* Area, UWidgetComponent* Widget, TSet<UPrimitiveComponent*> HighlightableObjects);
	
	/** 홀딩 입력 처리 시작 */
	void DurationPress();
	
	/** 키 입력 지속 체크 */
	void IsKeyDown();
	
	/** 연타 입력 처리 */
	void MultiplePress();
	
	/** 상호작용 전 준비 (입력 타입에 따른 분기) */
	void OnPreInteraction(AActor* Interactor);
	
	// ====================
	// 상호작용 후처리
	// ====================
	
	/** 상호작용 종료 처리 */
	void OnEndInteraction(AActor* Interactor);
	
	/** 재초기화 (Temporary 타입용) */
	void Reinitialize();
	
	/** 연결된 액터들 재초기화 */
	void ReInitializeAssociatedActors();
	
	/** 클라이언트 측 상호작용 종료 */
	void OnClientEndInteraction(AActor* Interactor);
	
	// ====================
	// 상태 관리
	// ====================
	
	/** 상호작용 가능 여부 설정 */
	void ToggleIsInteractable(bool Condition);
	
	/** 상호작용 위젯 표시 토글 */
	void SetInteractionWidgetVisible(bool Condition);
	
	/** 응답 타입에 따른 상호작용 제거 */
	void RemoveInteractionByResponse();
	
	/** 상호작용 값 변경 */
	void ChangeInteractableValue(bool Increment);
	
	/** 재초기화 가능 여부 설정 */
	void ToggleCanBeReInitialized(bool Condition);
	
	/** 위젯 로컬 소유자 설정 */
	void SetWidgetLocalOwner(APlayerController* OwningPlayer);
	
	UFUNCTION(Server, Reliable)
	void Server_SetWidgetLocalOwner(APlayerController* OwningPlayer);
	
	// ====================
	// 입력 처리
	// ====================
	
	/** 홀딩 입력 상태 체크 */
	EOperationStatus HoldingInput();
	
	/** 매싱 입력 처리 */
	EOperationStatus MashingInput(int32 MashingCount);
	
	/** 매싱 진행도 리셋 */
	void ResetMashingProgress();
	
	/** 목표 상호작용 값 체크 */
	bool IsTargetInteractableValue();
	
	/** 입력 액션에서 눌린 키 가져오기 */
	bool GetPressedKeyByAction(UInputAction* Action, FKey& OutKey);
	
public:
	// ====================
	// 이동 가능 오브젝트 설정
	// ====================
	UPROPERTY(EditDefaultsOnly, Category="Movable")
	float TravelDuration;

	UPROPERTY(EditDefaultsOnly, Category="Movable|Timeline")
	float TimelinePosition;

	UPROPERTY(EditDefaultsOnly, Category="Movable|Timeline")
	TEnumAsByte<ETimelineDirection::Type> TimelineDirection;

	// ====================
	// 상호작용 설정
	// ====================
	UPROPERTY(EditDefaultsOnly, Category="Interactable")
	TObjectPtr<UInputAction> InteractionAction;
	
	UPROPERTY(EditDefaultsOnly, Category="Interactable")
	bool DestroyAfterPickup = false;
	
	UPROPERTY(EditDefaultsOnly, Category="Interactable")
	EInteractionInputType InputType;
	
	UPROPERTY(EditDefaultsOnly, Category="Interactable")
	EInteractionResponse InteractionResponse;
	
	UPROPERTY(EditDefaultsOnly, Category="Interactable")
	int32 InteractableValue;
	
	UPROPERTY(EditDefaultsOnly, Category="Interactable")
	int32 InteractableTargetValue;
	
	UPROPERTY(EditDefaultsOnly, Category="Interactable")
	int32 InteractableLimitValue;
	
	UPROPERTY(EditDefaultsOnly, Category="Interactable")
	bool CheckForAssociatedActors;
	
	UPROPERTY(EditDefaultsOnly, Category="Interactable")
	bool CanBeReInitialized;
	
	UPROPERTY(EditDefaultsOnly, Category="Interactable")
	bool RemoveAssociatedInteractableOnComplete;
	
	UPROPERTY(EditDefaultsOnly, Category="Interactable")
	bool bIsInteractable = true;
	
	// ====================
	// 입력 타이밍 설정
	// ====================
	UPROPERTY(EditDefaultsOnly, Category="Interactable|Input")
	float MaxKeyTimeDown = 1.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Interactable|Input")
	float DurationInputDelay = 0.05f;
	
	UPROPERTY(EditDefaultsOnly, Category="Interactable|Input")
	float MashingKeyRetriggerableTime = 0.2f;
	
	// ====================
	// UI 설정
	// ====================
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UInteractionPromptWidget> InteractionWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	bool bShowLootBar;

	// ====================
	// 런타임 데이터 (GC 관리용)
	// ====================
	UPROPERTY()
	TArray<UPrimitiveComponent*> ObjectsToHighlight;
	
	UPROPERTY()
	TMap<AActor*, int32> AssociatedInteractableActors;
	
	UPROPERTY()
	TObjectPtr<UBoxComponent> InteractableArea;
	
	UPROPERTY()
	TObjectPtr<UWidgetComponent> InteractionWidget;
	
	UPROPERTY()
	TObjectPtr<UInteractionPromptWidget> InteractionWidgetRef;
	
	UPROPERTY()
	TObjectPtr<AActor> CurrentInteractor;
	
	// ====================
	// 런타임 상태 (UPROPERTY 불필요)
	// ====================
	FTimerHandle KeyDownTimer;
	FTimerHandle MashingTimer;
	FKey PressedInteractionKey;
	float CurrentMashingValue = 0.f;
	bool AlreadyInteracted = false;
	FText DefaultInteractionText;
};
