
#include "UI/Minimap/MinimapWidget.h"
#include "Animation/WidgetAnimation.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/Border.h"
#include "Components/RetainerBox.h"
#include "Header/DebugHeader.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UI/FunctionLibrary/UIUtilityLibrary.h"
#include "UI/Minimap/MinimapMarkerWidget.h"

UMinimapWidget::UMinimapWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	IsActive = true;
}

void UMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// Overlay_Map->SetRenderOpacity(1.f);
	// Overlay_DistantMarker->SetRenderOpacity(1.f);
}

void UMinimapWidget::NativePreConstruct()
{
	if(Image_Map)
	{
		if (IsDesignTime())
		{
			Image_Map->SetVisibility(ESlateVisibility::Collapsed);
		}
		else
		{
			Image_Map->SetVisibility(ESlateVisibility::Visible);
		}
	}

	// MapSize = GetMapTextureSize();
	// SetCanvasMapSize(MapSize);

	IsActive = false;
	IsActive ? SetRenderOpacity(1.f) : SetRenderOpacity(0.f);
	
	// SetBlendMask();

	if(RotateMinimapWithPlayer)
	{
		Image_Frame->SetRenderTransformAngle(-90.f);
		Overlay_PointerAnim->SetRenderTransformAngle(-90.f);
		Overlay_CanvasPanelMap->SetRenderTransformAngle(-90.f);
	}
}


void UMinimapWidget::InitMinimapData(const FMinimapInitData& Data)
{
	float Size = FMath::Clamp(Data.Size,64.f,512.f);
	UUIUtilityLibrary::SetSizeBoxSize(SizeBox_Content,Size,Size);
	UUIUtilityLibrary::SetImageBrushAndColor(Image_Pointer,Data.PointerIcon,Data.PointerColor);
	SetPointerSize(Data.PointerSize);
	SetMinimapTint(Data.MinimapTint);
	
	MinMaxScale = Data.PointerMinMaxScale;
	ZoomInterpSpeed = Data.ZoomInterpSpeed;
	ScalePointerWithZoom = Data.ScalePointerWithZoom;
	PointerMinMaxScale = Data.PointerMinMaxScale;
	RotateMinimapWithPlayer = Data.RotateMinimapWithPlayer;
	IsFirstPerson = Data.IsFirstPerson;
	
	if(Data.MinimapFrame)
	{
		Image_Frame->SetRenderOpacity(1.f);
		UUIUtilityLibrary::SetImageBrushAndColor(Image_Frame,Data.MinimapFrame,Data.MinimapFrameTint);
		Image_Frame->SetRenderScale(Data.MinimapFrameScale);
	}
}

void UMinimapWidget::ShowMinimap(FMiniMapInfo Info)
{
	if(IsActive) return;
	IsActive = true;
	SetupMinimapInfo(Info);
	SetRenderOpacity(1.f);
	// HandleFadeInMinimap();
}

void UMinimapWidget::HideMinimap()
{
	if(!IsActive) return;
	IsActive = false;
	SetRenderOpacity(0.f);
}

void UMinimapWidget::UpdateMiniMap(float NewZoomValue)
{
}

void UMinimapWidget::AddOrUpdateMarker(UObject* Marker, bool ScaleWithZoom, FMiniMapInfo Info)
{
}

void UMinimapWidget::RemoveMarker(UObject* Marker)
{
}

void UMinimapWidget::RemoveAllMinimapMarker()
{
}



void UMinimapWidget::SetupMinimapInfo(FMiniMapInfo& Info)
{
	MiniMapInfo = Info;
	SetupMap();
	SetMinimapInfoToMarker();
}


void UMinimapWidget::HandleFadeInMinimap(float FadeInSpeedPointer, float FadeInSpeedMap)
{
	FWidgetAnimationDynamicEvent AnimationFinish;
	AnimationFinish.BindUFunction(this,FName("OnAnimationFinished"));
	BindToAnimationFinished(FadeMapAnimation,AnimationFinish);
	
	// UnbindFromAnimationFinished(FadeMapAnimation,)
}

void UMinimapWidget::HandleFadeOutMinimap(float FadeInSpeedPointer, float FadeInSpeedMap)
{
}



