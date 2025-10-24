// Fill out your copyright notice in the Description page of Project Settings.



#include "UI/Minimap/MinimapDistantMarkerWidget.h"


UMinimapDistantMarkerWidget::UMinimapDistantMarkerWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}
//
// void UMinimapDistantMarkerWidget::UpdateDistantMarker(bool _IsMarkerInMapRadius, float _ZoomValue, float _ZoomDivider)
// {
// 	if(IsActive)
// 	{
// 		IsMarkerInMapRadius = _IsMarkerInMapRadius;
// 		ZoomValue = _ZoomValue;
// 		ZoomDivider = _ZoomDivider;
//
// 		if(IsMarkerInMapRadius)
// 		{
// 			HideMarker();
// 		}
// 		else
// 		{
// 			ShowMarker();
// 		}
//
// 		if(IsVisible)
// 		{
// 			
// 		}
// 	}
// }
//
// void UMinimapDistantMarkerWidget::ShowMarker()
// {
// 	if(!IsVisible)
// 	{
// 		IsVisible = true;
// 		AnimFade(true,12.f);
// 	}
// }
//
// void UMinimapDistantMarkerWidget::HideMarker()
// {
// 	if(IsVisible)
// 	{
// 		IsVisible = false;
// 		AnimFade(false,12.f);
// 	}
// }
//
// void UMinimapDistantMarkerWidget::PlayWidgetAnimation(UWidgetAnimation* WidgetAnim, bool FadeIn, bool Loop,
// 	float PlaybackSpeed)
// {
// 	int32 NumLoopsToPlay = Loop ? 0 : 1;
// 	EUMGSequencePlayMode::Type PlayMode = FadeIn ? EUMGSequencePlayMode::Forward : EUMGSequencePlayMode::Reverse;
//
// 	PlayAnimation(WidgetAnim,0.f,NumLoopsToPlay,PlayMode,PlaybackSpeed);
// }
//
// void UMinimapDistantMarkerWidget::AnimFade(bool FadeIn, float PlaybackSpeed)
// {
// 	if(nullptr != WidgetAnimation)
// 	{
// 		PlayWidgetAnimation(WidgetAnimation,FadeIn,false,PlaybackSpeed);
// 	}	
// }
//
// int32 UMinimapDistantMarkerWidget::GetYardDistanceToMarker()
// {
// 	float Length = (GetOwningPlayerPawn()->GetActorLocation() - GetMarkerLocation()).Size();
// 	if(Length <=0.f)
// 	{
// 		return 0.f;
// 	}
// 	return FMath::TruncToInt32((Length/100)* 1.094);
// }
//
// int32 UMinimapDistantMarkerWidget::GetMeterDistanceToMarker()
// {
// 	int32 TruncValue = FMath::TruncToInt32(GetPlayerDistanceToMarker());
// 	return TruncValue / 100.f;
// }
//
// FString UMinimapDistantMarkerWidget::GetDistanceValueFromText()
// {
// 	FString ResultStr,DummyStr;
// 	UKismetStringLibrary::Split(DistanceValue->GetText().ToString(), TEXT(" "), ResultStr, DummyStr);
// 	return ResultStr;
// }
//
// bool UMinimapDistantMarkerWidget::IsInStepRange(int32 Value, int32 Steps)
// {
// }
//
// FString UMinimapDistantMarkerWidget::FindYards()
// {
// }
//
// FString UMinimapDistantMarkerWidget::GetDigitFromInteger(int32 Integer, int32 Index)
// {
// }
//
// FString UMinimapDistantMarkerWidget::FindUnder10MiInt()
// {
// }
//
// FString UMinimapDistantMarkerWidget::FindOver10MiInt()
// {
// }
//
// FString UMinimapDistantMarkerWidget::FindMeters()
// {
// }
//
// FString UMinimapDistantMarkerWidget::FindUnder10KmInt()
// {
// }
//
// FString UMinimapDistantMarkerWidget::FindOver10KmInt()
// {
// }
//
// FString UMinimapDistantMarkerWidget::FindKilometers()
// {
// }
//
// FString UMinimapDistantMarkerWidget::FindMiles()
// {
// }
//
// void UMinimapDistantMarkerWidget::SetDistanceText(FString InString)
// {
// }
//
// void UMinimapDistantMarkerWidget::InterpolateRenderOpacity(UUserWidget* Input, float Target, float InterpSpeed)
// {
// }
//
// APawn* UMinimapDistantMarkerWidget::GetPlayerPawn() const
// {
// 	return GetOwningPlayer()->GetPawn();
// }
//
// FLinearColor UMinimapDistantMarkerWidget::FindDistantMarkerColor()
// {
// 	if(DistantMarkerInfo.Tint == FLinearColor{0.f,0.f,0.f,0.f})
// 	{
// 		
// 	}
// }
//
// UObject* UMinimapDistantMarkerWidget::FindDistantMarkerIcon()
// {
// }
//
// int32 UMinimapDistantMarkerWidget::FindMarkerTypeIndex()
// {
// }
//
// FVector2D UMinimapDistantMarkerWidget::FindZoomScaleValue()
// {
// }
//
// FLinearColor UMinimapDistantMarkerWidget::FindColor(FLinearColor InColor, FLinearColor StylesheetColor)
// {
// 	if(UUIUtilityLibrary::HasColorChanged(InColor))
// 	{
// 		return InColor;
// 	}
// 	return StylesheetColor;
// }
//
// FTextInfo UMinimapDistantMarkerWidget::GetDistanceTextInfo()
// {
// }
//
// FLinearColor UMinimapDistantMarkerWidget::FindDistanceTextColor(FLinearColor StylesheetColor)
// {
// }
//
// void UMinimapDistantMarkerWidget::HandleRenderOpacity(float InterpSpeedIcon, float InterpSpeedText)
// {
// 	if()
// }
//
// FVector UMinimapDistantMarkerWidget::GetMarkerLocation() const
// {
// 	if(AActor* Marker = Cast<AActor>(MarkerObject))
// 	{
// 		if(Marker)
// 		{
// 			return Marker->GetActorLocation();
// 		}
// 		else
// 		{
// 			USceneComponent* SceneComponent = Cast<USceneComponent>(MarkerObject);
// 			return SceneComponent->GetComponentLocation();
// 		}
// 	}
// 	return FVector{};
// }
//
// FVector UMinimapDistantMarkerWidget::GetPlayerLocation() const
// {
// 	return GetPlayerPawn()->GetActorLocation();
// }
//
// float UMinimapDistantMarkerWidget::GetPlayerDistanceToMarker() const
// {
// 	float Length = (GetOwningPlayerPawn()->GetActorLocation() - GetMarkerLocation()).Length();
// 	return Length <= 0.f ? 0.f : Length;
// }
//
// int32 UMinimapDistantMarkerWidget::GetPlayerDistanceToMarkerInM()
// {
// }