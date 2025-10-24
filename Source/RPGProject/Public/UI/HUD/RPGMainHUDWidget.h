// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Slate/WidgetTransform.h"
#include "Blueprint/UserWidget.h"
#include "UI/Minimap/MinimapWidget.h"
#include "RPGMainHUDWidget.generated.h"

class UCanvasPanel;
class ULootBarWidget;
class UInteractionPromptWidget;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API URPGMainHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	// virtual void NativeOnInitialized() override;
protected:

	// UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Widget")
	// TObjectPtr<UCompassWidget> CompassWidget;

	UPROPERTY(BlueprintReadOnly, meta=(BindWidget), Category="Widget")
	TObjectPtr<UMinimapWidget> MinimapWidget;
	
	UPROPERTY(BlueprintReadOnly, Category="Widget") // meta=(BindWidget),
	TObjectPtr<ULootBarWidget> LootBarWidget;

	UPROPERTY(BlueprintReadOnly, Category="Widget") // meta=(BindWidget),
	TObjectPtr<UCanvasPanel> CanvasPanel_InitMousePos;
	//UPROPERTY()
	//UCrosshairWidget
	
public:
	void SetMousePositionOnLootBar();
	void ShowLootBarWidget();
	void HideLootBarWidget();
	
#pragma region Minimap_Functions

	UFUNCTION(Category=Minimap)
	void InitMinimapVariables();
	
	UFUNCTION(Category=Minimap)
	void ShowMinimap(int32 MinimapIndex = 0) const;

	// UFUNCTION(Category=Minimap)
	// void HideMinimap() const { }

	// UFUNCTION(Category=Minimap)
	// void UpdateMinimap() const {};

	// UFUNCTION(Category=Minimap)
	// void AddOrUpdateMinimapMarker(UObject* MarkerObject, FMinimapMarkerInfo MarkerInfo) const;

	// UFUNCTION(Category=Minimap)
	// void RemoveMinimapMarker(UObject* MarkerObject) const;

	// UFUNCTION(Category=Minimap)
	// void RemoveAllMinimapMarkers() const;
#pragma endregion 
#pragma region Compass_Function

	UFUNCTION(Category=Compass)
	void InitCompassVariables();
	// UFUNCTION(Category=Compass)
	// void ShowCompass() const { }

	// UFUNCTION(Category=Compass)
	// void HidCompass() const { }

	// UFUNCTION(Category=Compass)
	// void RemoveAllCompassMarkers();

	// UFUNCTION(Category=Compass)
	// void AddOrUpdateCompassMarker(UObject* MarkerObject, FCompassMarkerInfo MarkerInfo);

	// UFUNCTION(Category=Compass)
	// void RemoveCompassMarker(UObject* MarkerObject);	
#pragma endregion 
#pragma region WorldMarker_Function

	// UFUNCTION(Category=WorldMarker)
	// void AddOrUpdateWorldMarker(UObject* MarkerObject, FWorldMarkerInfo MarkerInfo);
	// UFUNCTION(Category=WorldMarker)
	// void RemoveWorldMarker(UObject* MarkerObject);
	// UFUNCTION(Category=WorldMarker)
	// void RemoveAllWorldMarkers();
	
#pragma endregion
	
#pragma region Crosshair_Function
	
	// UFUNCTION(Category=Crosshair)
	// void AddCrosshair(ECrosshairType Type, FCrosshairInfo Info) const;
	// UFUNCTION(Category=Crosshair)
	// bool UpdateCrosshair(bool UseMovementStrength) const;
	// UFUNCTION(Category=Crosshair)
	// void RemoveCrosshair();
	// UFUNCTION(Category=Crosshair)
	// void HitFeedback(float Time, FLinearColor CrosshairColor, EHitFeedbackType Type, FLinearColor FeedbackColor, float FeedbackSpeed, float FeedbackStrength);
	// UFUNCTION(Category=Crosshair)
	// void Recoil(float Speed, float Strength);
#pragma endregion 

	
protected:
#pragma region HUD_Properties


	bool IsFirstPerson;
	
#pragma endregion 

