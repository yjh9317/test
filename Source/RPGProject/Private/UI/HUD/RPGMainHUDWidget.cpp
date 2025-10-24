// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/RPGMainHUDWidget.h"

#include "Blueprint/SlateBlueprintLibrary.h"
#include "Components/CanvasPanel.h"
#include "Header/DebugHeader.h"
#include "Inventory/UI/LootBar/LootBarWidget.h"
#include "UI/Compass/CompassWidget.h"
#include "UI/Minimap/MinimapWidget.h"

void URPGMainHUDWidget::NativeConstruct()
{
	InitCompassVariables();
	InitMinimapVariables();
}

void URPGMainHUDWidget::SetMousePositionOnLootBar()
{
	UWorld* LocalWorld = GetWorld();
	APlayerController* LocalPC = GetOwningPlayer();
	if(nullptr == LocalWorld || nullptr == LocalPC) return;
	
	const FGeometry& LocalGeometry = CanvasPanel_InitMousePos->GetCachedGeometry();
	FVector2D PixelPos = {};
	FVector2D ViewportPos = {};
	USlateBlueprintLibrary::LocalToViewport(LocalWorld,LocalGeometry,FVector2D{0.f,0.f},PixelPos,ViewportPos);

	LocalPC->SetMouseLocation(FMath::TruncToFloat(PixelPos.X),FMath::TruncToFloat(PixelPos.Y));
}

void URPGMainHUDWidget::ShowLootBarWidget()
{
	if(LootBarWidget)
	{
		LootBarWidget->ActivateLootBar();
		LootBarWidget->SetVisibility(ESlateVisibility::Visible);
		SetMousePositionOnLootBar();
	}
}

void URPGMainHUDWidget::HideLootBarWidget()
{
	if(LootBarWidget)
	{
		LootBarWidget->DeactivateLootBar();
		LootBarWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

#pragma region InitailizeFunction

void URPGMainHUDWidget::InitMinimapVariables()
{
	FMinimapInitData MinimapInitData;
	MinimapInitData.Size = Minimap_Size;
	MinimapInitData.MinimapFrame = Minimap_Frame;
	MinimapInitData.MinimapTint = Minimap_Tint;
	MinimapInitData.PointerColor = Minimap_PointerColor;
	MinimapInitData.PointerIcon = Minimap_PointerIcon;
	MinimapInitData.PointerSize = Minimap_PointerSize;
	MinimapInitData.IsFirstPerson = IsFirstPerson;
	MinimapInitData.MinimapFrameScale = Minimap_FrameScale;
	MinimapInitData.MinimapFrameTint = Minimap_FrameTint;
	MinimapInitData.MinMaxMarkerScale = Minimap_MinMaxMarkerScale;
	MinimapInitData.ZoomInterpSpeed = Minimap_ZoomInterpSpeed;
	MinimapInitData.PointerMinMaxScale = Minimap_PointerMinMaxScale;
	MinimapInitData.RotateMinimapWithPlayer = Minimap_RotateWithPlayer;
	MinimapInitData.ScalePointerWithZoom = Minimap_ScalePointerWithZoom;
	
	if(MinimapWidget)
	{
		MinimapWidget->InitMinimapData(MinimapInitData);
	}
}

void URPGMainHUDWidget::InitCompassVariables()
{
	FCompassInitData CompassInitData;
	CompassInitData.BackgroundColor = Compass_BackgroundColor;
	CompassInitData.BackgroundOpacity = Compass_BackgroundOpacity;
	CompassInitData.BackgroundTexture = Compass_BackgroundTexture;
	CompassInitData.BackgroundTransform = Compass_BackgroundTransform;
	CompassInitData.CompassMask = Compass_BlendMask;
	CompassInitData.CompassTint = Compass_Tint;
	CompassInitData.PointerScale =  Compass_PointerScale;
	CompassInitData.PointerTexture = Compass_PointerTexture;
	CompassInitData.PointerTint = Compass_PointerTint;
	CompassInitData.MarkerYTranslation = Compass_MarkerYTranslation;
	CompassInitData.PointerYTranslation = Compass_PointerYTranslation;
	
	// CompassWidget = CreateWidget<UCompassWidget>(this,CompassWidgetClass);
	// if(CompassWidget)
	// {
	// 	CompassWidget->InitCompassData(CompassInitData);
	// }
}

#pragma endregion

#pragma region Minimap

void URPGMainHUDWidget::ShowMinimap(int32 MinimapIndex) const
{
	FMiniMapInfo Info;
	if(Minimaps.IsValidIndex(MinimapIndex))
	{
		DebugHeader::LogWarning("URPGMainHUDWidget::ShowMinimap");
		
		Info = Minimaps[MinimapIndex];
		Info.BottomLeftActor = BottomLeftActor;
		Info.BottomLeftCoordinates ={-589.f,-2417.f};
		Info.TopRightActor = TopRightActor;
		Info.TopRightCoordinates = {-3243.f,1446.0};
		Info.MinimapTexture = MinimapTexture;
	}
	MinimapWidget->SetupMinimapInfo(Info);
}
#pragma endregion 