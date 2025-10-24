// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CameraAnimationCameraModifier.h"
#include "MovieSceneSequence.h"
#include "Camera/RPG_PlayerCameraManager.h"
#include "Data/RPG_CameraData.h"
#include "RPG_ConfigurableCameraManager.generated.h"

class UArmOffsetModifier;
class UFocusCameraModifier;
class UPitchToArmLengthAndFOVModifier;
class URPG_CameraDitheringModifier;
class URPG_CameraCollisionModifier;
class UAnglesConstraintModifier;
class URPG_CameraAnimationModifier;
class UPitchFollowModifier;
class UYawFollowCameraModifier;
class UTimelineComponent;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API ARPG_ConfigurableCameraManager : public ARPG_PlayerCameraManager
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void OnCameraDataStackChanged_Implementation(URPG_CameraDataAssetBase* CameraDA) override;
	UFUNCTION()
	void OnBlockedFocusLOS();
public:
	ARPG_ConfigurableCameraManager();
	
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable, Category = "Camera Blends")
	void ResetControlRotationTo(FRotator InRotation,float InDuration, UCurveFloat* InCurve);

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable, Category = "Camera Blends")
	void ResetControlRotation(float InDuration, UCurveFloat* InCurve);
	
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable, Category = "Camera Blends")
	void SetArmLengthRange(float InMinArmLength, float InMaxArmLength, float InBlendTime, UCurveFloat* InBlendCurve);

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable, Category = "Camera Blends")
	void SetFOVRange(float InMinFOV, float InMaxFOV, float InBlendTime, UCurveFloat* InBlendCurve);
	
	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable, Category = "Camera Blends")
	void SetArmSocketOffset(FVector InArmOffset, float InBlendTime, UCurveFloat* InBlendCurve);

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable, Category = "Camera Blends")
	void SetArmTargetOffset(FVector InArmTargetOffset, float InBlendTime, UCurveFloat* InBlendCurve);

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable, Category = "Camera Blends")
	void SetPitchViewConstraints(float InViewPitchMin, float InViewPitchMax, float InBlendTime, UCurveFloat* InBlendCurve);

	UFUNCTION(BlueprintImplementableEvent,BlueprintCallable, Category = "Camera Blends")
	void SetYawViewConstraints(float InViewYawMin, float InViewYawMax, float InBlendTime, UCurveFloat* InBlendCurve);
	
	// /** Please add a function description */
	UFUNCTION(BlueprintPure, Category="CameraFOV")
	void GetCurrentFOVRange(double& OutMinFOV, double& OutMaxFOV) const;
	
	/** Please add a function description */
	UFUNCTION(BlueprintPure, Category="CameraArm")
	void GetCurrentArmOffsets(FVector& OutArmSocketOffset, FVector& OutArmTargetOffset) const;
	
	/** Please add a function description */
	UFUNCTION(BlueprintPure, Category="CameraArm")
	void GetCurrentArmLengthRange(double& OutMinArmLength, double& OutMaxArmLength) const;
	
	/** Please add a function description */
	UFUNCTION(BlueprintCallable, Category="CameraAngleConstraints")
	void SetPitchConstraints(FCameraPitchConstraintSettings PitchConstraints);
	
	/** Please add a function description */
	UFUNCTION(BlueprintCallable, Category="CameraAngleConstraints")
	void SetYawConstraints(FCameraYawConstraintSettings YawConstraints);
	
	/** Please add a function description */
	UFUNCTION(BlueprintPure, Category="CameraAnimations")
	double GetCameraAnimationDuration(UMovieSceneSequence* InSequence, const FCameraAnimationParams& CameraAnimationParams) const;
	
	/** Please add a function description */
	UFUNCTION(BlueprintCallable, Category="CameraAnimations")
	void StopCameraAnimation(UCameraAnimationSequence* InSequence, bool bImmediate);
	
	/** Please add a function description */
	UFUNCTION(BlueprintCallable, Category="Camera Collisions")
	void SetCameraCollisionSettings(FCameraCollisionSettings CollisionSettings);
	
	/** Please add a function description */
	UFUNCTION(BlueprintPure, Category="CameraAnimations")
	bool IsPlayingCameraAnimation(UCameraAnimationSequence* InSequence) const;
	
	/** Please add a function description */
	UFUNCTION(BlueprintPure, Category="CameraAnimations")
	UCameraAnimationSequence* GetCurrentCameraAnimation() const;
	
	/** Please add a function description */
	UFUNCTION(BlueprintCallable, Category="RPG Camera")
	void SetupModifierReferences();
	
	/** Set all of the camera data settings with blends. This is useful to change the camera data once the character changes states (e.g., from standing to crouching). */
	UFUNCTION(BlueprintCallable, Category="RPG Camera")
	void SetCameraData(URPG_CameraDataAssetBase* Data);
	
	/** Please add a function description */
	UFUNCTION(BlueprintPure, Category="CameraFollow")
	bool IsYawMovementFollowEnabled();
	
	/** Please add a function description */
	UFUNCTION(BlueprintCallable, Category="CameraFollow")
	void SetYawMovementFollowSettings(FCameraYawFollowSettings YawFollowSettings);
	
	/** Please add a function description */
	UFUNCTION(BlueprintCallable, Category="CameraFollow")
	void SetPitchMovementFollowSettings(FCameraPitchFollowSettings PitchFollowSettings);
	
	/** Please add a function description */
	UFUNCTION(BlueprintCallable, Category="CameraDithering")
	void SetCameraDitheringSettings(FCameraDitheringSettings DitheringSettings);
	
	/** Please add a function description */
	UFUNCTION(BlueprintCallable, Category="CameraFocus")
	void SetCameraFocusSettings(FRPGCameraFocusSettings FocusSettings);
	
	/** Please add a function description */
	UFUNCTION(BlueprintPure, Category="CameraFollow")
	bool IsPitchMovementFollowEnabled();
	
	/** Please add a function description */
	UFUNCTION(BlueprintCallable, Category="CameraFocus")
	void ToggleCameraFocus(bool bEnabled);
	
	/** Please add a function description */
	UFUNCTION(BlueprintCallable, Category="CameraFocus")
	void SetCameraFocusTargetingMethod(TSubclassOf<URPG_IFocusTargetMethod> FocusMethodClass, URPG_IFocusTargetMethod*& TargetingMethodRef);
	
	/** Please add a function description */
	UFUNCTION(BlueprintCallable, Category="CameraPitchToArmFOV")
	void SetPitchToArmLengthAndFOVCurves(FCameraPitchToArmAndFOVCurveSettings PitchToArmAndFOVCurveSettings);
	
	/** Please add a function description */
	UFUNCTION(BlueprintCallable, Category="CameraFollow")
	void ToggleYawAndPitchFollow(bool EnablePitchMovementFollow, bool TriggerPitchFollowWhenFalling, bool TriggerPitchFollowOnSlopes, bool EnableYawMovementFollow);
	
	/** Please add a function description */
	UFUNCTION(BlueprintPure, Category="Camera Focus")
	void IsFocusEnabledInCurrentData(bool& bEnabled) const;
	