#pragma region Compass_Properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Compass | Properties")
	TObjectPtr<UTexture2D> Compass_BlendMask;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Compass | Properties")
	TObjectPtr<UTexture2D> Compass_Texture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Compass | Properties")
	FLinearColor Compass_Tint{1.f, 1.f, 1.f, 1.f};

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Compass | Properties")
	float Compass_Direction = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Compass | Background")
	TObjectPtr<UObject> Compass_BackgroundTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Compass | Background")
	FWidgetTransform Compass_BackgroundTransform;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Compass | Background")
	float Compass_BackgroundOpacity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Compass | Background")
	FLinearColor Compass_BackgroundColor;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Compass | Pointer")
	TObjectPtr<UObject> Compass_PointerTexture;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Compass | Pointer")
	float Compass_PointerScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Compass | Pointer")
	float Compass_PointerYTranslation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Compass | Pointer")
	FLinearColor Compass_PointerTint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Compass | Marker")
	float Compass_MarkerYTranslation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Compass | Marker")
	float Compass_MarkerPrecision;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Compass | Marker")
	TObjectPtr<UObject> Compass_MarkerPingIcon1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Compass | Marker")
	TObjectPtr<UObject> Compass_MarkerPingIcon2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Compass | Marker")
	float Compass_MarkerPingPlaybackSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Compass | Marker")
	float Compass_MarkerPingDelay;
#pragma endregion

#pragma region Minimap_Properties
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Minimap | Properties")
	TSubclassOf<AActor> BottomLeftActor;
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Minimap | Properties")
	TSubclassOf<AActor> TopRightActor;
	UPROPERTY(BlueprintReadOnly, Category = "Minimap | Properties")
	UTexture2D* MinimapTexture;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap | Marker")
	bool Minimap_ScaleMarkerWithZoom;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap | Marker")
	FVector2D Minimap_MinMaxMarkerScale;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap | Marker")
	TObjectPtr<UObject> Minimap_DistantMarkerArrow;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap | Marker")
	bool Minimap_ScaleDistantMarkerWithZoom;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap | Marker")
	FVector2D Minimap_DistantMarkerMinMaxScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap | Properties")
	float Minimap_Size;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap | Properties")
	float Minimap_ZoomInterpSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap | Properties")
	bool Minimap_RotateWithPlayer;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap | Properties")
	TObjectPtr<UTexture2D> Minimap_BlendMask;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap | Properties")
	FLinearColor Minimap_Tint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap | Properties")
	TObjectPtr<UObject> Minimap_Frame;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap | Properties")
	FLinearColor Minimap_FrameTint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap | Properties")
	FVector2D Minimap_FrameScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap | Pointer")
	float Minimap_PointerSize;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap | Pointer")
	TObjectPtr<UObject> Minimap_PointerIcon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap | Pointer")
	FLinearColor Minimap_PointerColor;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap | Pointer")
	bool Minimap_ScalePointerWithZoom;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap | Pointer")
	FVector2D Minimap_PointerMinMaxScale;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Minimap | Propertie")
	TArray<FMiniMapInfo> Minimaps;

#pragma endregion

#pragma region Crosshair_Properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crosshair | Properties")
	float VelocitySensitivity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Crosshair | Properties")
	float SensitivityInterpSpeed;
#pragma endregion

#pragma region WorldMarker_Properties
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WorldMarker | Ping")
	TObjectPtr<UObject> PingIcon1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WorldMarker | Ping")
	TObjectPtr<UObject> PingIcon2;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WorldMarker | Ping")
	float PingPlaybackSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WorldMarker | Ping")
	float PingDelay;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WorldMarker | Properties")
	float WorldMarkerScreenLimitation;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WorldMarker | Properties")
	float SmoothVisibilityDivider;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WorldMarker | Properties")
	float DistanceVisibilityInterpSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WorldMarker | Properties")
	float PositionInterpSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WorldMarker | Properties")
	float PositionY_SwitchTolerance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WorldMarker | Properties")
	float ArrowVisibilityEdgeTolerance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WorldMarker | Properties")
	float ArrowVisibilityInterpSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WorldMarker | Properties")
	float TextVisibilityInterpSpeed;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WorldMarker | Properties")
	float TextVisibilityEdgeTolerance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WorldMarker | Properties")
	float TextVisibilityScreenCenterTolerance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WorldMarker | Properties")
	float MarkerNameVisibilityDuration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WorldMarker | Arrow Pointer")
	TObjectPtr<UObject> ArrowIcon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "WorldMarker | Arrow Pointer")
	float ArrowIconScale = 1.f;
#pragma endregion
};
