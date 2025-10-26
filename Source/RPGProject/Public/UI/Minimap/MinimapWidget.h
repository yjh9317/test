// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinimapWidget.generated.h"

class UMinimapDistantMarkerWidget;
class UMinimapMarkerWidget;
class URetainerBox;
class UOverlay;
class UImage;
class UBorder;
class USizeBox;

USTRUCT(BlueprintType)
struct FMiniMapInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<AActor> BottomLeftActor{};
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FVector2D BottomLeftCoordinates{};
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	TSubclassOf<AActor> TopRightActor{};
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	FVector2D TopRightCoordinates {};
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
	UObject* MinimapTexture{};
};

USTRUCT(BlueprintType)
struct FMinimapInitData
{
	GENERATED_BODY()

	UPROPERTY()
	float Size{};

	UPROPERTY()
	FVector2D MinMaxMarkerScale{};

	UPROPERTY()
	float ZoomInterpSpeed{};

	UPROPERTY()
	UObject* PointerIcon{};

	UPROPERTY()
	FLinearColor PointerColor{};

	UPROPERTY()
	float PointerSize{};

	UPROPERTY()
	bool ScalePointerWithZoom{};

	UPROPERTY()
	FVector2D PointerMinMaxScale{};

	UPROPERTY()
	bool RotateMinimapWithPlayer{};
	
	UPROPERTY()
	bool IsFirstPerson{};

	UPROPERTY()
	FLinearColor MinimapTint{};
	
	UPROPERTY()
	UObject* MinimapFrame{};
	
	UPROPERTY()
	FLinearColor MinimapFrameTint{};
	
	UPROPERTY()
	FVector2D MinimapFrameScale{};
};

UCLASS()
class RPGPROJECT_API UMinimapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UMinimapWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	void InitMinimapData(const FMinimapInitData& Data);
public:

	void ShowMinimap(FMiniMapInfo Info);
	void HideMinimap();
	void UpdateMiniMap(float NewZoomValue);
	
	void AddOrUpdateMarker(UObject* Marker,bool ScaleWithZoom, FMiniMapInfo Info);
	void RemoveMarker(UObject* Marker);
	void RemoveAllMinimapMarker();

	// UFUNCTION(BlueprintImplementableEvent)
	// void OnAnimationFinishedCallback();
	
private:
	// Edit
	UPROPERTY(EditDefaultsOnly ,BlueprintReadOnly, meta = (AllowPrivateAccess))
	float MapSize;

	UPROPERTY(EditDefaultsOnly ,BlueprintReadOnly, meta = (AllowPrivateAccess))
	float ZoomInterpSpeed;

	UPROPERTY(EditDefaultsOnly ,BlueprintReadOnly, meta = (AllowPrivateAccess))
	float ZoomDivider;

	UPROPERTY(EditDefaultsOnly ,BlueprintReadOnly, meta = (AllowPrivateAccess))
	TSubclassOf<AActor> MinimapBottomLeftActor;

	UPROPERTY(EditDefaultsOnly ,BlueprintReadOnly, meta = (AllowPrivateAccess))
	TSubclassOf<AActor> MinimapTopRightActor;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool IsActive;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	float CurrentZoomValue; 

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	FMiniMapInfo MiniMapInfo;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	FVector2D BottomLeft;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	FVector2D BLCache;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	FVector2D TopRight;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	FVector2D TRCache;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	FVector2D PointerMinMaxScale;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	FVector2D MinMaxScale;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool ScalePointerWithZoom;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool RotateMinimapWithPlayer;
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess))
	bool IsFirstPerson;
	