public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Internal|Blend Timelines")
	TObjectPtr<UTimelineComponent> ResetControlRotationTimeline;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Internal|Blend Timelines")
	TObjectPtr<UTimelineComponent> ResetControlRotationToTimeline;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Internal|Blend Timelines")
	TObjectPtr<UTimelineComponent> ArmTargetOffsetTimeline;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Internal|Blend Timelines")
	TObjectPtr<UTimelineComponent> ArmSocketOffsetTimeline;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Internal|Blend Timelines")
	TObjectPtr<UTimelineComponent> YawRangeTimeline;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Internal|Blend Timelines")
	TObjectPtr<UTimelineComponent> PitchRangeTimeline;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Internal|Blend Timelines")
	TObjectPtr<UTimelineComponent> ArmLengthRangeTimeline;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category="Internal|BlendTimelines")
	TObjectPtr<UTimelineComponent> FOVRangeTimeline;
private:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category="RPGCamera|FOV", meta=(MultiLine="true",AllowPrivateAccess="true"))
	double MinFOV;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category="RPGCamera|FOV", meta=(MultiLine="true",AllowPrivateAccess="true"))
	double MaxFOV;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category="Internal|BlendVariables", AdvancedDisplay, meta=(MultiLine="true",AllowPrivateAccess="true"))
	double PreviousMinArmLength;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category="Internal|BlendVariables", AdvancedDisplay, meta=(MultiLine="true",AllowPrivateAccess="true"))
	double PreviousMaxArmLength;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category="Internal|BlendVariables", AdvancedDisplay, meta=(MultiLine="true",AllowPrivateAccess="true"))
	double PreviousMinFOV;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category="Internal|BlendVariables", AdvancedDisplay, meta=(MultiLine="true",AllowPrivateAccess="true"))
	double PreviousMaxFOV;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category="Internal|BlendVariables",meta=(AllowPrivateAccess="true"))
	FVector PreviousArmTargetOffset;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category="Internal|BlendVariables", AdvancedDisplay, meta=(MultiLine="true",AllowPrivateAccess="true"))
	FVector PreviousArmSocketOffset;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category="Internal|BlendVariables",meta=(AllowPrivateAccess="true"))
	double PreviousMinPitch;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category="Internal|BlendVariables",meta=(AllowPrivateAccess="true"))
	double PreviousMaxPitch;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category="Internal|BlendVariables",meta=(AllowPrivateAccess="true"))
	float PreviousMinYaw;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category="Internal|BlendVariables",meta=(AllowPrivateAccess="true"))
	float PreviousMaxYaw;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category="Internal|BlendVariables",meta=(AllowPrivateAccess="true"))
	FRotator StartControlRotation;

	/** Offset at end of spring arm; use this instead of the relative offset of the attached component to ensure the line trace works as desired. */
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category="RPGCamera|Camera Arm", meta=(MultiLine="true",AllowPrivateAccess="true"))
	FVector ArmSocketOffset;

	/** Offset at start of spring, applied in world space. Use this if you want a world-space offset from the parent component instead of the usual relative-space offset. */
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category="RPGCamera|Camera Arm", meta=(MultiLine="true",AllowPrivateAccess="true"))
	FVector ArmTargetOffset;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category="RPGCamera|Camera Arm", meta=(MultiLine="true",AllowPrivateAccess="true"))
	double MinArmLength;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite,EditDefaultsOnly, Category="RPGCamera|Camera Arm", meta=(MultiLine="true",AllowPrivateAccess="true"))
	double MaxArmLength;
