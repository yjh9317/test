#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h" // FTableRowBase를 위해 추가
#include "LocomotionEnum.h"
#include "LocomotionStructs.generated.h"

class UBlendSpace1D;
class UAimOffsetBlendSpace;
class UAnimSequence; // UAnimSequence를 위해 추가

USTRUCT(BlueprintType)
struct FLocomotion_HandIKSettings
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool DisableLHandIK = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector EffectorLocation_HandL = FVector::ZeroVector;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector JointTargetLocation_HandL = FVector::ZeroVector;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    EHandIKBones JointTargetBone_HandL;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool DisableRHandIK = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector EffectorLocation_HandR = FVector::ZeroVector;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector JointTargetLocation_HandR = FVector::ZeroVector;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    EHandIKBones JointTargetBone_HandR;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float HandFKWeight = 0.f;

    bool operator==(const FLocomotion_HandIKSettings& Other) const
    {
        return DisableLHandIK == Other.DisableLHandIK &&
               EffectorLocation_HandL == Other.EffectorLocation_HandL &&
               JointTargetLocation_HandL == Other.JointTargetLocation_HandL &&
               JointTargetBone_HandL == Other.JointTargetBone_HandL &&
               DisableRHandIK == Other.DisableRHandIK &&
               EffectorLocation_HandR == Other.EffectorLocation_HandR &&
               JointTargetLocation_HandR == Other.JointTargetLocation_HandR &&
               JointTargetBone_HandR == Other.JointTargetBone_HandR &&
               HandFKWeight == Other.HandFKWeight;
    }
};

USTRUCT(BlueprintType)
struct FLocomotion_Config
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector2D PlayRateClampStarts = FVector2D::ZeroVector;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector2D PlayRateClampPivots = FVector2D::ZeroVector;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector2D PlayRateClampCycle = FVector2D::ZeroVector;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotion_HandIKSettings HandIKSettings;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool DisableLegIK = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool DisableLegIKWhileMoving = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MaxSpeed = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float MaxAcceleration = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BrakingDecelerationWalking = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float JumpRecoveryAdditiveAlpha = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FRotator RotationRate = FRotator::ZeroRotator;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector2D RotateRootYawOffsetAngleClamp = FVector2D::ZeroVector;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool DisableIdleBreaks = false;

    bool operator==(const FLocomotion_Config& Other) const
    {
        return PlayRateClampStarts == Other.PlayRateClampStarts &&
               PlayRateClampPivots == Other.PlayRateClampPivots &&
               PlayRateClampCycle == Other.PlayRateClampCycle &&
               HandIKSettings == Other.HandIKSettings &&
               DisableLegIK == Other.DisableLegIK &&
               DisableLegIKWhileMoving == Other.DisableLegIKWhileMoving &&
               MaxSpeed == Other.MaxSpeed &&
               MaxAcceleration == Other.MaxAcceleration &&
               BrakingDecelerationWalking == Other.BrakingDecelerationWalking &&
               JumpRecoveryAdditiveAlpha == Other.JumpRecoveryAdditiveAlpha &&
               RotationRate == Other.RotationRate &&
               RotateRootYawOffsetAngleClamp == Other.RotateRootYawOffsetAngleClamp &&
               DisableIdleBreaks == Other.DisableIdleBreaks;
    }
};


USTRUCT(BlueprintType)
struct FLocomotion_Jump
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimSequence> Jump_Start = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimSequence> Jump_StartLoop = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimSequence> Jump_Apex = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimSequence> Jump_FallLoop = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimSequence> Jump_FallLand = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimSequence> Jump_RecoveryAdditive = nullptr;

    bool operator==(const FLocomotion_Jump& Other) const
    {
        return Jump_Start == Other.Jump_Start &&
               Jump_StartLoop == Other.Jump_StartLoop &&
               Jump_Apex == Other.Jump_Apex &&
               Jump_FallLoop == Other.Jump_FallLoop &&
               Jump_FallLand == Other.Jump_FallLand &&
               Jump_RecoveryAdditive == Other.Jump_RecoveryAdditive;
    }
};

USTRUCT(BlueprintType)
struct FLocomotion_Sprint
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimSequence> Cycle = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimSequence> Start = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimSequence> Stop = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotion_Config Config;

    bool operator==(const FLocomotion_Sprint& Other) const
    {
        return Cycle == Other.Cycle &&
               Start == Other.Start &&
               Stop == Other.Stop &&
               Config == Other.Config;
    }
};

