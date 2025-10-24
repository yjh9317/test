// Copyright(c) Aurora Devs 2023. All Rights Reserved.

#include "Camera/RPG_PlayerCameraManager.h"

#include "Camera/Data/RPG_CameraData.h"
#include "Camera/Modifiers/RPG_CameraAnimationModifier.h"
#include "Camera/Modifiers/RPG_CameraModifier.h"
#include "Camera/Modifiers/RPG_PlayCameraAnimCallbackProxy.h"
#include "CameraAnimationSequence.h"
#include "DrawDebugHelpers.h"
#include "Engine.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Interface/RPG_PawnInterface.h"
#include "Kismet/KismetMathLibrary.h"

ARPG_PlayerCameraManager::ARPG_PlayerCameraManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ARPG_PlayerCameraManager::InitializeFor(APlayerController* PC)
{
    Super::InitializeFor(PC);
    RefreshLevelSequences();
}

void ARPG_PlayerCameraManager::RefreshLevelSequences()
{
    // This resets the array and gets all actors of class.
    QUICK_SCOPE_CYCLE_COUNTER(ARPG_PlayerCameraManager_RefreshLevelSequences);
    LevelSequences.Reset();

    for (TActorIterator<ALevelSequenceActor> It(GetWorld()); It; ++It)
    {
        ALevelSequenceActor* LevelSequence = *It;
        LevelSequences.Add(LevelSequence);

        LevelSequence->SequencePlayer->OnPlay.AddDynamic(this, &ARPG_PlayerCameraManager::OnLevelSequenceStarted);
        LevelSequence->SequencePlayer->OnPlayReverse.AddDynamic(this, &ARPG_PlayerCameraManager::OnLevelSequenceStarted);
        LevelSequence->SequencePlayer->OnStop.AddDynamic(this, &ARPG_PlayerCameraManager::OnLevelSequenceEnded);
    }
}

void ARPG_PlayerCameraManager::OnLevelSequenceStarted()
{
    ++NbrActiveLevelSequences;
    DoForEachRPGModifier(&URPG_CameraModifier::OnAnyLevelSequenceStarted);
}

void ARPG_PlayerCameraManager::OnLevelSequenceEnded()
{
    --NbrActiveLevelSequences;
    ensure(NbrActiveLevelSequences >= 0);
    DoForEachRPGModifier(&URPG_CameraModifier::OnAnyLevelSequenceEnded);
}

void ARPG_PlayerCameraManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    UpdateInternalVariables(DeltaTime);
}

bool ARPG_PlayerCameraManager::IsPlayingAnyCameraAnimation() const
{
    if (URPG_CameraAnimationModifier const* CameraAnimModifier = FindCameraModifierOfType<URPG_CameraAnimationModifier>())
    {
        return CameraAnimModifier && CameraAnimModifier->IsAnyCameraAnimationSequence();
    }
    return false;
}

void ARPG_PlayerCameraManager::PlayCameraAnimation(UCameraAnimationSequence* CameraSequence, FRPGCameraAnimationParams const& Params)
{
    if (URPG_CameraAnimationModifier* CameraAnimModifier = FindCameraModifierOfType<URPG_CameraAnimationModifier>())
    {
        CameraAnimModifier->PlaySingleCameraAnimation(CameraSequence, static_cast<FCameraAnimationParams>(Params), Params.ResetType);
    }
}

void ARPG_PlayerCameraManager::SetViewTarget(AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams)
{
    auto OldPendingTarget = PendingViewTarget.Target;
    auto OldTarget = ViewTarget.Target;

    Super::SetViewTarget(NewViewTarget, TransitionParams);

    if (!OwnerPawn)
    {
        return;
    }

    bool const bAssignedNewTarget = ViewTarget.Target != OldTarget;
    bool const bBlendingToNewTarget = PendingViewTarget.Target != OldPendingTarget;
    if (bAssignedNewTarget || bBlendingToNewTarget)
    {
        bool const bWasImmediate = bAssignedNewTarget && !bBlendingToNewTarget;
        bool bNewTargetIsOwner = false;

        // If old character has been unpossessed, then our new taget is the owner!
        if (bWasImmediate && OwnerPawn->GetController() == nullptr)
        {
            bNewTargetIsOwner = true;
        }
        else
        {
            bNewTargetIsOwner = bWasImmediate ? ViewTarget.Target == OwnerPawn : PendingViewTarget.Target == OwnerPawn;
        }

        DoForEachRPGModifier([bNewTargetIsOwner, bWasImmediate](URPG_CameraModifier* RPGModifier)
        {
            RPGModifier->OnSetViewTarget(bWasImmediate, bNewTargetIsOwner);
        });
    }
}

