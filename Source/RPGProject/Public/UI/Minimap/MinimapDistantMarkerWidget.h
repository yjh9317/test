// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinimapDistantMarkerWidget.generated.h"


class UTextBlock;
class UBorder;

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UMinimapDistantMarkerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UMinimapDistantMarkerWidget(const FObjectInitializer& ObjectInitializer);
// 	void UpdateDistantMarker(bool _IsMarkerInMapRadius,float _ZoomValue,float _ZoomDivider);
// 	void ShowMarker();
// 	void HideMarker();
// 	
// 	bool IsFirstPerson;
// 	bool RotateMinimapWithPlayer;
// 	bool ScaleDistantMarkerWithZoom;
// 	FVector2D DistantMarkerMinMaxScale;
//
// 	
// private:
// 	// Widget
// 	UPROPERTY() // meta= (BindWidget)
// 	UTextBlock* DistanceValue;
// 	UPROPERTY()
// 	UBorder* MarkerVisibility;
// 	// Animation
// 	UPROPERTY(Category="Animation")
// 	UWidgetAnimation* Fade;
// 	// Distance
// 	UPROPERTY(Category="Distance")
// 	TEnumAsByte<EDistanceUnit> DistanceUnit = EDistanceUnit::Miles;
// 	UPROPERTY(Category="Distance")
// 	int32 MarkerDistanceYards;
// 	UPROPERTY(Category="Distance")
// 	int32 MarkerDistanceMeters;
// 	UPROPERTY(Category="Distance")
// 	bool ShowMarkerDistance = true;
// 	UPROPERTY(Category="Distance")
// 	bool IsConstructing;
// 	UPROPERTY(Category="Distance")
// 	int32 DistanceStepsYa;
// 	UPROPERTY(Category="Distance")
// 	int32 DistanceStepsKM;
// 	UPROPERTY(Category="Distance")
// 	int32 DistanceStepsM;
// 	UPROPERTY(Category="Distance")
// 	int32 DistanceStepsMi;
//
// 	FDistanceInfo DistanceInfo;
// 	FDistantMarkerInfo DistantMarkerInfo;
// 	bool IsActive;
// 	UPROPERTY()
// 	UObject* MarkerObject;
// 	FLinearColor MarkerColor;
// 	UPROPERTY()
// 	UObject* Icon;
// 	bool HideMarkerSmoothly;
// 	bool UseDistantMarkerArrow;
// 	int32 TextVisibilityRadius;
// 	int32 HideMarkerAtDistance;
// 	bool IsVisible;
// 	float ZoomDivider;
// 	float ZoomValue;
// 	float  Tolerance;
// 	bool IsMarkerInMapRadius;
// 	int32 MapBorderDistance;
//
// 	UPROPERTY()
// 	UWidgetAnimation* WidgetAnimation;
// 	void PlayWidgetAnimation(UWidgetAnimation* WidgetAnim, bool FadeIn, bool Loop, float PlaybackSpeed);
// 	void AnimFade(bool FadeIn, float PlaybackSpeed);
//
// private:
// 	UFUNCTION(BlueprintPure, Category= "Distance")
// 	int32 GetYardDistanceToMarker();
// 	UFUNCTION(BlueprintPure, Category= "Distance")
// 	int32 GetMeterDistanceToMarker();
// 	UFUNCTION(BlueprintPure, Category= "Distance")
// 	FString GetDistanceValueFromText();
// 	UFUNCTION(BlueprintPure, Category= "Distance")
// 	bool IsInStepRange(int32 Value, int32 Steps);
// 	UFUNCTION(BlueprintPure, Category= "Distance")
// 	FString FindYards();
// 	UFUNCTION(BlueprintPure, Category= "Distance")
// 	FString GetDigitFromInteger(int32 Integer, int32 Index);
// 	UFUNCTION(BlueprintPure, Category= "Distance")
// 	FString FindUnder10MiInt();
// 	UFUNCTION(BlueprintPure, Category= "Distance")
// 	FString FindOver10MiInt();
// 	UFUNCTION(BlueprintPure, Category= "Distance")
// 	FString FindMeters();
// 	UFUNCTION(BlueprintPure, Category= "Distance")
// 	FString FindUnder10KmInt();
// 	UFUNCTION(BlueprintPure, Category= "Distance")
// 	FString FindOver10KmInt();
// 	UFUNCTION(BlueprintPure, Category= "Distance")
// 	FString FindKilometers();
// 	UFUNCTION(BlueprintPure, Category= "Distance")
// 	FString FindMiles();
// 	UFUNCTION(BlueprintCallable, Category= "Distance")
// 	void SetDistanceText(FString InString);
//
//
// 	UFUNCTION(BlueprintCallable, Category= "Utility")
// 	void InterpolateRenderOpacity(UUserWidget* Input, float Target, float InterpSpeed);
//
// 	UFUNCTION(BlueprintPure, Category= "Utility")
// 	APawn* GetPlayerPawn() const;
// 	UFUNCTION(BlueprintPure, Category= "Utility")
// 	FLinearColor FindDistantMarkerColor();
// 	UFUNCTION(BlueprintPure, Category= "Utility")
// 	UObject* FindDistantMarkerIcon();
// 	UFUNCTION(BlueprintPure, Category= "Utility")
// 	int32 FindMarkerTypeIndex();
// 	UFUNCTION(BlueprintPure, Category= "Utility")
// 	FVector2D FindZoomScaleValue();
// 	UFUNCTION(BlueprintPure, Category= "Utility")
// 	FLinearColor FindColor(FLinearColor InColor, FLinearColor StylesheetColor);
//
// 	UFUNCTION(BlueprintPure, Category= "Location")
// 	FVector GetMarkerLocation() const;
// 	
// 	UFUNCTION(BlueprintPure, Category= "Location")
// 	FVector GetPlayerLocation() const;
// 	
// 	UFUNCTION(BlueprintPure, Category= "Location")
// 	float GetPlayerDistanceToMarker() const;
// 	
// 	UFUNCTION(BlueprintPure, Category= "Location")
// 	int32 GetPlayerDistanceToMarkerInM();
// 	
// 	UFUNCTION(BlueprintPure, Category= "Utility")
// 	FTextInfo GetDistanceTextInfo();
// 	UFUNCTION(BlueprintPure, Category= "Utility")
// 	FLinearColor FindDistanceTextColor(FLinearColor StylesheetColor);
//
// 	//
// 	void HandleRenderOpacity(float InterpSpeedIcon, float InterpSpeedText);
};


