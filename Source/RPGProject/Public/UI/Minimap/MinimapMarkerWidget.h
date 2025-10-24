// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MinimapWidget.h"
#include "Blueprint/UserWidget.h"
#include "MinimapMarkerWidget.generated.h"

USTRUCT(BlueprintType)
struct FMinimapMarkerInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UObject* MarkerIcon;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FLinearColor MarkerColor;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	float MarkerScale;
};
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UMinimapMarkerWidget : public UUserWidget
{
	GENERATED_BODY()

	
public:
	UMinimapMarkerWidget(const FObjectInitializer& ObjectInitializer);
	// UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	// UObject* MarkerObject;
	//
	// UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	// UMinimapWidget* MinimapWidget;
	//
	// UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	// UMinimapMarkerWidget* MinimapMarkerWidget;
	//
	// UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	
	float TextureMapSize;
	float MinimapSize;
	FVector2D MinMaxScale;
	float ZoomDivide;
	FMiniMapInfo MiniMapInfo;
	
	
};
