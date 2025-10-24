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
	UFUNCTION(BlueprintPure, Category="Interaction")
	FText GetInteractionText() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
	TObjectPtr<UWidgetComponent> InteractionWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Setup", meta=(ExposeOnSpawn="true"))
	TObjectPtr<UInteractableComponent> InteractableComponent;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void BorderFill(float Value);

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	TObjectPtr<UWidgetAnimation> FillAnimOpacity;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	TObjectPtr<UImage> Image_Input;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly)
	TObjectPtr<UImage> Image_FillBorder;

private:
	void SetInputIconAsync();
	void SetFillDecimalValue(float Value);
	void SetAppropriateFillingBackground();

	UPROPERTY(EditDefaultsOnly, Category="Config", meta=(AllowPrivateAccess="true"))
	TSubclassOf<UMaterialInterface> BorderMaterial;

	UPROPERTY(EditDefaultsOnly, Category="Config", meta=(AllowPrivateAccess="true"))
	FText Text;

	UPROPERTY(EditDefaultsOnly, Category="Config", meta=(AllowPrivateAccess="true"))
	EInteractionInputType InputType;
};
