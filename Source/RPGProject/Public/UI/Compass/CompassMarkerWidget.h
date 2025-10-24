// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CompassWidget.h"
#include "Blueprint/UserWidget.h"
#include "CompassMarkerWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UCompassMarkerWidget : public UUserWidget
{
	GENERATED_BODY()

	
	
public:
	UPROPERTY()
	TObjectPtr<UObject> MarkerObject;

	UPROPERTY()
	float MarkerPrecision = 1.f;

	UPROPERTY()
	FGeometry Geometry;

	UPROPERTY()
	bool IsFacingToObject = false;

	UPROPERTY()
	bool PingMarker = false;

	UPROPERTY()
	FVector2D RandomSideTranslation;

	UPROPERTY()
	bool ShowMarkerDistance;

	UPROPERTY()
	bool DistanceActive = false;

	UPROPERTY()
	bool CanUpdate = false;

	UPROPERTY()
	int32 MarkerVisibilityRadius = false;;

	UPROPERTY()
	bool MarkerVisibilitySmoothTransition = false;

	UPROPERTY()
	float DistanceVisibilityInterpSpeed = 5.f;

	UPROPERTY()
	float SmoothVisibilityDivider = 1.5f;

	UPROPERTY()
	bool UseDistanceVisibility = false;

	UPROPERTY()
	FCompassMarkerInfo MarkerInfo;

	UPROPERTY()
	TObjectPtr<UCompassWidget> Parent;

	UPROPERTY()
	bool ScreenPersistance = false;

	UPROPERTY()
	float PingPlaybackSpeed = 3.f;

	UPROPERTY()
	float PingDelay = 2.f;

	
};
