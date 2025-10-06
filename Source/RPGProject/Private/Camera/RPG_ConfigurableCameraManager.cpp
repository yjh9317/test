// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/RPG_ConfigurableCameraManager.h"
#include "Camera/Methods/RPG_IFocusTargetMethod.h"
#include "Camera/Modifiers/RPG_CameraAnimationModifier.h"
#include "Camera/Modifiers/RPG_CameraCollisionModifier.h"
#include "Camera/Modifiers/RPG_CameraDitheringModifier.h"
#include "Camera/Modifiers/Child/AnglesConstraintModifier.h"
#include "Camera/Modifiers/Child/ArmOffsetModifier.h"
#include "Camera/Modifiers/Child/CameraCollisionModifier.h"
#include "Camera/Modifiers/Child/FocusCameraModifier.h"
#include "Camera/Modifiers/Child/PitchFollowModifier.h"
#include "Camera/Modifiers/Child/PitchToArmLengthAndFOVModifier.h"
#include "Camera/Modifiers/Child/YawFollowCameraModifier.h"
#include "Components/TimelineComponent.h"
#include "SequencerScripting/Public/ExtensionLibraries/MovieSceneSequenceExtensions.h"

void ARPG_ConfigurableCameraManager::BeginPlay()
{
	Super::BeginPlay();

	SetupModifierReferences();

	if(RPGFocusCameraModifier)
	{
		RPGFocusCameraModifier->OnBlockedFocusLineOfSight.AddDynamic(this,&ARPG_ConfigurableCameraManager::OnBlockedFocusLOS);
	}
	
	if(DefaultCameraData)
	{
		SetCameraData(DefaultCameraData);
	}
}

void ARPG_ConfigurableCameraManager::OnCameraDataStackChanged_Implementation(URPG_CameraDataAssetBase* CameraDA)
{
	Super::OnCameraDataStackChanged_Implementation(CameraDA);

	if(CameraDA)
	{
		FCameraArmOffsetSettings ArmOffsetSettings = CameraDA->ArmOffsetSettings;
		SetArmSocketOffset(ArmOffsetSettings.ArmSocketOffset,ArmOffsetSettings.ArmSocketOffsetBlendTime,ArmOffsetSettings.ArmSocketOffsetBlendCurve);
		SetArmTargetOffset(ArmOffsetSettings.ArmTargetOffset,ArmOffsetSettings.ArmTargetOffsetBlendTime,ArmOffsetSettings.ArmTargetOffsetBlendCurve);

		FCameraArmLengthSettings ArmLengthSettings = CameraDA->ArmLengthSettings;
		SetArmLengthRange(ArmLengthSettings.MinArmLength,ArmLengthSettings.MaxArmLength,ArmLengthSettings.ArmRangeBlendTime,ArmLengthSettings.ArmRangeBlendCurve);

		FCameraFOVSettings FOVSettings = CameraDA->FOVSettings;
		SetFOVRange(FOVSettings.MinFOV,FOVSettings.MaxFOV,FOVSettings.FOVRangeBlendTime,FOVSettings.FOVRangeBlendCurve);

		SetPitchToArmLengthAndFOVCurves(CameraDA->PitchToArmAndFOVCurveSettings);
		SetPitchConstraints(CameraDA->PitchConstraints);
		SetYawConstraints(CameraDA->YawConstraints);
		SetCameraCollisionSettings(CameraDA->CollisionSettings);
		SetYawMovementFollowSettings(CameraDA->YawFollowSettings);
		SetPitchMovementFollowSettings(CameraDA->PitchFollowSettings);
		SetCameraDitheringSettings(CameraDA->DitheringSettings);
		SetCameraFocusSettings(CameraDA->FocusSettings);
	}
}

void ARPG_ConfigurableCameraManager::OnBlockedFocusLOS()
{
	ToggleCameraFocus(false);
}

ARPG_ConfigurableCameraManager::ARPG_ConfigurableCameraManager()
{
	ResetControlRotationTimeline =CreateDefaultSubobject<UTimelineComponent>(TEXT("ResetControlRotationTimeline"));
	ResetControlRotationToTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ResetControlRotationToTimeline"));
	ArmTargetOffsetTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ArmTargetOffsetTimeline"));
	ArmSocketOffsetTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ArmSocketOffsetTimeline;"));
	YawRangeTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("YawRangeTimeline"));
	PitchRangeTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("PitchRangeTimeline"));
	ArmLengthRangeTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("ArmLengthRangeTimeline"));
	FOVRangeTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("FOVRangeTimeline")); 
}

void ARPG_ConfigurableCameraManager::GetCurrentFOVRange(double& OutMinFOV, double& OutMaxFOV) const
{
	OutMinFOV = MinFOV;
	OutMaxFOV = MaxFOV;
}

void ARPG_ConfigurableCameraManager::GetCurrentArmOffsets(FVector& OutArmSocketOffset, FVector& OutArmTargetOffset) const
{
	OutArmSocketOffset = ArmSocketOffset;
	OutArmTargetOffset = ArmTargetOffset;
}

