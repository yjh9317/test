// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interface/WidgetManager.h"
#include "Player/Character/MainCharacter.h"
#include "PlayerWidgetComponent.generated.h"

class AMainPlayerController;
class AMainCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSwitchedDelegate, EWidgetType, Param);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UPlayerWidgetComponent : public UActorComponent
{
	GENERATED_BODY()


public:	
	// Sets default values for this component's properties
	UPlayerWidgetComponent();
	
	FOnSwitchedDelegate OnSwitchedWidget;
	FOnSwitchedDelegate OnSwitchedTab;
	
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY()
	TObjectPtr<AMainCharacter> MainCharacter;
	UPROPERTY()
	TObjectPtr<AMainPlayerController> MainPlayerController;

	EWidgetType ActiveWidget;
	EWidgetType ActiveTab;
	EWidgetPopup ActivePopup;
	UPROPERTY()
	TObjectPtr<UUserWidget> CurrentMessageWidget;
	UPROPERTY()
	TObjectPtr<UUserWidget> CurrentPopupWidget;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UUserWidget> MessageClass;
public:
	void InitializeComponentData(AMainPlayerController* PC, AMainCharacter* Character);
	void SetNewActiveWidget(EWidgetType NewType);
	void SetNewActiveTab(EWidgetType NewType);
	void SetNewActivePopup(UUserWidget* NewWidget, EWidgetPopup NewType);
	void CloseActivePopup();
	void ClearCurrentMessage();
	void DisplayMessagePopup(const FText& Message);
	
	FORCEINLINE EWidgetPopup GetActivePopupType() const { return ActivePopup;}
	FORCEINLINE UUserWidget* GetActivePopupWidget() const { return CurrentPopupWidget;}
	FORCEINLINE EWidgetType GetActiveWidget() const { return ActiveWidget;}
	FORCEINLINE EWidgetType GetActiveTab() const { return ActiveTab;}
};
