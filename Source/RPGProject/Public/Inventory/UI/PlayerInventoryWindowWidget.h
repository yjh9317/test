// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerInventoryWindowWidget.generated.h"

class UPlayerInventoryWidget;
class UEquipmentWidget;
class UInventoryComponent;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UPlayerInventoryWindowWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void PlayConstructAnim();

protected:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnAnalogValueChanged(const FGeometry& InGeometry, const FAnalogInputEvent& InAnalogEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

private:
	UPROPERTY(BlueprintReadOnly, Category="Component", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInventoryComponent> PlayerInventory;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Widget", meta=(BindWidget, AllowPrivateAccess="true"))
	TObjectPtr<UEquipmentWidget> EquipmentWidget;
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Widget", meta=(BindWidget, AllowPrivateAccess="true"))
	TObjectPtr<UPlayerInventoryWidget> PlayerInventoryWidget;
};