UCameraModifier* ARPG_PlayerCameraManager::FindCameraModifierOfClass(TSubclassOf<UCameraModifier> ModifierClass, bool bIncludeInherited)
{
    for (UCameraModifier* Mod : ModifierList)
    {
        if (bIncludeInherited)
        {
            if (Mod->GetClass()->IsChildOf(ModifierClass))
            {
                return Mod;
            }
        }
        else
        {
            if (Mod->GetClass() == ModifierClass)
            {
                return Mod;
            }
        }
    }

    return nullptr;
}

UCameraModifier const* ARPG_PlayerCameraManager::FindCameraModifierOfClass(TSubclassOf<UCameraModifier> ModifierClass, bool bIncludeInherited) const
{
    for (UCameraModifier* Mod : ModifierList)
    {
        if (bIncludeInherited)
        {
            if (Mod->GetClass()->IsChildOf(ModifierClass))
            {
                return Mod;
            }
        }
        else
        {
            if (Mod->GetClass() == ModifierClass)
            {
                return Mod;
            }
        }
    }

    return nullptr;
}

void ARPG_PlayerCameraManager::ToggleRPGCameraModifiers(bool const bEnabled, bool const bImmediate)
{
    DoForEachRPGModifier([bEnabled, bImmediate](URPG_CameraModifier* RPGModifier)
    {
        if (bEnabled)
        {
            RPGModifier->EnableModifier();
        }
        else
        {
            RPGModifier->DisableModifier(bImmediate);
        }
    });
}

void ARPG_PlayerCameraManager::ToggleCameraModifiers(bool const bEnabled, bool const bImmediate)
{
    for (int32 ModifierIdx = 0; ModifierIdx < ModifierList.Num(); ModifierIdx++)
    {
        if (ModifierList[ModifierIdx] != nullptr)
        {
            if (bEnabled)
            {
                ModifierList[ModifierIdx]->EnableModifier();
            }
            else
            {
                ModifierList[ModifierIdx]->DisableModifier(bImmediate);
            }
        }
    }
}

void ARPG_PlayerCameraManager::ToggleAllRPGModifiersDebug(bool const bEnabled)
{
    DoForEachRPGModifier([bEnabled](URPG_CameraModifier* RPGModifier)
    {
        if (!RPGModifier->IsDisabled())
        {
            RPGModifier->bDebug = bEnabled;
        }
    });
}

void ARPG_PlayerCameraManager::ToggleAllModifiersDebug(bool const bEnabled)
{
    for (int32 ModifierIdx = 0; ModifierIdx < ModifierList.Num(); ModifierIdx++)
    {
        if (ModifierList[ModifierIdx] != nullptr && !ModifierList[ModifierIdx]->IsDisabled())
        {
            ModifierList[ModifierIdx]->bDebug = bEnabled;
        }
    }
}

void ARPG_PlayerCameraManager::PushCameraData(URPG_CameraDataAssetBase* CameraDA)
{
    CameraDataStack.Push(CameraDA);
    OnCameraDataStackChanged(CameraDA);
}

void ARPG_PlayerCameraManager::PopCameraDataHead()
{
    CameraDataStack.Pop();
    OnCameraDataStackChanged(CameraDataStack.IsEmpty() ? nullptr : CameraDataStack[CameraDataStack.Num() - 1]);
}

void ARPG_PlayerCameraManager::PopCameraData(URPG_CameraDataAssetBase* CameraDA)
{
    if (CameraDataStack.IsEmpty())
    {
        return;
    }

    if (GetCurrentCameraDataAsset() == CameraDA)
    {
        PopCameraDataHead();
    }

    CameraDataStack.Remove(CameraDA);
}

void ARPG_PlayerCameraManager::OnCameraDataStackChanged_Implementation(URPG_CameraDataAssetBase* CameraDA)
{
}

void ARPG_PlayerCameraManager::ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot)
{
    Super::ProcessViewRotation(DeltaTime, OutViewRotation, OutDeltaRot);

    if (PCOwner && ViewTarget.Target)
    {
        FRotator const ControlRotation = PCOwner->GetControlRotation();
        FRotator const OwnerRotation = ViewTarget.Target->GetActorRotation();
        FRotator InLocalControlRotation = ControlRotation - OwnerRotation;
        InLocalControlRotation.Normalize();

        float OutPitchTurnRate = PitchTurnRate;
        float OutYawTurnRate = YawTurnRate;

        ProcessTurnRate(DeltaTime, InLocalControlRotation, OutPitchTurnRate, OutYawTurnRate);

        PitchTurnRate = FMath::Clamp(OutPitchTurnRate, 0.f, 1.f);
        YawTurnRate = FMath::Clamp(OutYawTurnRate, 0.f, 1.f);
    }
}

