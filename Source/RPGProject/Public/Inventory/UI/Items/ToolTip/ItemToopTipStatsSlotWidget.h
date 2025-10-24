// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemToopTipStatsSlotWidget.generated.h"

class UWidgetSwitcher;
class UTextBlock;
class UProgressBar;
class UImage;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UItemToopTipStatsSlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Default") // ,meta=(BindWidget)
	TObjectPtr<UImage> Image_Bullet;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Bind Widget") // ,meta=(BindWidget)
	TObjectPtr<UImage> Image_GreenArrow;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Bind Widget") // ,meta=(BindWidget)
	TObjectPtr<UImage> Image_RedArrow;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Bind Widget") // ,meta=(BindWidget)
	TObjectPtr<UImage> Image_GrayArrow;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Bind Widget") // ,meta=(BindWidget)
	TObjectPtr<UProgressBar> ProgressBar_Base;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Bind Widget") // ,meta=(BindWidget)
	TObjectPtr<UProgressBar> ProgressBar_Added;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Bind Widget") // ,meta=(BindWidget)
	TObjectPtr<UTextBlock> TextBlock_StatName;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Bind Widget") // ,meta=(BindWidget)
	TObjectPtr<UTextBlock> TextBlock_StatValue;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,Category="Bind Widget") // ,meta=(BindWidget)
	TObjectPtr<UWidgetSwitcher> WidgetSwitcher;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Variable")
	FText StatName;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Variable")
	float StatValue;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Variable")
	float ComparisonValue;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly,Category="Variable")
	bool bShowComparison;
};
