// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ObjectInteraction.generated.h"

class UInteractableComponent;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UObjectInteraction : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGPROJECT_API IObjectInteraction
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * 오브젝트와의 상호작용을 시작합니다
	 * @param InteractableComponent 상호작용할 대상 컴포넌트
	 */
	virtual void StartInteractionWithObject(UInteractableComponent* InteractableComponent) = 0;
	
	/**
	 * 오브젝트와의 상호작용을 종료합니다
	 * @param InteractableComponent 상호작용을 종료할 대상 컴포넌트
	 */
	virtual void EndInteractionWithObject(UInteractableComponent* InteractableComponent) = 0;
	
	/**
	 * 오브젝트와의 상호작용을 제거합니다 (강제 종료)
	 * @param InteractableComponent 상호작용을 제거할 대상 컴포넌트
	 */
	virtual void RemoveInteractionWithObject(UInteractableComponent* InteractableComponent) = 0;
	
	/**
	 * 오브젝트와의 상호작용을 초기화합니다 (상호작용 전 준비 단계)
	 * @param InteractableComponent 초기화할 대상 컴포넌트
	 */
	virtual void InitializeInteractionWithObject(UInteractableComponent* InteractableComponent) = 0;
	
	/**
	 * 현재 상호작용 중인 오브젝트를 반환합니다
	 * @return 현재 상호작용 중인 액터, 없으면 nullptr
	 */
	virtual AActor* GetCurrentInteractableObject() = 0;
};