void ARPG_PlayerCameraManager::ProcessTurnRate(float DeltaTime, FRotator InLocalControlRotation, float& OutPitchTurnRate, float& OutYawTurnRate)
{
    DoForEachRPGModifierWithBreak([&](URPG_CameraModifier* RPGModifier) -> bool
    {
        if (!RPGModifier->IsDisabled())
        {
            if (!RPGModifier->CanPlayDuringCameraAnimation())
            {
                if (IsPlayingAnyCameraAnimation())
                {
                    return false;
                }
            }

            return RPGModifier->ProcessTurnRate(DeltaTime, InLocalControlRotation, PitchTurnRate, YawTurnRate, OutPitchTurnRate, OutYawTurnRate);
        }
        return false; // Don't break
    });
}

void ARPG_PlayerCameraManager::UpdateInternalVariables_Implementation(float DeltaTime)
{
    AspectRatio = GetCameraCacheView().AspectRatio;
    HorizontalFOV = GetFOVAngle();
    ensureAlways(!FMath::IsNearlyZero(AspectRatio));
    VerticalFOV = FMath::RadiansToDegrees(2.f * FMath::Atan(FMath::Tan(FMath::DegreesToRadians(HorizontalFOV) * 0.5f) / AspectRatio));

    if (PCOwner && PCOwner->GetPawn())
    {
        APawn* OwnerPawnNew = PCOwner->GetPawn();
        if (!OwnerPawn || OwnerPawnNew != OwnerPawn)
        {
            // Either initialising reference, or we have possessed a new character.
            OwnerPawn = OwnerPawnNew;
            OwnerCharacter = PCOwner->GetPawn<ACharacter>();
            if (OwnerCharacter)
            {
                MovementComponent = OwnerCharacter->GetCharacterMovement();
            }
            CameraArm = OwnerPawn->FindComponentByClass<USpringArmComponent>();

        }

        if (OwnerPawn)
        {
            MovementInput = GetMovementControlInput();
            bHasMovementInput = !MovementInput.IsZero();
            TimeSinceMovementInput = bHasMovementInput ? 0.f : TimeSinceMovementInput + DeltaTime;

            RotationInput = GetRotationInput();
            bHasRotationInput = !RotationInput.IsZero();
            TimeSinceRotationInput = bHasRotationInput ? 0.f : TimeSinceRotationInput + DeltaTime;
        }
    }
}

FRotator ARPG_PlayerCameraManager::GetRotationInput_Implementation() const
{
    FRotator RotInput = FRotator::ZeroRotator;

    if (OwnerPawn && OwnerPawn->GetClass()->ImplementsInterface(URPG_PawnInterface::StaticClass()))
    {
        RotInput = IRPG_PawnInterface::Execute_GetRotationInput(OwnerPawn);
    }
    return RotInput;
    // return FRotator::ZeroRotator;
}

FVector ARPG_PlayerCameraManager::GetMovementControlInput_Implementation() const
{
    FVector MovInput = FVector::ZeroVector;

    if (OwnerPawn && OwnerPawn->GetClass()->ImplementsInterface(URPG_PawnInterface::StaticClass()))
    {
        MovInput = IRPG_PawnInterface::Execute_GetMovementInput(OwnerPawn);
    }
    return MovInput;
    // return FVector::ZeroVector;
}

// Limit the view yaw in local space instead of world space.
void ARPG_PlayerCameraManager::LimitViewYaw(FRotator& ViewRotation, float InViewYawMin, float InViewYawMax)
{
    if (PCOwner && PCOwner->GetPawn())
    {
        FRotator ActorRotation = PCOwner->GetPawn()->GetActorRotation();
        ViewRotation.Yaw = FMath::ClampAngle(ViewRotation.Yaw, ActorRotation.Yaw + InViewYawMin, ActorRotation.Yaw + InViewYawMax);
        ViewRotation.Yaw = FRotator::ClampAxis(ViewRotation.Yaw);
    }
}