void ARPG_ConfigurableCameraManager::GetCurrentArmLengthRange(double& OutMinArmLength, double& OutMaxArmLength) const
{
	OutMinArmLength = MinArmLength;
	OutMaxArmLength = MaxArmLength;
}

void ARPG_ConfigurableCameraManager::SetPitchConstraints(FCameraPitchConstraintSettings PitchConstraints)
{
	if(RPGAnglesConstraintModifier)
	{
		RPGAnglesConstraintModifier->PitchConstraintTolerance = PitchConstraints.PitchConstraintTolerance;
		SetPitchViewConstraints(
			PitchConstraints.bConstrainPitch ? PitchConstraints.LocalMinPitch : 89.900002f,
			PitchConstraints.bConstrainPitch ? PitchConstraints.LocalMinPitch : 89.900002f,
			PitchConstraints.PitchConstraintsBlendTime,
			PitchConstraints.PitchConstraintsBlendCurve
			);
	}
}

void ARPG_ConfigurableCameraManager::SetYawConstraints(FCameraYawConstraintSettings YawConstraints)
{
	if(RPGAnglesConstraintModifier)
	{
		RPGAnglesConstraintModifier->PitchConstraintTolerance = YawConstraints.YawConstraintTolerance;
		SetYawViewConstraints(
			YawConstraints.bConstrainYaw ? YawConstraints.LocalMinYaw : 0.f,
			YawConstraints.bConstrainYaw ? YawConstraints.LocalMinYaw : 359.998993f,
			YawConstraints.YawConstraintsBlendTime,
			YawConstraints.YawConstraintsBlendCurve
			);
	}
}

double ARPG_ConfigurableCameraManager::GetCameraAnimationDuration(UMovieSceneSequence* InSequence,
	const FCameraAnimationParams& CameraAnimationParams) const
{
	if(!InSequence)
	{
		return 0.f;
	}
	
	/*
	 *	CameraAnimationParams.DurationOverride != 0 ? Seconds : CameraAnimationParams.DurationOverride;
	 */

	// GetPlaybackEndSecond의 구현 부분을 가져와서 사용
	
	UMovieScene* MovieScene = InSequence->GetMovieScene();
	if (MovieScene)
	{
		FFrameRate DisplayRate = MovieScene->GetDisplayRate();
		return CameraAnimationParams.PlayRate * DisplayRate.AsSeconds(ConvertFrameTime(UE::MovieScene::DiscreteExclusiveUpper(MovieScene->GetPlaybackRange()), MovieScene->GetTickResolution(), DisplayRate));
	}

	return 0.f;
}

void ARPG_ConfigurableCameraManager::StopCameraAnimation(UCameraAnimationSequence* InSequence, bool bImmediate)
{
	if(RPGActionCameraModifier)
	{
		RPGActionCameraModifier->StopCameraAnimationSequence(InSequence,bImmediate);
	}
}

void ARPG_ConfigurableCameraManager::SetCameraCollisionSettings(FCameraCollisionSettings CollisionSettings)
{
	if(RPGCameraCollisionModifier)
	{
		RPGCameraCollisionModifier->CollisionSettings = CollisionSettings;
	}
}

bool ARPG_ConfigurableCameraManager::IsPlayingCameraAnimation(UCameraAnimationSequence* InSequence) const
{
	if(RPGActionCameraModifier)
	{
		return RPGActionCameraModifier->IsCameraAnimationSequenceActive(InSequence);
	}
	return false;
}

UCameraAnimationSequence* ARPG_ConfigurableCameraManager::GetCurrentCameraAnimation() const
{
	if(RPGActionCameraModifier)
	{
		return RPGActionCameraModifier->GetCurrentCameraAnimation();
	}
	return nullptr;
}

void ARPG_ConfigurableCameraManager::SetupModifierReferences()
{
	RPGActionCameraModifier = Cast<URPG_CameraAnimationModifier>(FindCameraModifierByClass(URPG_CameraAnimationModifier::StaticClass()));
	RPGYawFollowModifier = Cast<UYawFollowCameraModifier>(FindCameraModifierByClass(UYawFollowCameraModifier::StaticClass()));
	RPGPitchFollowModifier = Cast<UPitchFollowModifier>(FindCameraModifierByClass(UPitchFollowModifier::StaticClass()));
	RPGCameraCollisionModifier = Cast<UCameraCollisionModifier>(FindCameraModifierByClass(UCameraCollisionModifier::StaticClass()));
	RPGCameraDitheringModifier = Cast<URPG_CameraDitheringModifier>(FindCameraModifierByClass(URPG_CameraDitheringModifier::StaticClass()));
	RPGAnglesConstraintModifier =  Cast<UAnglesConstraintModifier>(FindCameraModifierByClass(UAnglesConstraintModifier::StaticClass()));
	RPGPitchToArmLengthAndFOVModifier = Cast<UPitchToArmLengthAndFOVModifier>(FindCameraModifierByClass(UPitchToArmLengthAndFOVModifier::StaticClass())); 
	RPGFocusCameraModifier = Cast<UFocusCameraModifier>(FindCameraModifierByClass(UFocusCameraModifier::StaticClass()));
	RPGArmOffsetModifier = Cast<UArmOffsetModifier>(FindCameraModifierByClass(UArmOffsetModifier::StaticClass()));
}