public:
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Internal|RPGModifiers|CameraFollow", AdvancedDisplay)
	TObjectPtr<UYawFollowCameraModifier> RPGYawFollowModifier;
	
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Internal|RPGModifiers|CameraFollow", AdvancedDisplay)
	TObjectPtr<UPitchFollowModifier> RPGPitchFollowModifier;
	
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Internal|RPGModifiers|CameraAnimations", AdvancedDisplay)
	TObjectPtr<URPG_CameraAnimationModifier> RPGActionCameraModifier;
	
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Internal|RPGModifiers|CameraAngleConstraints", AdvancedDisplay)
	TObjectPtr<UAnglesConstraintModifier> RPGAnglesConstraintModifier;
	
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Internal|RPGModifiers|CameraCollisions", AdvancedDisplay)
	TObjectPtr<URPG_CameraCollisionModifier> RPGCameraCollisionModifier;
	
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Internal|RPGModifiers|CameraDithering", AdvancedDisplay)
	TObjectPtr<URPG_CameraDitheringModifier> RPGCameraDitheringModifier;
	
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Internal|RPGModifiers|CameraPitchToArmFOV", AdvancedDisplay)
	TObjectPtr<UPitchToArmLengthAndFOVModifier> RPGPitchToArmLengthAndFOVModifier;
	
	/** The camera data to use in the default state */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category="Internal", meta=(ExposeOnSpawn="true"))
	TObjectPtr<URPG_CameraDataAssetBase> DefaultCameraData;
	
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Internal|RPGModifiers|CameraFocus", AdvancedDisplay)
	TObjectPtr<UFocusCameraModifier> RPGFocusCameraModifier;
	
	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Internal|RPGModifiers|CameraArm")
	TObjectPtr<UArmOffsetModifier> RPGArmOffsetModifier;
	
};
