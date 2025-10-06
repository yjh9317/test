// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "UI/Enums/HUDEnum.h"
#include "CompassWidget.generated.h"

class UCompassMarkerWidget;
class URetainerBox;
class UBorder;

USTRUCT(BlueprintType)
struct FCompassMarkerInfo
{
	GENERATED_BODY()
	
	UPROPERTY()
	UObject* MarkerIcon;

	UPROPERTY()
	float MarkerScale;

	UPROPERTY()
	FLinearColor MarkerColor;

	UPROPERTY()
	FLinearColor HighlightColor;

	UPROPERTY()
	bool PingMarker;

	UPROPERTY()
	bool ShowMarkerDistance;

	UPROPERTY()
	int32 MarkerVisibilityRadius;

	UPROPERTY()
	bool MarkerVisibilitySmoothTransition;

	UPROPERTY()
	bool ScreenPersistance;
};

USTRUCT(BlueprintType)
struct FCompassInitData
{
	GENERATED_BODY()

	UPROPERTY()
	FLinearColor BackgroundColor;

	UPROPERTY()
	float BackgroundOpacity;

	UPROPERTY()
	UTexture2D* CompassMask;

	UPROPERTY()
	FLinearColor CompassTint;

	UPROPERTY()
	UObject* PointerTexture;

	UPROPERTY()
	float PointerScale;

	UPROPERTY()
	float PointerYTranslation;

	UPROPERTY()
	FLinearColor PointerTint;

	UPROPERTY()
	float MarkerYTranslation;

	UPROPERTY()
	UObject* BackgroundTexture;
	
	UPROPERTY()
	FWidgetTransform BackgroundTransform;
};


class UOverlay;
class USizeBox;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UCompassWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UCompassWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;
	virtual bool Initialize() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	void InitCompassData(const FCompassInitData& Data);
	
	UFUNCTION()
	float GetPlayerRotationNormalized(ERotationType _Type);
	
	void AddOrUpdateCompassMarker(UObject* MarkerObject, UObject* PingIcon1, UObject* PingIcon2);


protected:
	// UPROPERTY()
	// UBorder* B_Compass;


	bool IsActive;
	float Orientation;
	float RotationValue;

	UPROPERTY()
	TMap<UObject*,UCompassMarkerWidget*> MarkerMap;


private:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UImage* Image_Compass;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UImage* Image_CompassBackground;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UImage* Image_DirectionLines;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UImage* Image_Shine;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UImage* Image_SoftShine;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UImage* Image_Lighting;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	URetainerBox* RetainerBox_Compass;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	USizeBox* SizeBox_Pointer;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UImage* Image_Pointer;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, meta=(AllowPrivateAccess))
	UOverlay* Overlay_MarkerParent;
public:
	// Get Set
	FORCEINLINE UMaterialInstanceDynamic* GetCompassMaterial() const { return Image_DirectionLines->GetDynamicMaterial(); }

	void SetCompassOffset(float RotationNormalized, bool InvertCompass, float Orientation);
	void SetBlendMask(UTexture2D* InTexture);
	void SetCompassMask(UTexture2D* InTexture);
	void SetBackgroundColor(FColor InColor);
	void SetBackgroundOpacity(float InOpacity);
	void SetCompassTint(FLinearColor InColorAndOpacity);
	void SetPointerScale(float SizeScale);
	void SetPointerTranslation(FVector2D InTranslation);
	void SetPointerTint(FLinearColor InPointerTint);
	void SetMarkerTranslation(FVector2D InTranslation);
	void SetBackgroundTexture(UObject* InBrush);
	void SetBackgroundTransform(FWidgetTransform InTransform);
	void SetAllMarkersCanUpdate(bool InCanUpdate);
};
