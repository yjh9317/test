// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/Data/InventoryDatas.h"
#include "SortSlotWidget.generated.h"

class UTextBlock;
class UButton;
class UBorder;
/**
 * 
 */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSortButtonClicked,ESortMethod SortMethod);

UCLASS()
class RPGPROJECT_API USortSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	FOnSortButtonClicked OnSortButtonClicked;

protected:
	virtual void NativePreConstruct() override;
	virtual void NativeOnAddedToFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnRemovedFromFocusPath(const FFocusEvent& InFocusEvent) override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;

	UFUNCTION()
	void EventOnSortButtonClick();

	UFUNCTION()
	void EventOnSortButtonHover();
    
	UFUNCTION()
	void EventOnSortButtonUnHover();

	UPROPERTY(BlueprintReadOnly, Category="Widget", meta=(BindWidget))
	TObjectPtr<UBorder> Border_Background;

	UPROPERTY(BlueprintReadOnly, Category="Widget", meta=(BindWidget))
	TObjectPtr<UButton> Button_Background;

	UPROPERTY(BlueprintReadOnly, Category="Widget", meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_TitleText;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables", meta=(AllowPrivateAccess = "true"))
	ESortMethod SortMethod;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Variables", meta=(AllowPrivateAccess = "true"))
	FText Title;
};
