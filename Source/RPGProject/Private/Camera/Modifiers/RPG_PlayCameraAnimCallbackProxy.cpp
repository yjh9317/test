// Copyright(c) Aurora Devs 2023. All Rights Reserved.

#include "Camera/Modifiers/RPG_PlayCameraAnimCallbackProxy.h"
#include "Camera/PlayerCameraManager.h"

namespace PlayCameraAnimCallbackProxyHelper
{
    FCameraAnimationHandle const RPGInvalid(MAX_int16, 0);
}

URPG_PlayCameraAnimCallbackProxy::URPG_PlayCameraAnimCallbackProxy(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , bInterruptedCalledBeforeBlendingOut(false)
{
}

FRPGCameraAnimationParams::operator FCameraAnimationParams() const
{
    FCameraAnimationParams Params;
    Params.PlayRate = PlayRate;
    Params.EaseInDuration = EaseInDuration;
    Params.EaseOutDuration = EaseOutDuration;
    Params.EaseInType = EaseInType;
    Params.EaseOutType = EaseOutType;
    Params.bLoop = false;

    return Params;
}

URPG_PlayCameraAnimCallbackProxy* URPG_PlayCameraAnimCallbackProxy::CreateProxyObjectForPlayCameraAnim(APlayerCameraManager* InPlayerCameraManager, TSubclassOf<URPG_CameraAnimationModifier> ModifierClass, UCameraAnimationSequence* CameraSequence, FRPGCameraAnimationParams Params, FCameraAnimationHandle& Handle)
{
    URPG_PlayCameraAnimCallbackProxy* Proxy = NewObject<URPG_PlayCameraAnimCallbackProxy>();
    Proxy->SetFlags(RF_StrongRefOnFrame);
    Proxy->PlayCameraAnimation(InPlayerCameraManager, ModifierClass, CameraSequence, Params, Handle);
    return Proxy;
}

URPG_PlayCameraAnimCallbackProxy* URPG_PlayCameraAnimCallbackProxy::CreateProxyObjectForPlayCameraAnimForModifier(URPG_CameraAnimationModifier* CameraAnimationModifier, UCameraAnimationSequence* CameraSequence, FRPGCameraAnimationParams Params, FCameraAnimationHandle& Handle)
{
    URPG_PlayCameraAnimCallbackProxy* Proxy = NewObject<URPG_PlayCameraAnimCallbackProxy>();
    Proxy->SetFlags(RF_StrongRefOnFrame);
    Proxy->PlayCameraAnimation(CameraAnimationModifier, CameraSequence, Params, Handle);
    return Proxy;
}

void URPG_PlayCameraAnimCallbackProxy::PlayCameraAnimation(URPG_CameraAnimationModifier* CameraAnimModifier, UCameraAnimationSequence* CameraSequence, FRPGCameraAnimationParams Params, FCameraAnimationHandle& Handle)
{
    Handle = PlayCameraAnimCallbackProxyHelper::RPGInvalid;
    bool bPlayedSuccessfully = false;

    if (CameraAnimModifier)
    {
        Handle = CameraAnimModifier->PlaySingleCameraAnimation(CameraSequence, static_cast<FCameraAnimationParams>(Params), Params.ResetType);
        bPlayedSuccessfully = Handle.IsValid();

        if (bPlayedSuccessfully)
        {
            CameraAnimationModifierPtr = CameraAnimModifier;

            CameraAnimationEasingOutDelegate.BindUObject(this, &URPG_PlayCameraAnimCallbackProxy::OnCameraAnimationEasingOut);
            CameraAnimationModifierPtr->CameraAnimation_SetEasingOutDelegate(CameraAnimationEasingOutDelegate, Handle);

            CameraAnimationEndedDelegate.BindUObject(this, &URPG_PlayCameraAnimCallbackProxy::OnCameraAnimationEnded);
            CameraAnimationModifierPtr->CameraAnimation_SetEndedDelegate(CameraAnimationEndedDelegate, Handle);
        }
    }

    if (!bPlayedSuccessfully)
    {
        OnInterrupted.Broadcast();
    }
}

void URPG_PlayCameraAnimCallbackProxy::PlayCameraAnimation(APlayerCameraManager* InPlayerCameraManager, TSubclassOf<URPG_CameraAnimationModifier> ModifierClass, UCameraAnimationSequence* CameraSequence, FRPGCameraAnimationParams Params, FCameraAnimationHandle& Handle)
{
    if (InPlayerCameraManager)
    {
        if (URPG_CameraAnimationModifier* CameraAnimModifier = Cast<URPG_CameraAnimationModifier>(InPlayerCameraManager->FindCameraModifierByClass(ModifierClass)))
        {
            PlayCameraAnimation(CameraAnimModifier, CameraSequence, Params, Handle);
        }
    }
}

void URPG_PlayCameraAnimCallbackProxy::OnCameraAnimationEasingOut(UCameraAnimationSequence* CameraAnimation)
{
    OnEaseOut.Broadcast();
}

void URPG_PlayCameraAnimCallbackProxy::OnCameraAnimationEnded(UCameraAnimationSequence* CameraAnimation, bool bInterrupted)
{
    if (!bInterrupted)
    {
        OnCompleted.Broadcast();
    }
    else if (!bInterruptedCalledBeforeBlendingOut)
    {
        OnInterrupted.Broadcast();
    }
}