protected:
	/* BindWidget */
	
	UPROPERTY(BlueprintReadOnly,meta=(BindWidget))
	UImage* Image_Pointer;

	UPROPERTY(BlueprintReadOnly,meta=(BindWidget))
	USizeBox* SizeBox_Content;
	
	UPROPERTY(BlueprintReadOnly,meta=(BindWidget))
	UImage* Image_Map;

	UPROPERTY(BlueprintReadOnly,meta=(BindWidget))
	UImage* Image_Frame;

	UPROPERTY(BlueprintReadOnly,meta=(BindWidget))
	UOverlay* Overlay_MapMarkers;

	UPROPERTY(BlueprintReadOnly,meta=(BindWidget))
	UOverlay* Overlay_DistantMarker;

	UPROPERTY(BlueprintReadOnly,meta=(BindWidget))
	UOverlay* Overlay_Map;

	UPROPERTY(BlueprintReadOnly,meta=(BindWidget))
	UOverlay* Overlay_PointerAnim;

	UPROPERTY(BlueprintReadOnly,meta=(BindWidget))
	UOverlay* Overlay_CanvasPanelMap;

	UPROPERTY(BlueprintReadOnly,meta=(BindWidget))
	UBorder* Border_MapMarkers;

	UPROPERTY(BlueprintReadOnly,meta=(BindWidget))
	USizeBox* SizeBox_MapMarkers;

	UPROPERTY(BlueprintReadOnly,meta=(BindWidget))
	URetainerBox* RetainerBox_Minimap;

	//meta = (BindWidgetAnim)
	UPROPERTY()
	TObjectPtr<UWidgetAnimation> FadeMapAnimation;

	UPROPERTY()
	TMap<UObject*,UMinimapMarkerWidget*> MarkerMap;

	UPROPERTY()
	TObjectPtr<UMinimapMarkerWidget> CurrentMarker;
	
	UPROPERTY()
	TMap<UMinimapMarkerWidget*,UMinimapDistantMarkerWidget*> DistantMarkerMap;

	UPROPERTY()
	TObjectPtr<UUserWidget> CurrentDistantMarker;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMinimapMarkerWidget> MinimapMarkerClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMinimapDistantMarkerWidget> MinimapDistantMarkerClass;

protected:

	virtual void NativePreConstruct() override;
	void HandleFadeInMinimap(float FadeInSpeedPointer = 3.f,float FadeInSpeedMap = 4.f);
	void HandleFadeOutMinimap(float FadeInSpeedPointer = 3.f,float FadeInSpeedMap = 4.f);
	void SetupMap();
	void SetMinimapInfoToMarker();
	
public:	
	void SetupMinimapInfo(FMiniMapInfo& Info);
	
	/* Map Calculate */
	APawn* GetPlayerPawn() const;
	FVector2D GetPlayerLocation2D() const;
	FVector2D GetBottomLeftLocation2D() const;
	FVector2D GetTopRightLocation2D() const;
	FVector2D FindPlayer2DLocationOnMap(FVector2D InValue) const;
	FVector2D GetCanvasMapPosition() const;
	float GetCanvasMapSize() const;
	void SetCanvasMapPosition(FVector2D InPosition) const;
	void SetCanvasMapSize(float InSize) const;
	float GetMapTextureSize() const;
	void FindZoomAspectRatio(float Value, float& AspectRatio, float& ZoomSize) const;

	// GetSet
	UFUNCTION()
	void SetMinimapSize(float InSize);

	UFUNCTION()
	void SetPointerSize(float InSize);

	UFUNCTION()
	void SetPointerIconAndColor(UObject* InIcon, FLinearColor InColor);

	UFUNCTION()
	void SetBlendMask(UTexture2D* InTexture);

	UFUNCTION()
	void SetMinimapTint(FLinearColor InColor);

	UFUNCTION()
	float FindAngleByDirection(int32 InAngle);

	UFUNCTION()
	float GetPlayerPointerAngle();
	
	UFUNCTION()
	bool MarkerObjectInUse(UObject* InMarkerObject);

	
private:
	UObject* ValidateMarkerObject(UObject* InObject);
	bool IsMarkerObjectInUse(UObject* InObject);
	UMinimapMarkerWidget* FindMarker(UObject* InObject);
	UMinimapDistantMarkerWidget* FindDistantMarker(UMinimapMarkerWidget* InMarkerWidget);
	void InterpolateCanvasMapPosition(float InInterpSpeed,bool InInterpConstant);
	void InterpolateCanvasMapZoom(float InZoomValue, float InInterpSpeed, bool InInterpConstant);
	void HandleCreateOrFindMarker(UObject* InObject);
};