/** Draw a debug camera shape. */
void ARPG_PlayerCameraManager::DrawDebRPGamera(float Duration, bool bDrawCamera, FLinearColor CameraColor, bool bDrawSpringArm, FLinearColor SpringArmColor, float Thickness)
{
#if ENABLE_DRAW_DEBUG
    if (bDrawSpringArm && CameraArm)
    {
        FVector const SafeLocation = CameraArm->GetComponentLocation() + CameraArm->TargetOffset;
        ::DrawDebugLine(GetWorld(), GetCameraCacheView().Location, SafeLocation, SpringArmColor.ToFColor(true), false, Duration, 0, Thickness);
    }

    // if (bDrawCamera)
    // {
    //     ::DrawDebRPGCamera(GetWorld(), GetCameraCacheView().Location, GetCameraCacheView().Rotation, GetCameraCacheView().FOV, 1.0f, CameraColor.ToFColor(true), false, Duration);
    // }
#endif
}

void ARPG_PlayerCameraManager::DoForEachRPGModifier(TFunction<void(URPG_CameraModifier*)> const& Function)
{
    if (Function)
    {
        for (int32 ModifierIdx = 0; ModifierIdx < RPGModifiersList.Num(); ++ModifierIdx)
        {
            ensure(RPGModifiersList[ModifierIdx]);

            if (RPGModifiersList[ModifierIdx])
            {
                Function(RPGModifiersList[ModifierIdx]);
            }
        }
    }
}

void ARPG_PlayerCameraManager::DoForEachRPGModifierWithBreak(TFunction<bool(URPG_CameraModifier*)> const& Function)
{
    if (Function)
    {
        for (int32 ModifierIdx = 0; ModifierIdx < RPGModifiersList.Num(); ++ModifierIdx)
        {
            ensure(RPGModifiersList[ModifierIdx]);

            if (RPGModifiersList[ModifierIdx])
            {
                if (Function(RPGModifiersList[ModifierIdx]))
                {
                    break;
                }
            }
        }
    }
}

UCameraModifier* ARPG_PlayerCameraManager::AddNewCameraModifier(TSubclassOf<UCameraModifier> ModifierClass)
{
    UCameraModifier* AddedModifier = Super::AddNewCameraModifier(ModifierClass);
    if (AddedModifier)
    {
        if (URPG_CameraModifier* RPGModifier = Cast<URPG_CameraModifier>(AddedModifier))
        {
            RPGModifiersList.Add(RPGModifier);
        }
    }
    return AddedModifier;
}

bool ARPG_PlayerCameraManager::RemoveCameraModifier(UCameraModifier* ModifierToRemove)
{
    if (ModifierToRemove)
    {
        if (URPG_CameraModifier* RPGModifierToRemove = Cast<URPG_CameraModifier>(ModifierToRemove))
        {
            // Loop through each modifier in camera
            for (int32 ModifierIdx = 0; ModifierIdx < RPGModifiersList.Num(); ++ModifierIdx)
            {
                // If we found ourselves, remove ourselves from the list and return
                if (RPGModifiersList[ModifierIdx] == RPGModifierToRemove)
                {
                    RPGModifiersList.RemoveAt(ModifierIdx, 1);
                    break;
                }
            }
        }
    }
    return Super::RemoveCameraModifier(ModifierToRemove);
}

FVector ARPG_PlayerCameraManager::GetOwnerVelocity() const
{
    FVector Velocity = FVector::ZeroVector;
    if (MovementComponent)
    {
        Velocity = MovementComponent->Velocity;
    }
    // else if (OwnerPawn && OwnerPawn->GetClass()->ImplementsInterface(URPG_PawnMovementInterface::StaticClass()))
    // {
    //     Velocity = IRPG_PawnMovementInterface::Execute_GetOwnerVelocity(OwnerPawn);
    // }
    return Velocity;
}

bool ARPG_PlayerCameraManager::IsOwnerFalling() const
{
    bool bIsFalling = false;
    if (MovementComponent)
    {
        bIsFalling = MovementComponent->IsFalling();
    }
    // else if (OwnerPawn && OwnerPawn->GetClass()->ImplementsInterface(URPG_PawnMovementInterface::StaticClass()))
    // {
    //     bIsFalling = IRPG_PawnMovementInterface::Execute_IsOwnerFalling(OwnerPawn);
    // }
    return bIsFalling;
}

