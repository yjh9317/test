// Copyright(c) Aurora Devs 2023. All Rights Reserved.

#include "Camera/Modifiers/RPG_CameraModifier.h"
#include "Camera/Modifiers/RPG_CameraAnimationModifier.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/RPG_PlayerCameraManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"

void URPG_CameraModifier::EnableModifier()
{
    Super::EnableModifier();
    OnModifierEnabled(CameraOwner->GetCameraCacheView());
}

void URPG_CameraModifier::DisableModifier(bool bImmediate)
{
    Super::DisableModifier(bImmediate);

    if (bImmediate && bDisabled && !bPendingDisable)
    {
        Alpha = 0.f;
    }
    OnModifierDisabled(CameraOwner->GetCameraCacheView(), bImmediate);
}

void URPG_CameraModifier::OnModifierEnabled_Implementation(FMinimalViewInfo const& LastPOV)
{
}

void URPG_CameraModifier::OnModifierDisabled_Implementation(FMinimalViewInfo const& LastPOV, bool bImmediate)
{
}

void URPG_CameraModifier::ProcessBoomLengthAndFOV_Implementation(float DeltaTime, float InFOV, float InArmLength, FVector ViewLocation, FRotator ViewRotation, float& OutFOV, float& OutArmLength)
{
    OutFOV = InFOV;
    OutArmLength = InArmLength;
}

void URPG_CameraModifier::ProcessBoomOffsets_Implementation(float DeltaTime, FVector InSocketOffset, FVector InTargetOffset, FVector ViewLocation, FRotator ViewRotation, FVector& OutSocketOffset, FVector& OutTargetOffset)
{
    OutSocketOffset = InSocketOffset;
    OutTargetOffset = InTargetOffset;
}

void URPG_CameraModifier::OnAnyLevelSequenceStarted_Implementation()
{
}

void URPG_CameraModifier::OnAnyLevelSequenceEnded_Implementation()
{
}

void URPG_CameraModifier::OnSetViewTarget_Implementation(bool bImmediate, bool bNewTargetIsOwner)
{
}

bool URPG_CameraModifier::IsDebugEnabled() const
{
    return bDebug;
}

void URPG_CameraModifier::ToggleDebug(bool const bEnabled)
{
    bDebug = bEnabled;
}

bool URPG_CameraModifier::ModifyCamera(float DeltaTime, FMinimalViewInfo& InOutPOV)
{
    return Super::ModifyCamera(DeltaTime, InOutPOV);
}

void URPG_CameraModifier::ModifyCamera(float DeltaTime, FVector ViewLocation, FRotator ViewRotation, float FOV, FVector& OutViewLocation, FRotator& OutViewRotation, float& OutFOV)
{
    Super::ModifyCamera(DeltaTime, ViewLocation, ViewRotation, FOV, OutViewLocation, OutViewRotation, OutFOV);

    OutViewLocation = ViewLocation;
    OutViewRotation = ViewRotation;
    OutFOV = FOV;

    UpdateOwnerReferences();

    if (!RPGCameraManager)
    {
        return;
    }

    if (!bPlayDuringCameraAnimations)
    {
        if (RPGCameraManager->IsPlayingAnyCameraAnimation())
        {
            return;
        }
    }

    if (OwnerController && OwnerPawn && OwnerPawn->IsLocallyControlled())
    {
        UpdateInternalVariables(DeltaTime);
        if (SpringArm)
        {
            ProcessBoomLengthAndFOV(DeltaTime, FOV, CurrentArmLength, ViewLocation, ViewRotation, OutFOV, SpringArm->TargetArmLength);
            ProcessBoomOffsets(DeltaTime, CurrentSocketOffset, CurrentTargetOffset, ViewLocation, ViewRotation, SpringArm->SocketOffset, SpringArm->TargetOffset);
        }
    }
}

bool URPG_CameraModifier::ProcessViewRotation(AActor* ViewTarget, float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
    bool bResult = Super::ProcessViewRotation(ViewTarget, DeltaTime, OutViewRotation, OutDeltaRot);

    if (!bPlayDuringCameraAnimations)
    {
        if (RPGCameraManager && RPGCameraManager->IsPlayingAnyCameraAnimation())
        {
            return bResult;
        }
    }

    if (ViewTarget && CameraOwner && CameraOwner->GetOwningPlayerController())
    {
        FVector const CameraLocation = CameraOwner->GetCameraLocation();
        FRotator const ControlRotation = CameraOwner->GetOwningPlayerController()->GetControlRotation();
        FRotator const OwnerRotation = ViewTarget ? ViewTarget->GetActorRotation() : FRotator::ZeroRotator;
        FRotator InLocalControlRotation = ControlRotation - OwnerRotation;
        InLocalControlRotation.Normalize();
        bResult = ProcessControlRotation(ViewTarget, DeltaTime, CameraLocation, OutViewRotation, InLocalControlRotation, OutDeltaRot, OutDeltaRot);
    }

    return bResult;
}

