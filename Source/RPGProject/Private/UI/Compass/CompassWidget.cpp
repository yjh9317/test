// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Compass/CompassWidget.h"
#include "UI/Compass/CompassMarkerWidget.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/RetainerBox.h"
#include "Components/SizeBox.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UI/FunctionLibrary/UIUtilityLibrary.h"

UCompassWidget::UCompassWidget(const FObjectInitializer& ObjectInitializer)
    :Super(ObjectInitializer)
{
}

void UCompassWidget::NativePreConstruct()
{
    if(GetCompassMaterial())
    {
        // UKismetMathLibrary::MapRangeClamped(
    }
}

void UCompassWidget::NativeConstruct()
{
    Super::NativeConstruct();
}

bool UCompassWidget::Initialize()
{
    // if(IsActive)  B_Compass->SetRenderOpacity(1.f);
    // else  B_Compass->SetRenderOpacity(0);
    
    return UUserWidget::Initialize();
}

void UCompassWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    UUserWidget::NativeTick(MyGeometry, InDeltaTime);

    if(IsActive)
    {
        
    }
}

void UCompassWidget::InitCompassData(const FCompassInitData& Data)
{
    
}


#pragma region CompassFunction

void UCompassWidget::AddOrUpdateCompassMarker(UObject* MarkerObject, UObject* PingIcon1, UObject* PingIcon2)
{
}

void UCompassWidget::SetCompassOffset(float _RotationNormalized, bool _InvertCompass, float _Orientation)
{
    float Value = 0.f;
    if(_InvertCompass)
    {
        Value = UKismetMathLibrary::MapRangeClamped(_RotationNormalized,-1.f,1.f,-5.f,5.f);
    }
    else
    {
        Value = UKismetMathLibrary::MapRangeClamped(_RotationNormalized,1.f,-1.f,-5.f,5.f);
    }

    if(UMaterialInstanceDynamic* MID = GetCompassMaterial())
    {
        MID->SetScalarParameterValue("Offset",Value);
        Value = UKismetMathLibrary::MapRangeClamped(_Orientation, 0.f,1.f,-0.5f,0.5f);
        MID->SetScalarParameterValue("Orientation",Value);
    }
    
}

float UCompassWidget::GetPlayerRotationNormalized(ERotationType _Type)
{
    float Value = 0.f;
    float Yaw = 0.f;
    switch (_Type)
    {
    case ERotationType::ControlRotation:
        {
            Yaw = GetOwningPlayerPawn()->GetControlRotation().Yaw;
            Value = UKismetMathLibrary::MapRangeClamped(Yaw,0.f,360.f,-1.f,1.f);
            break;
        }
    case ERotationType::CameraRotation:
        {
            Yaw = GetOwningPlayerCameraManager()->GetCameraRotation().Yaw;
            Value = UKismetMathLibrary::MapRangeClamped(Yaw,-180.f,180.f,-1.f,1.f);
            break;
        }
    }
    return Value;
}


#pragma endregion

#pragma region GetSet

void UCompassWidget::SetBlendMask(UTexture2D* InTexture)
{
    if(UMaterialInstanceDynamic* MID = RetainerBox_Compass->GetEffectMaterial())
    {
        MID->SetTextureParameterValue("Mask",InTexture);
    }
}

void UCompassWidget::SetCompassMask(UTexture2D* InTexture)
{
    if(UMaterialInstanceDynamic* MID = GetCompassMaterial())
    {
        MID->SetTextureParameterValue("CompassMask",InTexture);
    }
}

void UCompassWidget::SetBackgroundColor(FColor InColor)
{
    Image_CompassBackground->SetColorAndOpacity(InColor);
}

void UCompassWidget::SetBackgroundOpacity(float InOpacity)
{
    Image_CompassBackground->SetRenderOpacity(InOpacity);
}

void UCompassWidget::SetCompassTint(FLinearColor InColorAndOpacity)
{
    Image_DirectionLines->SetColorAndOpacity(InColorAndOpacity);
    Image_Shine->SetColorAndOpacity(InColorAndOpacity);
    Image_SoftShine->SetColorAndOpacity(InColorAndOpacity);
    Image_Lighting->SetColorAndOpacity(InColorAndOpacity);
}

void UCompassWidget::SetPointerScale(float SizeScale)
{
    float ScaledPointerSize = UKismetMathLibrary::MapRangeClamped(SizeScale,0.f,2.f,16.f,128.f);
    UUIUtilityLibrary::SetSizeBoxSize(SizeBox_Pointer,ScaledPointerSize,ScaledPointerSize);

    if(UTexture2D* Texture2D = Cast<UTexture2D>(Image_Pointer->Brush.GetResourceObject()))
    {
        FVector2D NewTextureSize = UUIUtilityLibrary::GetTexture2DSize(Texture2D);
        UUIUtilityLibrary::SetImageBrushSize(Image_Pointer,NewTextureSize);
    }
}

void UCompassWidget::SetPointerTranslation(FVector2D InTranslation)
{
    SizeBox_Pointer->SetRenderTranslation(InTranslation);
}

void UCompassWidget::SetPointerTint(FLinearColor InPointerTint)
{
    Image_Pointer->SetColorAndOpacity(InPointerTint);
}

void UCompassWidget::SetMarkerTranslation(FVector2D InTranslation)
{
    Overlay_MarkerParent->SetRenderTranslation(InTranslation);
}

void UCompassWidget::SetBackgroundTexture(UObject* InBrush)
{
    UUIUtilityLibrary::SetImageBrush(Image_CompassBackground,InBrush);
}

void UCompassWidget::SetBackgroundTransform(FWidgetTransform InTransform)
{
    Image_CompassBackground->SetRenderTransform(InTransform);
}

void UCompassWidget::SetAllMarkersCanUpdate(bool InCanUpdate)
{
    TArray<UCompassMarkerWidget*> MarkerWidgets;
    MarkerMap.GenerateValueArray(MarkerWidgets);
    
    for(auto& Widget : MarkerWidgets)
    {
        Widget->CanUpdate = InCanUpdate;
    }
}

#pragma endregion 