bool ARPG_PlayerCameraManager::IsOwnerStrafing() const
{
    bool bIsStrafing = false;
    if (MovementComponent && OwnerPawn)
    {
        bIsStrafing = OwnerPawn->bUseControllerRotationYaw || (MovementComponent->bUseControllerDesiredRotation && !MovementComponent->bOrientRotationToMovement);
    }
    else if (OwnerPawn)
    {
        // if (OwnerPawn->GetClass()->ImplementsInterface(URPG_PawnMovementInterface::StaticClass()))
        // {
        //     bIsStrafing = IRPG_PawnMovementInterface::Execute_IsOwnerStrafing(OwnerPawn);
        // }
        // else
        // {
        //     bIsStrafing = OwnerPawn->bUseControllerRotationYaw;
        // }
    }
    return bIsStrafing;
}

bool ARPG_PlayerCameraManager::IsOwnerMovingOnGround() const
{
    bool bIsMovingOnGround = false;
    if (MovementComponent)
    {
        bIsMovingOnGround = MovementComponent->IsMovingOnGround();
    }
    // else if (OwnerPawn && OwnerPawn->GetClass()->ImplementsInterface(URPG_PawnMovementInterface::StaticClass()))
    // {
    //     bIsMovingOnGround = IRPG_PawnMovementInterface::Execute_IsOwnerMovingOnGround(OwnerPawn);
    // }
    return bIsMovingOnGround;
}

void ARPG_PlayerCameraManager::ComputeOwnerFloorDist(float SweepDistance, float CapsuleRadius, bool& bOutFloorExists, float& OutFloorDistance) const
{
    if (MovementComponent && OwnerCharacter)
    {
        CapsuleRadius = FMath::Max(CapsuleRadius, OwnerCharacter->GetCapsuleComponent()->GetScaledCapsuleRadius());
        FFindFloorResult OutFloorResult;
        MovementComponent->ComputeFloorDist(OwnerPawn->GetActorLocation(), SweepDistance, SweepDistance, OutFloorResult, CapsuleRadius);
        bOutFloorExists = OutFloorResult.bBlockingHit;
        OutFloorDistance = bOutFloorExists ? OutFloorResult.FloorDist : 0.f;
    }
    else
    {
        FHitResult OutHit;
        bOutFloorExists = GetWorld()->SweepSingleByChannel(OutHit
            , OwnerPawn->GetActorLocation()
            , OwnerPawn->GetActorLocation() - SweepDistance * FVector::UpVector
            , FQuat::Identity
            , ECollisionChannel::ECC_Visibility
            , FCollisionShape::MakeSphere(CapsuleRadius));

        OutFloorDistance = bOutFloorExists ? OutHit.Distance : 0.f;
    }
}

void ARPG_PlayerCameraManager::ComputeOwnerFloorNormal(float SweepDistance, float CapsuleRadius, bool& bOutFloorExists, FVector& OutFloorNormal) const
{
    if (MovementComponent && OwnerCharacter)
    {
        bOutFloorExists = MovementComponent->CurrentFloor.IsWalkableFloor();
        OutFloorNormal = MovementComponent->CurrentFloor.HitResult.ImpactNormal;
    }
    else
    {
        FHitResult OutHit;
        bOutFloorExists = GetWorld()->SweepSingleByChannel(OutHit
            , OwnerPawn->GetActorLocation()
            , OwnerPawn->GetActorLocation() - SweepDistance * FVector::UpVector
            , FQuat::Identity
            , ECollisionChannel::ECC_Visibility
            , FCollisionShape::MakeSphere(CapsuleRadius));

        bOutFloorExists = OutHit.bBlockingHit;
        OutFloorNormal = bOutFloorExists ? OutHit.ImpactNormal : FVector::ZeroVector;
    }
}

void ARPG_PlayerCameraManager::ComputeOwnerSlopeAngle(float& OutSlopePitchDegrees, float& OutSlopeRollDegrees)
{
    bool bOutFloorExists = false;
    FVector OutFloorNormal = FVector::ZeroVector;
    ComputeOwnerFloorNormal(96.f, 64.f, bOutFloorExists, OutFloorNormal);
    UKismetMathLibrary::GetSlopeDegreeAngles(OwnerPawn->GetActorRightVector(), OutFloorNormal, OwnerPawn->GetActorUpVector(), OutSlopePitchDegrees, OutSlopeRollDegrees);
}

float ARPG_PlayerCameraManager::ComputeOwnerLookAndMovementDot()
{
    if (IsOwnerStrafing())
    {
        return 1.f;
    }

    FVector const Velocity = GetOwnerVelocity();
    if (Velocity.IsNearlyZero())
    {
        return 0.f;
    }

    float const Dot = Velocity | OwnerPawn->GetControlRotation().Vector();
    return Dot;
}