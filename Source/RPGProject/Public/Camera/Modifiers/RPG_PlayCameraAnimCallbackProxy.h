// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"
#include "UObject/ScriptMacros.h"
#include "RPG_CameraAnimationModifier.h"
#include "RPG_PlayCameraAnimCallbackProxy.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCameraAnimationPlayDelegate);

/** Parameter struct for adding new camera animations to RPGCameraAnimationCameraModifier */
USTRUCT(BlueprintType)
struct FRPGCameraAnimationParams
{
	GENERATED_BODY()

	/** Time scale for playing the new camera animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Animation")
	float PlayRate = 1.f;

	/** Ease-in function type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Animation")
	ECameraAnimationEasingType EaseInType = ECameraAnimationEasingType::Linear;
	/** Ease-in duration in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Animation")
	float EaseInDuration = 0.f;

	/** Ease-out function type */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Animation")
	ECameraAnimationEasingType EaseOutType = ECameraAnimationEasingType::Linear;
	/** Ease-out duration in seconds */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Animation")
	float EaseOutDuration = 0.f;

	/** How should the camera behave after the animation is over. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera Animation")
	ECameraAnimationResetType ResetType = ECameraAnimationResetType::ResetToZero;

	explicit operator FCameraAnimationParams() const;
};

/**
 * 
 */
UCLASS()
class RPGPROJECT_API URPG_PlayCameraAnimCallbackProxy : public UObject
{
	GENERATED_BODY()

	URPG_PlayCameraAnimCallbackProxy(const FObjectInitializer& ObjectInitializer);
    // Called when Camera Animation finished playing and wasn't interrupted
    UPROPERTY(BlueprintAssignable)
    FOnCameraAnimationPlayDelegate OnCompleted;

    // Called when Camera Animation starts blending out and is not interrupted
    UPROPERTY(BlueprintAssignable)
    FOnCameraAnimationPlayDelegate OnEaseOut;

    // Called when Camera Animation has been interrupted (or failed to play)
    UPROPERTY(BlueprintAssignable)
    FOnCameraAnimationPlayDelegate OnInterrupted;

    // Called to perform the query internally
    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
        static URPG_PlayCameraAnimCallbackProxy* CreateProxyObjectForPlayCameraAnim(
            class APlayerCameraManager* InPlayerCameraManager,
            TSubclassOf<class URPG_CameraAnimationModifier> ModifierClass,
            class UCameraAnimationSequence* CameraSequence,
            FRPGCameraAnimationParams Params,
            struct FCameraAnimationHandle& Handle);

    UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
        static URPG_PlayCameraAnimCallbackProxy* CreateProxyObjectForPlayCameraAnimForModifier(
            class URPG_CameraAnimationModifier* CameraAnimationModifier,
            class UCameraAnimationSequence* CameraSequence,
            FRPGCameraAnimationParams Params,
            struct FCameraAnimationHandle& Handle);

protected:
    UFUNCTION()
        void OnCameraAnimationEasingOut(UCameraAnimationSequence* CameraAnimation);

    UFUNCTION()
        void OnCameraAnimationEnded(UCameraAnimationSequence* CameraAnimation, bool bInterrupted);

private:
    TWeakObjectPtr<class URPG_CameraAnimationModifier> CameraAnimationModifierPtr;

    bool bInterruptedCalledBeforeBlendingOut = false;

    FOnCameraAnimationEaseOutStarted CameraAnimationEasingOutDelegate;
    FOnCameraAnimationEnded CameraAnimationEndedDelegate;

    void PlayCameraAnimation(
        class APlayerCameraManager* InPlayerCameraManager,
        TSubclassOf<class URPG_CameraAnimationModifier> ModifierClass,
        class UCameraAnimationSequence* CameraSequence,
        FRPGCameraAnimationParams Params,
        struct FCameraAnimationHandle& Handle);

    void PlayCameraAnimation(
        class URPG_CameraAnimationModifier* CameraAnimationModifier,
        class UCameraAnimationSequence* CameraSequence,
        FRPGCameraAnimationParams Params,
        struct FCameraAnimationHandle& Handle);
};