USTRUCT(BlueprintType)
struct FLocomotion_Starts
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimSequence> Start_Forward = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Start_Forward_L90"))
    TObjectPtr<UAnimSequence> Start_Forward_Left90 = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Start_Forward_R90"))
    TObjectPtr<UAnimSequence> Start_Forward_Right90 = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Start_Forward_Left180"))
    TObjectPtr<UAnimSequence> Start_Forward_Left180 = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Start_Forward_Right180"))
    TObjectPtr<UAnimSequence> Start_Forward_Right180 = nullptr;

    bool operator==(const FLocomotion_Starts& Other) const
    {
        return Start_Forward == Other.Start_Forward &&
               Start_Forward_Left90 == Other.Start_Forward_Left90 &&
               Start_Forward_Right90 == Other.Start_Forward_Right90 &&
               Start_Forward_Left180 == Other.Start_Forward_Left180 &&
               Start_Forward_Right180 == Other.Start_Forward_Right180;
    }
};

USTRUCT(BlueprintType)
struct FLocomotion_TurnInPlace
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimSequence> Left_90 = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimSequence> Right_90 = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float PlayRate = 0.f;

    bool operator==(const FLocomotion_TurnInPlace& Other) const
    {
        return Left_90 == Other.Left_90 &&
               Right_90 == Other.Right_90 &&
               PlayRate == Other.PlayRate;
    }
};


USTRUCT(BlueprintType)
struct FLocomotion_AimOffset
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAimOffsetBlendSpace> AimOffsetBlendSpace = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector2D YawRange = FVector2D::ZeroVector;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector2D PitchRange = FVector2D::ZeroVector;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FVector2D Offset = FVector2D::ZeroVector;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float InterpSpeed = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName BlendName = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BlendWeight = 0.f;

    bool operator==(const FLocomotion_AimOffset& Other) const
    {
        return AimOffsetBlendSpace == Other.AimOffsetBlendSpace &&
               YawRange == Other.YawRange &&
               PitchRange == Other.PitchRange &&
               Offset == Other.Offset &&
               InterpSpeed == Other.InterpSpeed &&
               BlendName == Other.BlendName &&
               BlendWeight == Other.BlendWeight;
    }
};

USTRUCT(BlueprintType)
struct FLocomotion_CardinalDirections
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimSequence> Forward = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimSequence> Backward = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimSequence> Left = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimSequence> Right = nullptr;

    bool operator==(const FLocomotion_CardinalDirections& Other) const
    {
        return Forward == Other.Forward &&
               Backward == Other.Backward &&
               Left == Other.Left &&
               Right == Other.Right;
    }
};

USTRUCT(BlueprintType)
struct FLocomotion_General
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimSequence> CrouchIdleEntry = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimSequence> CrouchIdleExit = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UBlendSpace1D> Leans = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotion_Sprint Sprint;

    bool operator==(const FLocomotion_General& Other) const
    {
        return CrouchIdleEntry == Other.CrouchIdleEntry &&
               CrouchIdleExit == Other.CrouchIdleExit &&
               Leans == Other.Leans &&
               Sprint == Other.Sprint;
    }
};



// ==========
// Override
// ==========

USTRUCT(BlueprintType)
struct FLocomotionOverride_Sequence
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimSequence> Sequence = nullptr;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float StartPosition = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float Override_Weight = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float InterpSpeed = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BlendTime = 0.f;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName BlendName = NAME_None;

    bool operator==(const FLocomotionOverride_Sequence& Other) const
    {
        return Sequence == Other.Sequence &&
               StartPosition == Other.StartPosition &&
               Override_Weight == Other.Override_Weight &&
               InterpSpeed == Other.InterpSpeed &&
               BlendTime == Other.BlendTime &&
               BlendName == Other.BlendName;
    }
};

USTRUCT(BlueprintType)
struct FLocomotionOverride_AimOffset
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bEnable = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotion_AimOffset AimOffset;

    bool operator==(const FLocomotionOverride_AimOffset& Other) const
    {
        return bEnable == Other.bEnable && AimOffset == Other.AimOffset;
    }
};

USTRUCT(BlueprintType)
struct FLocomotionOverride_Cardinals
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotionOverride_Sequence Start;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotionOverride_Sequence Stop;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotionOverride_Sequence Cycle;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotionOverride_Sequence Pivot;

    bool operator==(const FLocomotionOverride_Cardinals& Other) const
    {
        return Start == Other.Start &&
               Stop == Other.Stop &&
               Cycle == Other.Cycle &&
               Pivot == Other.Pivot;
    }
};