void ARPG_ConfigurableCameraManager::SetCameraData(URPG_CameraDataAssetBase* Data)
{
	if(Data)
	{
		PushCameraData(Data);
	}
}

bool ARPG_ConfigurableCameraManager::IsYawMovementFollowEnabled()
{
	if(RPGYawFollowModifier)
	{
		return RPGYawFollowModifier->FollowSettings.bEnableYawMovementFollow;
	}
	return false;
}

void ARPG_ConfigurableCameraManager::SetYawMovementFollowSettings(FCameraYawFollowSettings YawFollowSettings)
{
	if(RPGYawFollowModifier)
	{
		RPGYawFollowModifier->FollowSettings = YawFollowSettings;
	}
}

void ARPG_ConfigurableCameraManager::SetPitchMovementFollowSettings(FCameraPitchFollowSettings PitchFollowSettings)
{
	if(RPGPitchFollowModifier)
	{
		RPGPitchFollowModifier->FollowSettings = PitchFollowSettings;
	}
}

void ARPG_ConfigurableCameraManager::SetCameraDitheringSettings(FCameraDitheringSettings DitheringSettings)
{
	if(RPGCameraDitheringModifier)
	{
		RPGCameraDitheringModifier->DitheringSettings = DitheringSettings;
	}
}

void ARPG_ConfigurableCameraManager::SetCameraFocusSettings(FRPGCameraFocusSettings FocusSettings)
{
	if(RPGFocusCameraModifier)
	{
		RPGFocusCameraModifier->FocusSettings = FocusSettings;
	}
}

bool ARPG_ConfigurableCameraManager::IsPitchMovementFollowEnabled()
{
	if(RPGPitchFollowModifier)
	{
		FCameraPitchFollowSettings TempPitchFollowSettings = RPGPitchFollowModifier->FollowSettings;
		return TempPitchFollowSettings.bEnablePitchMovementFollow || TempPitchFollowSettings.bTriggerWhenFalling || TempPitchFollowSettings.bTriggerOnSlopes;
	}
	return false;
}

void ARPG_ConfigurableCameraManager::ToggleCameraFocus(bool bEnabled)
{
	if(RPGFocusCameraModifier)
	{
		RPGFocusCameraModifier->FocusSettings.bEnabled = bEnabled;
	}
}

void ARPG_ConfigurableCameraManager::SetCameraFocusTargetingMethod(TSubclassOf<URPG_IFocusTargetMethod> FocusMethodClass,
	URPG_IFocusTargetMethod*& TargetingMethodRef)
{
	if (!FocusMethodClass)
	{
		TargetingMethodRef = nullptr;
		return;
	}
	
	if(RPGFocusCameraModifier)
	{
		UObject* NewObjectInstance = NewObject<UObject>(this, FocusMethodClass);
		URPG_IFocusTargetMethod* NewFocusTargetMethod = Cast<URPG_IFocusTargetMethod>(NewObjectInstance);
		
		if (NewFocusTargetMethod)
		{
			RPGFocusCameraModifier->FocusSettings.FocusTargetMethod = NewFocusTargetMethod;
			TargetingMethodRef = NewFocusTargetMethod;
		}
		else
		{
			TargetingMethodRef = nullptr;
		}
	}
	else
	{
		TargetingMethodRef = nullptr;	
	}
}

void ARPG_ConfigurableCameraManager::SetPitchToArmLengthAndFOVCurves(
	FCameraPitchToArmAndFOVCurveSettings PitchToArmAndFOVCurveSettings)
{
	if(RPGPitchToArmLengthAndFOVModifier)
	{
		RPGPitchToArmLengthAndFOVModifier->CurveSettings = PitchToArmAndFOVCurveSettings;
	}
}

void ARPG_ConfigurableCameraManager::ToggleYawAndPitchFollow(bool EnablePitchMovementFollow,
	bool TriggerPitchFollowWhenFalling, bool TriggerPitchFollowOnSlopes, bool EnableYawMovementFollow)
{
	if(RPGPitchFollowModifier)
	{
		RPGPitchFollowModifier->FollowSettings.bEnablePitchMovementFollow = EnablePitchMovementFollow;
		RPGPitchFollowModifier->FollowSettings.bTriggerWhenFalling = TriggerPitchFollowWhenFalling;
		RPGPitchFollowModifier->FollowSettings.bTriggerOnSlopes = TriggerPitchFollowOnSlopes;
	}
	if(RPGYawFollowModifier)
	{
		RPGYawFollowModifier->FollowSettings.bEnableYawMovementFollow = EnableYawMovementFollow;
	}
}

void ARPG_ConfigurableCameraManager::IsFocusEnabledInCurrentData(bool& bEnabled) const
{
	if(URPG_CameraDataAssetBase* CameraDA = GetCurrentCameraDataAsset())
	{
		bEnabled = CameraDA->FocusSettings.bEnabled;
	}
}
