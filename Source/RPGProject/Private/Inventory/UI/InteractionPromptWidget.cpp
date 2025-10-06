// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/InteractionPromptWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Header/DebugHeader.h"
#include "Interaction/Components/InteractableComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

TObjectPtr<UMaterialInterface> UInteractionPromptWidget::SharedBorderMaterial = nullptr;

UInteractionPromptWidget::UInteractionPromptWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	static bool bIsMaterialFound = false;
	if (!bIsMaterialFound)
	{
		static ConstructorHelpers::FObjectFinder<UMaterialInterface> BorderMaterialFinder(
			TEXT("/Game/Blueprint/Material/MI_SquareFill.MI_SquareFill")
		);

		if (BorderMaterialFinder.Succeeded())
		{
			SharedBorderMaterial = BorderMaterialFinder.Object;
		}
		
		bIsMaterialFound = true;
	}
}

void UInteractionPromptWidget::NativeConstruct()
{
	if (Image_FillBorder && SharedBorderMaterial)
	{
		Image_FillBorder->SetBrushFromMaterial(SharedBorderMaterial);
	}
	
	if(InteractableComponent)
	{
		InteractableComponent->OnUpdateHoldingValue.AddDynamic(this,&UInteractionPromptWidget::BorderFill);
		InteractableComponent->OnUpdateMashingValue.AddDynamic(this,&UInteractionPromptWidget::BorderFill);
	}
	
	switch (InputType)
	{
	case EInteractionInputType::Single:
		Image_FillBorder->SetVisibility(ESlateVisibility::Hidden);
		break;
	case EInteractionInputType::Holding:
		Image_FillBorder->SetVisibility(ESlateVisibility::HitTestInvisible);
		break;
	case EInteractionInputType::MultipleAndMashing:
		Image_FillBorder->SetVisibility(ESlateVisibility::HitTestInvisible);
		break;
	}

	SetFillDecimalValue(0.05f);

	TextBlock_Description->SetText(Description);
}

void UInteractionPromptWidget::NativeDestruct()
{
	if(InteractableComponent)
	{
		InteractableComponent->OnUpdateHoldingValue.RemoveDynamic(this,&UInteractionPromptWidget::BorderFill);
		InteractableComponent->OnUpdateMashingValue.RemoveDynamic(this,&UInteractionPromptWidget::BorderFill);
	}
}

void UInteractionPromptWidget::SetFillDecimalValue_Implementation(float Value)
{
	UMaterialInstanceDynamic* MID = Image_FillBorder->GetDynamicMaterial();
	if(MID)
	{
		float ClampedValue = FMath::Clamp(Value,0.05f,1.f);
		MID->SetScalarParameterValue("Decimal",ClampedValue);
	}
}

void UInteractionPromptWidget::BorderFill_Implementation(float Value)
{
	Image_FillBorder->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
	SetFillDecimalValue(Value);
}