USTRUCT(BlueprintType)
struct FLocomotionOverride_Config
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool bEnable = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotion_Config Config;

    bool operator==(const FLocomotionOverride_Config& Other) const
    {
        return bEnable == Other.bEnable && Config == Other.Config;
    }
};

USTRUCT(BlueprintType)
struct FLocomotionOverride_ExtendedPose
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    bool DisableMoveAnim = false;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName ExtendedPoseName = NAME_None;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float OverrideWeight = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float InterpSpeed = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float BlendTime = 0.f;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FName BlendName = NAME_None;

    bool operator==(const FLocomotionOverride_ExtendedPose& Other) const
    {
        return DisableMoveAnim == Other.DisableMoveAnim &&
               ExtendedPoseName == Other.ExtendedPoseName &&
               OverrideWeight == Other.OverrideWeight &&
               InterpSpeed == Other.InterpSpeed &&
               BlendTime == Other.BlendTime &&
               BlendName == Other.BlendName;
    }
};

USTRUCT(BlueprintType)
struct FLocomotionOverride_Sprint
{
    GENERATED_BODY()
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotionOverride_Sequence Start;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotionOverride_Sequence Stop;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotionOverride_Sequence Cycle;

    bool operator==(const FLocomotionOverride_Sprint& Other) const
    {
        return Start == Other.Start &&
               Stop == Other.Stop &&
               Cycle == Other.Cycle;
    }
};

USTRUCT(BlueprintType)
struct FLocomotionOverride_Starts
{
    GENERATED_BODY()
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotionOverride_Sequence Forward;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Forward_L90"))
    FLocomotionOverride_Sequence Forward_Left90;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Forward_R90"))
    FLocomotionOverride_Sequence Forward_Right90;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Forward_L180"))
    FLocomotionOverride_Sequence Forward_Left180;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (DisplayName = "Forward_R180"))
    FLocomotionOverride_Sequence Forward_Right180;

    bool operator==(const FLocomotionOverride_Starts& Other) const
    {
        return Forward == Other.Forward &&
               Forward_Left90 == Other.Forward_Left90 &&
               Forward_Right90 == Other.Forward_Right90 &&
               Forward_Left180 == Other.Forward_Left180 &&
               Forward_Right180 == Other.Forward_Right180;
    }
};


USTRUCT(BlueprintType)
struct FLocomotionOverride
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotionOverride_Sequence GlobalPose;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotionOverride_Sequence IdlePose;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotionOverride_Sequence JumpPose;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotionOverride_Cardinals MovePose;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotionOverride_Starts StartsPose;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotionOverride_Sequence TurnPose;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotionOverride_Sprint SprintPose;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotionOverride_ExtendedPose ExtendedPose;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotionOverride_AimOffset AimOffset;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotionOverride_Config Config;
    
    bool operator==(const FLocomotionOverride& Other) const
    {
        return GlobalPose == Other.GlobalPose &&
               IdlePose == Other.IdlePose &&
               JumpPose == Other.JumpPose &&
               MovePose == Other.MovePose &&
               StartsPose == Other.StartsPose &&
               TurnPose == Other.TurnPose &&
               SprintPose == Other.SprintPose &&
               ExtendedPose == Other.ExtendedPose &&
               AimOffset == Other.AimOffset &&
               Config == Other.Config;
    }
};

// ===============
//
// ===============


USTRUCT(BlueprintType)
struct FLocomotionMovement
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TObjectPtr<UAnimSequence> Idle;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TArray<TObjectPtr<UAnimSequence>> IdleBreaks;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotion_AimOffset AimOffset;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotion_TurnInPlace TurnInPlace;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotion_CardinalDirections Start_Cardinals;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotion_CardinalDirections Stop_Cardinals;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotion_CardinalDirections Pivot_Cardinals;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotion_CardinalDirections Cycle_Cardinals;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotion_Starts Start_ForwardFacing;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotion_Jump Jump;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotion_Config Config;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TMap<FName, FLocomotionOverride> Override;
};

// AnimSet
USTRUCT(BlueprintType)
struct FAnimSetting : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FLocomotion_General General;
    
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    TMap<EPlayerMovementMode, FLocomotionMovement> Movement;
};