void UMinimapWidget::SetupMap()
{
	BottomLeft = MiniMapInfo.BottomLeftCoordinates;
	BLCache = BottomLeft;
	TopRight = MiniMapInfo.TopRightCoordinates;
	TRCache = TopRight;

	UUIUtilityLibrary::SetImageBrush(Image_Map,MiniMapInfo.MinimapTexture);
}

void UMinimapWidget::SetMinimapInfoToMarker()
{
	TArray<UWidget*> Widgets = Overlay_MapMarkers->GetAllChildren();

	for(auto& Widget : Widgets)
	{
		if(UMinimapMarkerWidget* MarkerWidget = Cast<UMinimapMarkerWidget>(Widget))
		{
			MarkerWidget->MiniMapInfo = MiniMapInfo;
		}
	}
}

APawn* UMinimapWidget::GetPlayerPawn() const
{
	return GetOwningPlayer()->GetPawn();
}

FVector2D UMinimapWidget::GetPlayerLocation2D() const
{
	return FVector2D{GetPlayerPawn()->GetActorLocation().X, GetPlayerPawn()->GetActorLocation().Y};
}

FVector2D UMinimapWidget::GetBottomLeftLocation2D() const
{
	FVector ActorLocation{};
	FVector2D ActorLocation2D{};
	if(MiniMapInfo.BottomLeftActor)
	{
		ActorLocation = UUIUtilityLibrary::GetActorLocationByClassAndIndex(GetWorld(),MiniMapInfo.BottomLeftActor);
		ActorLocation2D = {ActorLocation.X, ActorLocation.Y};
	}
	else if(MinimapBottomLeftActor)
	{
		TArray<AActor*> MinimapBottomLeftActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(),MinimapBottomLeftActor,MinimapBottomLeftActors);

		if(!MinimapBottomLeftActors.IsEmpty())
		{
			ActorLocation = MinimapBottomLeftActors[0]->GetActorLocation();
			ActorLocation2D = {ActorLocation.X, ActorLocation.Y};
			return  ActorLocation2D;
		}
	}
	return ActorLocation2D;
}

FVector2D UMinimapWidget::GetTopRightLocation2D() const
{
	FVector ActorLocation{};
	FVector2D ActorLocation2D{};
	if(MiniMapInfo.TopRightActor)
	{
		ActorLocation = UUIUtilityLibrary::GetActorLocationByClassAndIndex(GetWorld(),MiniMapInfo.TopRightActor);
		ActorLocation2D = {ActorLocation.X, ActorLocation.Y};
	}
	else if(MinimapTopRightActor)
	{
		TArray<AActor*> MinimapBottomLeftActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(),MinimapTopRightActor,MinimapBottomLeftActors);

		if(!MinimapBottomLeftActors.IsEmpty())
		{
			ActorLocation = MinimapBottomLeftActors[0]->GetActorLocation();
			ActorLocation2D = {ActorLocation.X, ActorLocation.Y};
			return  ActorLocation2D;
		}
	}
	return ActorLocation2D;
}

FVector2D UMinimapWidget::FindPlayer2DLocationOnMap(FVector2D Value) const
{
	return UUIUtilityLibrary::MapRangeClampedVector2D(
		Value,
		GetBottomLeftLocation2D(),
		GetTopRightLocation2D(),
		BottomLeft,
		TopRight);
}

FVector2D UMinimapWidget::GetCanvasMapPosition() const
{
	return UWidgetLayoutLibrary::SlotAsCanvasSlot(Image_Map)->GetPosition();
}

float UMinimapWidget::GetCanvasMapSize() const
{
	return UWidgetLayoutLibrary::SlotAsCanvasSlot(Image_Map)->GetSize().X;
}

void UMinimapWidget::SetCanvasMapPosition(FVector2D InPosition) const
{
	UWidgetLayoutLibrary::SlotAsCanvasSlot(Image_Map)->SetSize(InPosition);
	UWidgetLayoutLibrary::SlotAsCanvasSlot(Border_MapMarkers)->SetSize(InPosition);
}

void UMinimapWidget::SetCanvasMapSize(float InSize) const
{
	FVector2D SizeVector = {InSize,InSize};
	UWidgetLayoutLibrary::SlotAsCanvasSlot(Image_Map)->SetSize(SizeVector);
	UWidgetLayoutLibrary::SlotAsCanvasSlot(Border_MapMarkers)->SetSize(SizeVector);
}