void URPG_CameraModifier::UpdateOwnerReferences()
{
    RPGCameraManager = Cast<ARPG_PlayerCameraManager>(CameraOwner);
    ensureMsgf(RPGCameraManager, TEXT("Please use RPG Camera Modifiers only with a player camera manager inheriting from RPG_PlayerCameraManager."));
    if (!RPGCameraManager)
    {
        return;
    }

    OwnerController = RPGCameraManager->GetOwningPlayerController();
    OwnerCharacter = RPGCameraManager->OwnerCharacter;
    OwnerPawn = RPGCameraManager->OwnerPawn;
    if (OwnerPawn && OwnerPawn->IsLocallyControlled())
    {
        SpringArm = RPGCameraManager->CameraArm;
        MovementComponent = RPGCameraManager->MovementComponent;
    }
}

void URPG_CameraModifier::UpdateInternalVariables(float DeltaTime)
{
    if (!RPGCameraManager)
    {
        return;
    }

    bHasMovementInput = RPGCameraManager->bHasMovementInput;
    PreviousMovementInput = MovementInput;
    MovementInput = RPGCameraManager->MovementInput;
    TimeSinceMovementInput = RPGCameraManager->TimeSinceMovementInput;
    bHasRotationInput = RPGCameraManager->bHasRotationInput;
    RotationInput = RPGCameraManager->RotationInput;
    TimeSinceRotationInput = RPGCameraManager->TimeSinceRotationInput;

    if (SpringArm)
    {
        CurrentSocketOffset = SpringArm->SocketOffset;
        CurrentTargetOffset = SpringArm->TargetOffset;
        CurrentArmLength = SpringArm->TargetArmLength;
    }
}

bool URPG_CameraModifier::ProcessTurnRate_Implementation(float DeltaTime, FRotator InLocalControlRotation, float InPitchTurnRate, float InYawTurnRate, float& OutPitchTurnRate, float& OutYawTurnRate)
{
    return false;
}

bool URPG_CameraModifier::ProcessControlRotation_Implementation(AActor* ViewTarget, float DeltaTime, FVector InViewLocation, FRotator InViewRotation, FRotator InLocalControlRotation, FRotator InDeltaRot, FRotator& OutDeltaRot)
{
    return false;
}

FVector URPG_CameraModifier::GetOwnerVelocity() const
{
    ensureMsgf(RPGCameraManager, TEXT("Please use RPG Camera Modifiers only with a player camera manager inheriting from RPG_PlayerCameraManager."));
    return RPGCameraManager->GetOwnerVelocity();
}

bool URPG_CameraModifier::IsOwnerFalling() const
{
    ensureMsgf(RPGCameraManager, TEXT("Please use RPG Camera Modifiers only with a player camera manager inheriting from RPG_PlayerCameraManager."));
    return RPGCameraManager->IsOwnerFalling();
}

bool URPG_CameraModifier::IsOwnerStrafing() const
{
    ensureMsgf(RPGCameraManager, TEXT("Please use RPG Camera Modifiers only with a player camera manager inheriting from RPG_PlayerCameraManager."));
    return RPGCameraManager->IsOwnerStrafing();
}

bool URPG_CameraModifier::IsOwnerMovingOnGround() const
{
    ensureMsgf(RPGCameraManager, TEXT("Please use RPG Camera Modifiers only with a player camera manager inheriting from RPG_PlayerCameraManager."));
    return RPGCameraManager->IsOwnerMovingOnGround();
}

void URPG_CameraModifier::ComputeOwnerFloorDistance(float SweepDistance, float CapsuleRadius, bool& bOutFloorExists, float& OutFloorDistance) const
{
    ensureMsgf(RPGCameraManager, TEXT("Please use RPG Camera Modifiers only with a player camera manager inheriting from RPG_PlayerCameraManager."));
    return RPGCameraManager->ComputeOwnerFloorDist(SweepDistance, CapsuleRadius, bOutFloorExists, OutFloorDistance);
}

void URPG_CameraModifier::ComputeOwnerFloorNormal(float SweepDistance, float CapsuleRadius, bool& bOutFloorExists,FVector& OutFloorNormal) const
{
    ensureMsgf(RPGCameraManager, TEXT("Please use RPG Camera Modifiers only with a player camera manager inheriting from RPG_PlayerCameraManager."));
    return RPGCameraManager->ComputeOwnerFloorNormal(SweepDistance, CapsuleRadius, bOutFloorExists, OutFloorNormal);
}

void URPG_CameraModifier::ComputeOwnerSlopeAngle(float& OutSlopePitchDegrees, float& OutSlopeRollDegrees)
{
    ensureMsgf(RPGCameraManager, TEXT("Please use RPG Camera Modifiers only with a player camera manager inheriting from RPG_PlayerCameraManager."));
    return RPGCameraManager->ComputeOwnerSlopeAngle(OutSlopePitchDegrees, OutSlopeRollDegrees);
}

float URPG_CameraModifier::ComputeOwnerLookAndMovementDot()
{
    ensureMsgf(RPGCameraManager, TEXT("Please use RPG Camera Modifiers only with a player camera manager inheriting from RPG_PlayerCameraManager."));
    return RPGCameraManager->ComputeOwnerLookAndMovementDot();
}
