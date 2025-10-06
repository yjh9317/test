// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FocusWidgetHandler.h"
#include "Item/Data/ItemDataStructs.h"
#include "GameFramework/HUD.h"
#include "UObject/Interface.h"
#include "WidgetManager.generated.h"


UENUM()
enum class EWidgetPopup
{
	None,
	SplitStackPopup,
	ConfirmationPopup,
	TextDocumentPopup
};

enum class EItemDestination : uint8;
enum class EInputMethod : uint8;
class UInventoryCoreComponent;
enum class EWidgetType : uint8;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UWidgetManager : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGPROJECT_API IWidgetManager
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void SetActiveWidget(EWidgetType Widget) {}
	virtual EWidgetType GetActiveWidget() const {return EWidgetType::NONE; }
	virtual void SetActiveTab(EWidgetType Tab) {}
	virtual EWidgetType GetActiveTab() const {return EWidgetType::NONE; }
	virtual void CloseActiveWidget() {}
	virtual void OpenNewWidget(EWidgetType Widget) {}
	virtual void SwitchWidgetTo(EWidgetType Widget) {}
	virtual void SwitchTabTo(EWidgetType NewTab) {}
	virtual void OpenSplitStackPopup(UInventoryCoreComponent* Sender,UInventoryCoreComponent* Receiver,FItemData ItemData, FItemData InSlotData,
		EInputMethod Method, EItemDestination Initiator, EItemDestination Destination, UUserWidget* SenderWidget) {}
	virtual void OpenConfirmationPopup(FText QuestionText,UInventoryCoreComponent* Sender,UInventoryCoreComponent* Receiver,FItemData ItemData, FItemData InSlotData,
		EInputMethod Method, EItemDestination Initiator, EItemDestination Destination, UUserWidget* SenderWidget) {}
	virtual void OpenTextDocumentPopup(const FItemData& ItemData,UUserWidget* SenderWidget) {};
	virtual void CloseActivePopup() {}
	virtual EWidgetPopup GetActivePopup() const{ return EWidgetPopup::None;}
	virtual void DisplayMessageNotify(FText Message) {};
	virtual AHUD* GetPlayerHUD() const {return nullptr;}

	virtual void StartPlayerCapture() {}
	virtual void StopPlayerCapture() {}
	virtual void AddPlayerCaptureRotation(FRotator Rotator) {}
};