float UMinimapWidget::GetMapTextureSize() const
{
	FSlateBrush MapBrush = Image_Map->Brush;
	
	if(UObject* LocalMapImage = MapBrush.GetResourceObject())
	{
		if(UTexture2D* Texture2D = Cast<UTexture2D>(LocalMapImage))
		{
			return Texture2D->GetSizeX();
		}
	}
	return 0.f;
}

void UMinimapWidget::FindZoomAspectRatio(float Value, float& AspectRatio, float& ZoomSize) const
{
	float OutputValue = UKismetMathLibrary::MapRangeClamped(Value,0.f,2.f,MapSize/ZoomDivider,MapSize);
	AspectRatio = MapSize / OutputValue;
	ZoomSize = OutputValue;
}



void UMinimapWidget::SetMinimapSize(float InSize)
{
	float NewSize = FMath::Clamp(InSize,64.f,512.f);
	UUIUtilityLibrary::SetSizeBoxSize(SizeBox_Content,NewSize,NewSize);
}

void UMinimapWidget::SetPointerSize(float InSize)
{
	UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(Image_Pointer);
	if(CanvasSlot)
	{
		FVector2D PointerSize = FVector2D{InSize,InSize};
		FVector2D PointerPosition = UKismetMathLibrary::Divide_Vector2DFloat(PointerSize,2.f) * -1;
	
		CanvasSlot->SetSize(PointerSize);
		UUIUtilityLibrary::SetImageBrushSize(Image_Pointer,PointerSize);
		CanvasSlot->SetPosition(PointerPosition);
	}
}

void UMinimapWidget::SetPointerIconAndColor(UObject* InIcon, FLinearColor InColor)
{
	UUIUtilityLibrary::SetImageBrush(Image_Pointer,InIcon);
	Image_Pointer->SetColorAndOpacity(InColor);
}

void UMinimapWidget::SetBlendMask(UTexture2D* InTexture)
{
	if(UMaterialInstanceDynamic* MID = RetainerBox_Minimap->GetEffectMaterial())
	{
		MID->SetTextureParameterValue("Mask",InTexture);
	}
}

void UMinimapWidget::SetMinimapTint(FLinearColor InColor)
{
	Image_Map->SetColorAndOpacity(InColor);
}


float UMinimapWidget::FindAngleByDirection(int32 Angle)
{
	float Result = 0.f;
	if(Angle >= -180.f && Angle <= -90.f)
	{
		Result = UKismetMathLibrary::MapRangeClamped(Angle,-90.f,-180.f,0.f,90.f);	
	}
	else if(Angle >= -90.f && Angle <= 0.f)
	{
		Result = UKismetMathLibrary::MapRangeClamped(Angle,-90.f,0.f,0.f,-90.f);
	}
	else if(Angle >= 0.f && Angle <= 90.f)
	{
		Result = UKismetMathLibrary::MapRangeClamped(Angle,0.f,90.f,-90.f,-180.f);
	}
	else if(Angle >= 90.f && Angle <= 180.f)
	{
		Result = UKismetMathLibrary::MapRangeClamped(Angle,90.f,180.f,180.f,90.f);
	}
	return Result;
}

float UMinimapWidget::GetPlayerPointerAngle()
{
	FRotator CameraRot = GetOwningPlayerCameraManager()->GetCameraRotation();
	FRotator InverseRot = UKismetMathLibrary::MakeRotator(0.f,0.f,CameraRot.Yaw).GetInverse();

	FRotator PawnRot = GetOwningPlayerPawn()->GetActorRotation();
	
	return UKismetMathLibrary::ComposeRotators(InverseRot,PawnRot).Yaw;
}

bool UMinimapWidget::MarkerObjectInUse(UObject* InMarkerObject)
{
	TArray<UWidget*> Markers = Overlay_MapMarkers->GetAllChildren();

	for(auto Marker : Markers)
	{
		UMinimapMarkerWidget* MarkerWidget = Cast<UMinimapMarkerWidget>(Marker);
		if(MarkerWidget == InMarkerObject)
		{
			return true;
		}
	}
	return false;
}

UObject* UMinimapWidget::ValidateMarkerObject(UObject* InObject)
{
	if (InObject)
	{
		if (AActor* Actor = Cast<AActor>(InObject))
		{
			return Cast<USceneComponent>(Actor) ? InObject : nullptr;
		}
	}
	return nullptr;
}

