// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ItemInteractionWidget.generated.h"

enum class EInteractionInputType;
class UInteractableComponent;
class UWidgetComponent;
class UImage;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UItemInteractionWidget : public UUserWidget
{
	GENERATED_BODY()


	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void BorderFill(float Value);
	void SetInputIconAsync();

	FText GetInteractionText() const;
	void SetFillDecimalValue(float Value);
	void SetAppropriateFillingBackground();


	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWidgetAnimation> FillAnimOpacity;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMaterialInterface> BorderMaterial;

	UPROPERTY()
	TObjectPtr<UWidgetComponent> InteractionWidget;
	UPROPERTY()
	TObjectPtr<UInteractableComponent> InteractableComponent;
	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_Input;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_FillBorder;

	UPROPERTY(EditDefaultsOnly)
	FText Text;

	UPROPERTY(EditDefaultsOnly)
	EInteractionInputType InputType;
};
