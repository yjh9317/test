// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/Items/Interaction/ItemInteractionWidget.h"
#include "Components/Image.h"
#include "Interaction/Components/InteractableComponent.h"
#include "Interaction/Data/InteractableDefinitions.h"
#include "Materials/MaterialInstanceDynamic.h"

void UItemInteractionWidget::NativeConstruct()
{
	SetAppropriateFillingBackground();

	if(Image_FillBorder)
	{
		switch (InputType)
		{
		case EInteractionInputType::Single:
			Image_FillBorder->SetVisibility(ESlateVisibility::Hidden);	
		case EInteractionInputType::Holding:
			Image_FillBorder->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		case EInteractionInputType::MultipleAndMashing:
			Image_FillBorder->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			break;
		}
	}
	SetFillDecimalValue(0.05f);

	if(FillAnimOpacity)
	{
		PlayAnimation(FillAnimOpacity);
	}

	if(InteractableComponent)
	{
		InteractableComponent->OnUpdateMashingValue.AddDynamic(this,&UItemInteractionWidget::BorderFill);
		InteractableComponent->OnUpdateHoldingValue.AddDynamic(this,&UItemInteractionWidget::BorderFill);
	}
}

void UItemInteractionWidget::NativeDestruct()
{
	if(InteractableComponent)
	{
		InteractableComponent->OnUpdateMashingValue.RemoveDynamic(this,&UItemInteractionWidget::BorderFill);
		InteractableComponent->OnUpdateHoldingValue.RemoveDynamic(this,&UItemInteractionWidget::BorderFill);
	}
}

void UItemInteractionWidget::BorderFill(float Value)
{
	if(FillAnimOpacity && Image_FillBorder)
	{
		StopAnimation(FillAnimOpacity);
		Image_FillBorder->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		SetFillDecimalValue(Value);
	}
}

void UItemInteractionWidget::SetInputIconAsync()
{
	
}


void UItemInteractionWidget::SetFillDecimalValue(float Value)
{
	if(Image_FillBorder)
	{
		UMaterialInstanceDynamic* DynamicMaterial = Image_FillBorder->GetDynamicMaterial();
		if(DynamicMaterial)
		{
			const float ClampValue = FMath::Clamp(Value,0.05f,1.f);
			DynamicMaterial->SetScalarParameterValue("Decimal",ClampValue);
		}
	}
}

void UItemInteractionWidget::SetAppropriateFillingBackground()
{
	if(Image_FillBorder && BorderMaterial)
	{
		Image_FillBorder->SetBrushFromMaterial(BorderMaterial.GetDefaultObject());
	}
}

FText UItemInteractionWidget::GetInteractionText() const
{
	return Text;
}