bool UMinimapWidget::IsMarkerObjectInUse(UObject* InObject)
{
	if(InObject)
	{
		return MarkerObjectInUse(InObject); 
	}
	return false;
}

UMinimapMarkerWidget* UMinimapWidget::FindMarker(UObject* InObject)
{
	if(UObject* ValidateObj = ValidateMarkerObject(InObject))
	{
		return *MarkerMap.Find(ValidateObj);
	}
	return nullptr;
}

UMinimapDistantMarkerWidget* UMinimapWidget::FindDistantMarker(UMinimapMarkerWidget* InMarkerWidget)
{
	if(InMarkerWidget)
	{
		return *DistantMarkerMap.Find(InMarkerWidget);
	}
	return nullptr;
}


void UMinimapWidget::InterpolateCanvasMapPosition(float InInterpSpeed, bool InInterpConstant)
{
	FVector2D NewCanvasMapPosition = UUIUtilityLibrary::Vector2DInterpolate(
		GetWorld(),
		GetCanvasMapPosition(),
		FindPlayer2DLocationOnMap(GetPlayerLocation2D()),
		InInterpSpeed,
		InInterpConstant);

	SetCanvasMapPosition(NewCanvasMapPosition);
}

void UMinimapWidget::InterpolateCanvasMapZoom(float InZoomValue, float InInterpSpeed, bool InInterpConstant)
{
	CurrentZoomValue = UUIUtilityLibrary::FInterpolate(
		GetWorld(),
		CurrentZoomValue,
		InZoomValue,
		InInterpSpeed/2,
		InInterpConstant);
	
	float ClampedZoomValue = FMath::Clamp(CurrentZoomValue,0,2.f);
	float OutAspectRatio,OutZoomSize;
	
	FindZoomAspectRatio(ClampedZoomValue,OutAspectRatio,OutZoomSize);

	BottomLeft = UUIUtilityLibrary::Vector2DInterpolate(
		GetWorld(),
		BottomLeft,
		BLCache/OutAspectRatio,
		InInterpSpeed,
		InInterpConstant);

	TopRight = UUIUtilityLibrary::Vector2DInterpolate(
	GetWorld(),
	TopRight,
	TRCache/OutAspectRatio,
	InInterpSpeed,
	InInterpConstant);

	
	float NewCanvasMapSize = UUIUtilityLibrary::FInterpolate(
		GetWorld(),
		GetCanvasMapSize(),
		OutZoomSize,
		InInterpSpeed,
		InInterpConstant);
	
	SetCanvasMapSize(NewCanvasMapSize);
}

void UMinimapWidget::HandleCreateOrFindMarker(UObject* InObject)
{
	ValidateMarkerObject(InObject);
	bool IsUse = IsMarkerObjectInUse(InObject);

	// if(IsUse)
	// {
	// 	CurrentMarker = FindMarker(InObject);
	// 	CurrentDistantMarker = FindDistantMarker(CurrentMarker);
	// }
	// else
	// {
	// 	if(MinimapDistantMarkerClass)
	// 	{
	// 		UMinimapDistantMarkerWidget* DistantMarkerWidget =
	// 			CreateWidget<UMinimapDistantMarkerWidget>(this,MinimapDistantMarkerClass);
	//
	// 		// 변수 세팅
	// 		if(DistantMarkerWidget)
	// 		{
	// 			CurrentDistantMarker = DistantMarkerWidget;
	// 		}
	// 		UUIUtilityLibrary::AddChildToOverlay(
	// 			Overlay_DistantMarker,
	// 			CurrentDistantMarker,
	// 			VAlign_Center,
	// 			HAlign_Center);
	//
	// 		UMinimapMarkerWidget* MinimapMarkerWidget
	// 			= CreateWidget<UMinimapMarkerWidget>(this,MinimapMarkerClass);
	//
	// 		CurrentMarker = MinimapMarkerWidget;
	// 		UUIUtilityLibrary::AddChildToOverlay(
	// 			Overlay_MapMarkers,
	// 			MinimapMarkerWidget,
	// 			VAlign_Center,
	// 			HAlign_Center);
	//
	// 		// 변수 세팅
	//
	// 		MarkerMap.Add(InObject,CurrentMarker);
	// 		DistantMarkerMap.Add(CurrentMarker,CurrentDistantMarker);
	// 	}
	// }
}
