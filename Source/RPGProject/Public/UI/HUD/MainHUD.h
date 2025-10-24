// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Interface/InteractableInterface.h"
#include "MainHUD.generated.h"

struct FInteractableData;
class URPGMainHUDWidget;
class UMainMenu;
class UInteractionWidget;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API AMainHUD : public AHUD
{
	GENERATED_BODY()

public:
	// ─────────────────────────────────────────────────────────────────────
	//	Properties & Variables
	// ─────────────────────────────────────────────────────────────────────
	
	
	UPROPERTY(EditDefaultsOnly, Category= "Widgets")
	TSubclassOf<UUserWidget> CharacterOverlayClass;

	UPROPERTY()
	UUserWidget* CharacterOverlay;
	
	UPROPERTY(EditDefaultsOnly, Category= "Widgets")
	TSubclassOf<UMainMenu> MainMenuClass;

	// UPROPERTY(EditDefaultsOnly, Category= "Widgets")
	// TSubclassOf<UInteractionWidget> InteractionWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category= "Widgets")
	TSubclassOf<URPGMainHUDWidget> RPGMainHUDWidgetClass;

	// ─────────────────────────────────────────────────────────────────────
	//	Function
	// ─────────────────────────────────────────────────────────────────────
	AMainHUD();
	void InitWidgets();

	void SetMenuVisibility(bool IsVisible);
	void SetInteractionWidgetVisibility(bool IsVisible) const;
	void UpdateInteractionWidget(const FInteractableData* InteractableData) const;


protected:
	// ─────────────────────────────────────────────────────────────────────
	//	Properties & Variables
	// ─────────────────────────────────────────────────────────────────────
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UMainMenu> MainMenuWidget;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UInteractionWidget> InteractionWidget;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UUserWidget> StatsWindowWidget;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TObjectPtr<URPGMainHUDWidget> RPGMainHUDWidget;
	
	// ─────────────────────────────────────────────────────────────────────
	//	Function
	// ─────────────────────────────────────────────────────────────────────
	virtual void BeginPlay() override;

private:
	
};
