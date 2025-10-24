// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Blueprint/UserWidget.h"
#include "ItemStatsSlotWidget.generated.h"

class UWidgetSwitcher;
class UImage;
class UProgressBar;
class UTextBlock;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UItemStatsSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void ShowComparisonToolTip(float InComparisonValue);
	void HideComparisonToolTip();

	UFUNCTION(BlueprintPure)
	FText GetStatValue() const;

	UFUNCTION(BlueprintPure)
	FText GetComparisonValue() const;

	UFUNCTION(BlueprintPure)
	FSlateColor GetComparisonColor() const;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Default")
	FText StatName;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Default")
	float StatValue;
    
protected:
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
    
	UPROPERTY(BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_ComparisonGreaterValue;

	UPROPERTY(BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_ComparisonLesserValue;

	UPROPERTY(BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_ComparisonEqualValue;
    
	UPROPERTY(BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_StatName;

	UPROPERTY(BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_StatValue;

	UPROPERTY(BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_Base;
    
	UPROPERTY(BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_Add;
    
	UPROPERTY(BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UImage> Image_UpArrow;
    
	UPROPERTY(BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UImage> Image_DownArrow;

	UPROPERTY(BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UImage> Image_ThumbIcon;
    
	UPROPERTY(BlueprintReadOnly, Category="Widget",meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher_CompValue;

private:
	void SetAppropriateComparisonBox() const;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category="Default", meta=(AllowPrivateAccess = "true"))
	float ComparisonValue;
    
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default", meta=(AllowPrivateAccess = "true"))
	bool bShowComparison;
};
