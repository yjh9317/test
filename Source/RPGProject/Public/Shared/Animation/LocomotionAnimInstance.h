// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AimOffsetBlendSpace.h"
#include "Animation/AnimExecutionContext.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimNodeReference.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/Data/LocomotionEnum.h"
#include "Player/Data/LocomotionStructs.h"
// #include "Animation/AnimExecutionContext.h"
// #include "Animation/AnimNodeReference.h"
#include "LocomotionAnimInstance.generated.h"

class UCharacterMovementComponent;
class ULocomotionComponent;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API ULocomotionAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	
protected:
	void BindLocomotionEvents();
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual void NativeThreadSafeUpdateAnimation(float DeltaSeconds) override;

#pragma region LocomotionComponentEvent

	UFUNCTION(BlueprintCallable,Category="Locomotion Component Event")
	void OnSprintChanged(bool IsSprint);

	UFUNCTION(BlueprintCallable,Category="Locomotion Component Event")
	void OnSetOverrideName(const FName& Name);
	
	UFUNCTION(BlueprintCallable,Category="Locomotion Component Event")
	void OnSetMovementType(EPlayerMovementMode MovementMode);

	UFUNCTION(BlueprintCallable,Category="Locomotion Component Event")
	void OnSetAnimset(const FName& InAnimsetName);

	UFUNCTION(BlueprintCallable,Category="Locomotion Component Event")
	void OnSetRotationMode(ERotationMode RotationMode);

	UFUNCTION(BlueprintCallable,Category="Locomotion Component Event")
	void OnAccelerationChanged(bool InHasAcceleration,float InLastVelocityDirection);

	UFUNCTION(BlueprintCallable,Category="Locomotion Component Event")
	void OnSetMount(AActor* MountActor);

	UFUNCTION(BlueprintCallable,Category="Locomotion Component Event")
	void OnRootYawOffsetStatus(bool Disable);

	UFUNCTION(BlueprintCallable,Category="Locomotion Component Event")
	void OnSetGlobalExtendedPose(EExtendedPose Pose);

	UFUNCTION(BlueprintCallable,Category="Locomotion Component Event")
	void OnMovementModeChanged(ACharacter* InCharacter, EMovementMode PrevMovementMode, uint8 PreviousCustomMode);

#pragma endregion
	
	UFUNCTION()
	void AfterDelay_CheckWallCollision();
	void CheckIsRunningIntoWall();

	UFUNCTION(BlueprintCallable,Category="Turn In Place")
	void SetRootYawOffset(float InRootYawOffset);
	UFUNCTION(BlueprintCallable,Category="Turn In Place")
	void ProcessTurnYawCurve();
	UFUNCTION(BlueprintCallable,Category="Turn In Place")
	void UpdateRootYawOffset(float InDeltaTime);
	UFUNCTION(BlueprintCallable,Category="Turn In Place")
	void ProcessTurnYawForwardFacing();
	UFUNCTION(BlueprintCallable,Category="State Node Functions", meta=(BlueprintThreadSafe))
	void UpdateIdleState(const FAnimUpdateContext& Context,const FAnimNodeReference& Node); 
	UFUNCTION(BlueprintCallable,Category="State Node Functions", meta=(BlueprintThreadSafe))
	void SetUpStartState(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="State Node Functions", meta=(BlueprintThreadSafe))
	void UpdateStartState(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="State Node Functions",  meta=(BlueprintThreadSafe))
	void UpdateStopState(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="State Node Functions", meta=(BlueprintThreadSafe))
	void SetUpPivotState(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="State Node Functions", meta=(BlueprintThreadSafe))
	void UpdatePivotState(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);

	UFUNCTION(BlueprintPure,Category="HelperFunctions", meta=(BlueprintThreadSafe))
	ECardinalDirection SelectCardinalDirectionFromAngle(float Angle,float DeadZone,ECardinalDirection CurrentDirection, bool UseCurrentDirection);
	UFUNCTION(BlueprintPure,Category="HelperFunctions", meta=(BlueprintThreadSafe))
	ECardinalDirection GetOppositeCardinalDirection(ECardinalDirection CurrentDirection);
	UFUNCTION(BlueprintPure,Category="HelperFunctions", meta=(BlueprintThreadSafe))
	bool IsMovingPerpendicularToInitialPivot();

	UFUNCTION(BlueprintPure,Category="Idle Breaks - Layer", meta=(BlueprintThreadSafe))
	bool CanPlayIdleBreak();
	UFUNCTION(BlueprintCallable,Category="Idle Breaks - Layer", meta=(BlueprintThreadSafe))
	void ResetIdleBreakTransitionLogic();
	UFUNCTION(BlueprintCallable,Category="Idle Breaks - Layer", meta=(BlueprintThreadSafe))
	void ProcessIdleBreakTransitionLogic(float DeltaTime);
	UFUNCTION(BlueprintCallable,Category="Idle Breaks - Layer", meta=(BlueprintThreadSafe))
	void ChooseIdleBreakDelayTime();

	UFUNCTION(BlueprintCallable,Category="State Node Functions - Layer", meta=(BlueprintThreadSafe))
	void SetUpIdleState_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="State Node Functions - Layer", meta=(BlueprintThreadSafe))
	void UpdateIdleState_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="State Node Functions - Layer", meta=(BlueprintThreadSafe))
	void SetupIdleTransition_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable,Category="Anim Node Functions - Layer", meta=(BlueprintThreadSafe))
	void UpdateIdleAnim_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="Anim Node Functions - Layer", meta=(BlueprintThreadSafe))
	void SetUpIdleBreakAnim_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="Anim Node Functions - Layer", meta=(BlueprintThreadSafe))
	void SetUpStartAnim_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="Anim Node Functions - Layer", meta=(BlueprintThreadSafe))
	void UpdateStartAnim_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="Anim Node Functions - Layer", meta=(BlueprintThreadSafe))
	void UpdateCycleAnim_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="Anim Node Functions - Layer", meta=(BlueprintThreadSafe))
	void SetUpStopAnim_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="Anim Node Functions - Layer", meta=(BlueprintThreadSafe))
	void UpdateStopAnim_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="Anim Node Functions - Layer", meta=(BlueprintThreadSafe))
	void SetUpPivotAnim_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="Anim Node Functions - Layer", meta=(BlueprintThreadSafe))
	void UpdatePivotAnim_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="Anim Node Functions - Layer", meta=(BlueprintThreadSafe))
	void SetUpFallLandAnim_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="Anim Node Functions - Layer", meta=(BlueprintThreadSafe))
	void UpdateFallLandAnim_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="Anim Node Functions - Layer")
	void UpdateJumpFallData_Layer(float InDeltaTime);
	UFUNCTION(BlueprintCallable,Category="Anim Node Functions - Layer", meta=(BlueprintThreadSafe))
	void LandRecoveryStartLayer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);

	UFUNCTION(BlueprintPure,Category="Distance Matching - Layer", meta=(BlueprintThreadSafe))
	float GetPredictedStopDistance();
	UFUNCTION(BlueprintPure,Category="Distance Matching - Layer", meta=(BlueprintThreadSafe))
	bool ShouldDistanceMatchStop();
	
	UFUNCTION(BlueprintCallable,Category="Turn In Place - Layer", meta=(BlueprintThreadSafe))
	void SetupTurnInPlaceAnim_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="Turn In Place - Layer")
	UAnimSequence* SelectTurnInPlaceAnimation(float Direction);
	UFUNCTION(BlueprintCallable,Category="Turn In Place - Layer", meta=(BlueprintThreadSafe))
	void UpdateTurnInPlaceAnim_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="Turn In Place - Layer", meta=(BlueprintThreadSafe))
	void SetUpTurnInPlaceRotationState_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="Turn In Place - Layer", meta=(BlueprintThreadSafe))
	void SetUpTurnInPlaceRecoveryState_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="Turn In Place - Layer", meta=(BlueprintThreadSafe))
	void UpdateTurnInPlaceRecoveryAnim_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);

	UFUNCTION(BlueprintCallable ,Category="State Node Functions - Override", meta=(BlueprintThreadSafe))
	void UpdateOverrideGlobalPose(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	
	UFUNCTION(BlueprintCallable, Category="State Node Functions - Override", meta=(BlueprintThreadSafe))
	void UpdateOverrideIdlePose(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="State Node Functions - Override", meta=(BlueprintThreadSafe))
	void UpdateOverrideJumpPose(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="State Node Functions - Override", meta=(BlueprintThreadSafe))
	void UpdateOverrideMoveStartPose(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="State Node Functions - Override", meta=(BlueprintThreadSafe))
	void UpdateOverrideMoveStopPose(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="State Node Functions - Override", meta=(BlueprintThreadSafe))
	void UpdateOverrideMoveCyclePose(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="State Node Functions - Override", meta=(BlueprintThreadSafe))
	void UpdateOverrideMovePivotPose(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	UFUNCTION(BlueprintCallable,Category="State Node Functions - Override", meta=(BlueprintThreadSafe))
	void UpdateOverrideTurnPose(const FAnimUpdateContext& Context,const FAnimNodeReference& Node);
	
	UFUNCTION(BlueprintPure,Category="Animset", meta=(BlueprintThreadSafe))
	FLocomotionOverride_Sequence GetStartOverride();
	UFUNCTION(BlueprintPure,Category="Animset", meta=(BlueprintThreadSafe))
	FLocomotionMovement GetMovementStruct();
	UFUNCTION(BlueprintPure,Category="Animset", meta=(BlueprintThreadSafe))
	FLocomotionOverride_Sequence GetOverride_Global();
	UFUNCTION(BlueprintCallable,Category="Animset")
	void SetAnimValid();
	UFUNCTION(BlueprintPure,Category="Animset", meta=(BlueprintThreadSafe))
	bool IsValidStartAnim();
	UFUNCTION(BlueprintPure,Category="Animset", meta=(BlueprintThreadSafe))
	bool IsValidStopAnim();
	UFUNCTION(BlueprintPure,Category="Animset", meta=(BlueprintThreadSafe))
	bool IsValidPivotAnim();
	UFUNCTION(BlueprintCallable,Category="Animset")
	void SetDefaultStarts();
	UFUNCTION(BlueprintPure,Category="Animset", meta=(BlueprintThreadSafe))
	UAnimSequenceBase* GetStartAnim();

	UFUNCTION(BlueprintCallable, Category="Blueprint Thread Safe Update Functions")
	void UpdateLocationData(float DeltaTime);
	UFUNCTION(BlueprintCallable, Category="Blueprint Thread Safe Update Functions")
	void UpdateRotationData();
	UFUNCTION(BlueprintCallable, Category="Blueprint Thread Safe Update Functions")
	void UpdateVelocityData();
	UFUNCTION(BlueprintCallable, Category="Blueprint Thread Safe Update Functions")
	void UpdateAccelerationData();
	UFUNCTION(BlueprintCallable, Category="Blueprint Thread Safe Update Functions")
	void UpdateCharacterStateData(float DeltaTime);
	UFUNCTION(BlueprintCallable, Category="Blueprint Thread Safe Update Functions")
	void UpdateJumpFallData();
	UFUNCTION(BlueprintCallable, Category="Blueprint Thread Safe Update Functions")
	void UpdateWallDetectionHeuristic();
	UFUNCTION(BlueprintCallable, Category="Blueprint Thread Safe Update Functions")
	void UpdateAimingData(float DeltaTime);
	UFUNCTION(BlueprintCallable, Category="Blueprint Thread Safe Update Functions")
	void UpdateUpperBodyAlpha(float DeltaTime);
	UFUNCTION(BlueprintCallable, Category="Blueprint Thread Safe Update Functions")
	void UpdateSkelControlData(float DeltaTime);
	UFUNCTION(BlueprintCallable, Category="Blueprint Thread Safe Update Functions")
	void UpdateBlendWeightData(float DeltaTime);
	
	UFUNCTION(BlueprintPure, Category="Blueprint Thread Safe Get Functions", meta=(BlueprintThreadSafe))
	UCharacterMovementComponent* GetMovementComponent();
	UFUNCTION(BlueprintPure, Category="Blueprint Thread Safe Get Functions", meta=(BlueprintThreadSafe))
	float GetAimOffsetYaw();
	UFUNCTION(BlueprintPure, Category="Blueprint Thread Safe Get Functions", meta=(BlueprintThreadSafe))
	float GetAimOffsetPitch();
	
	UFUNCTION(BlueprintCallable,Category="Not Thread Safe")
	void Initialization();
	UFUNCTION(BlueprintCallable,Category="Not Thread Safe")
	void SetAnimationDatas();
	UFUNCTION(BlueprintCallable,Category="Not Thread Safe")
	void SetGroundDistance();
	UFUNCTION(BlueprintCallable,Category="Not Thread Safe")
	void InitializeEvents();


public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="World Rotation")
	FRotator WorldRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="World Rotation")
	float YawDeltaSinceLastUpdate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="World Rotation")
	float AdditiveLeanAngle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="World Rotation")
	float YawDeltaSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Location Data")
	FVector WorldLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Location Data")
	float DisplacementSinceLastUpdate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Location Data")
	float DisplacementSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="World Velocity")
	FVector WorldVelocity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="World Velocity")
	FVector LocalVelocity2D;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="World Velocity")
	float LocalVelocityDirectionAngle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="World Velocity")
	float LocalVelocityDirectionAngleWithOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="World Velocity")
	ECardinalDirection LocalVelocityDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="World Velocity")
	ECardinalDirection LocalVelocityDirectionNoOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="World Velocity")
	bool HasVelocity;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="World Velocity")
	ECardinalDirection LastDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="World Velocity")
	float LastVelocityDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Acceleration Data")
	FVector LocalAcceleration2D;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Acceleration Data")
	bool bHasAcceleration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Acceleration Data")
	FVector PivotDirection2D;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Character State Data")
	bool IsOnGround;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Character State Data")
	bool IsCrouching;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Character State Data")
	bool CrouchStateChange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Character State Data")
	float TimeSinceFiredWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Character State Data")
	bool IsJumping;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Character State Data")
	bool IsFalling;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Character State Data")
	float TimeToJumpApex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Character State Data")
	bool IsRunningIntoWall;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Character State Data")
	bool AnimsetChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Character State Data")
	bool bIsSprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Character State Data")
	bool ReachedEndOfTurn;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Character State Data")
	bool PlayStartAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Character State Data")
	bool RotationModeChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Character State Data")
	bool InWall;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Character State Data")
	bool MovementModeChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Character State Data")
	bool ExtendedPoseChanged;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Gameplay Tag Bindings")
	bool GameplayTag_IsADS;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Gameplay Tag Bindings")
	bool GameplayTag_IsFiring;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Gameplay Tag Bindings")
	bool GameplayTag_IsReloading;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Gameplay Tag Bindings")
	bool GameplayTag_IsDashing;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Gameplay Tag Bindings")
	bool GameplayTag_IsMelee;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="LocomotionSM Data")
	ECardinalDirection StartDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="LocomotionSM Data")
	ECardinalDirection PivotInitialDirection;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="LocomotionSM Data")
	float LastPivotTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="LocomotionSM Data")
	ECardinalDirection CardinalDirectionFromAcceleration;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Linked Layer Data")
	TObjectPtr<UAnimInstance> LastLinkedLayer;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Turn In Place")
	float RootYawOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Turn In Place")
	FFloatSpringState RootYawOffsetSpringState;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Turn In Place")
	float TurnYawCurveValue;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Turn In Place")
	ERootYawOffsetMode RootYawOffsetMode;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Turn In Place")
	FVector2D RootYawOffsetAngleClamp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Turn In Place")
	FVector2D RootYawOffsetAngleClampCrouched;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Turn In Place")
	FVector2D RotateRootYawOffsetAngleClamp;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Turn In Place")
	float TurnInPlaceAnimTime;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Turn In Place")
	float TurnInPlaceRotationDirection;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Turn In Place")
	float TurnInPlaceRecoveryDirection;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Turn In Place")
	float MaxTurnYawValue;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Turn In Place")
	float CachedTurnYawCurveValue;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Turn In Place")
	bool DisableRootYawOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Idle Breaks")
	float TimeUntilNextIdleBreak;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Idle Breaks")
	int32 CurrentIdleBreakIndex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Idle Breaks")
	float IdleBreakDelayTime;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Idle Breaks")
	bool DisableIdleBreaks;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Pivots")
	FVector PivotStartingAcceleration;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Pivots")
	float TimeAtPivotStop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Jump")
	float GroundDistance;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Jump")
	float LandRecoveryAlpha;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Jump")
	float TimeFalling;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Jump")
	float JumpRecoveryAdditiveAlpha;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stride Warping")
	float StrideWarpingStartAlpha;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stride Warping")
	float StrideWarpingPivotAlpha;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Stride Warping")
	float StrideWarpingCycleAlpha;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Skeletal Control Data")
	float HandFKWeight = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Skeletal Control Data")
	float HandIK_Right_Alpha = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Skeletal Control Data")
	float HandIK_Left_Alpha = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Skeletal Control Data")
	float IKAlpha;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Skeletal Control Data")
	bool DisableLHandIK;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Skeletal Control Data")
	FVector EffectorLocationHandL;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Skeletal Control Data")
	FVector JointTargetLocationHandL;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Skeletal Control Data")
	EHandIKBones JointTargetBoneHandL;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Skeletal Control Data")
	bool DisableRHandIK;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Skeletal Control Data")
	FVector EffectorLocationHandR;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Skeletal Control Data")
	FVector JointTargetLocationHandR;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Skeletal Control Data")
	EHandIKBones JointTargetBoneHandR;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aiming Data")
	float AimYaw;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aiming Data")
	float AimPitch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aiming Data")
	FVector2D YawRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aiming Data")
	FVector2D PitchRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aiming Data")
	TArray<UAimOffsetBlendSpace*> AimOffsetList;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aiming Data")
	int32 AimOffsetIndex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aiming Data")
	FVector2D AimingOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aiming Data")
	float AimOffset_InterpSpeed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aiming Data")
	float AimOffset_BlendWeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Default")
	bool bIsFirstUpdate = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Default")
	bool DisableControlRig;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Default")
	bool TurnInPlaceB;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Reference")
	TObjectPtr<ULocomotionComponent> LocomotionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Reference")
	TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovement;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Reference")
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aim Sequence")
	TObjectPtr<UAimOffsetBlendSpace> AimOffset;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aim Sequence")
	TObjectPtr<UAnimSequence> Jump_Start;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aim Sequence")
	TObjectPtr<UAnimSequence> Jump_Start_Loop;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aim Sequence")
	TObjectPtr<UAnimSequence> Jump_Apex;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aim Sequence")
	TObjectPtr<UAnimSequence> Jump_Fall_Loop;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aim Sequence")
	TObjectPtr<UAnimSequence> Jump_Fall_Land;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aim Sequence")
	TObjectPtr<UAnimSequence> Jump_Recovery_Additive;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aim Sequence")
	TObjectPtr<UAnimSequence> IdleAnim;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aim Sequence")
	TArray<TObjectPtr<UAnimSequence>> Idle_Breaks; 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aim Sequence")
	TObjectPtr<UAnimSequence> CrouchIdleEntry;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aim Sequence")
	TObjectPtr<UAnimSequence> CrouchIdleExit;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aim Sequence")
	FLocomotion_TurnInPlace TurnInPlace;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aim Sequence")
	FLocomotion_CardinalDirections StartCardinals;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aim Sequence")
	FLocomotion_CardinalDirections StopCardinals;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aim Sequence")
	FLocomotion_CardinalDirections PivotCardinals;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aim Sequence")
	FLocomotion_CardinalDirections CycleCardinals;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aim Sequence")
	FLocomotion_Starts LookingDirection;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aim Sequence")
	TObjectPtr<UBlendSpace1D> Leans;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aim Sequence")
	TObjectPtr<UAnimSequence> SprintStart;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aim Sequence")
	TObjectPtr<UAnimSequence> SprintStop;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Aim Sequence")
	TObjectPtr<UAnimSequence> SprintCycle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Override")
	FLocomotionOverride_Sequence Override_GlobalPose = {nullptr,0.f,0.f,10.f,0.25f,TEXT("UpperBodyMask")};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Override")
	FLocomotionOverride_Sequence Override_IdlePose = {nullptr,0.f,0.f,10.f,0.25f,TEXT("UpperBodyMask")};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Override")
	FLocomotionOverride_Sequence Override_JumpPose = {nullptr,0.f,0.f,10.f,0.25f,TEXT("UpperBodyMask")};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Override")
	FLocomotionOverride_Sequence Override_MoveStartPose = {nullptr,0.f,0.f,10.f,0.25f,TEXT("UpperBodyMask")};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Override")
	FLocomotionOverride_Sequence Override_MoveStartPose_Cached ={nullptr,0.f,0.f,10.f,0.25f,TEXT("UpperBodyMask")};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Override")
	FLocomotionOverride_Sequence Override_MoveStopPose = {nullptr,0.f,0.f,10.f,0.25f,TEXT("UpperBodyMask")};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Override")
	FLocomotionOverride_Sequence Override_MoveCyclePose = {nullptr,0.f,0.f,10.f,0.25f,TEXT("UpperBodyMask")};
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Override")
	FLocomotionOverride_Sequence Override_MovePivotPose = {nullptr,0.f,0.f,10.f,0.25f,TEXT("UpperBodyMask")};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Override")
	FLocomotionOverride_Sequence Override_TurnPose= {nullptr,0.f,0.f,10.f,0.25f,TEXT("UpperBodyMask")};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Override")
	FLocomotionOverride_Starts Override_FacingStartsPose= {nullptr,0.f,0.f,10.f,0.25f,TEXT("UpperBodyMask")};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Override")
	FLocomotionOverride_Sequence Override_SprintStartPose = {nullptr,0.f,0.f,10.f,0.25f,TEXT("UpperBodyMask")};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Override")
	FLocomotionOverride_Sequence Override_SprintCyclePose = {nullptr,0.f,0.f,10.f,0.25f,TEXT("UpperBodyMask")};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Override")
	FLocomotionOverride_Sequence Override_SprintStopPose= {nullptr,0.f,0.f,10.f,0.25f,TEXT("UpperBodyMask")};

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Mount")
	FName AimOffset_BlendName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Mount")
	TObjectPtr<ACharacter> MountCharacter;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Mount")
	bool MountIsControlled;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Valid")
	bool IsValid_FwdStart;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Valid")
	bool IsValid_BwdStart;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Valid")
	bool IsValid_LeftStart;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Valid")
	bool IsValid_RightStart;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Valid")
	bool IsValid_Animset = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Valid")
	bool IsValid_AimOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Valid")
	bool IsValid_StopFwd;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Valid")
	bool IsValid_StopBwd;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Valid")
	bool IsValid_StopLeft;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Valid")
	bool IsValid_StopRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Valid")
	bool IsValid_PivotFwd;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Valid")
	bool IsValid_PivotBwd;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Valid")
	bool IsValid_PivotLeft;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Valid")
	bool IsValid_PivotRight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Valid")
	bool IsValid_JumpApex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Valid")
	bool IsValid_JumpFallLand;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Anim Valid")
	bool IsValid_JumpStartLoop;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Locomotion System")
	FAnimSetting LS_AnimSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Locomotion System")
	EPlayerMovementMode LS_MovementType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Locomotion System")
	ERotationMode LS_RotationMode;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Locomotion System")
	FName LS_AnimsetName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Locomotion System")
	FName LS_OverrideName = TEXT("Default");

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Extended")
	EExtendedPose ExtendedPose;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Extended")
	FName Extended_BlendName;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Extended")
	float Extended_OverrideWeight;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Extended")
	bool Extended_DisableMoveAnim;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Extended")
	float Extended_InterpSpeed;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Extended")
	float Extended_BlendTime;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Extended")
	EExtendedPose GlobalExtendedPose;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Extended")
	EExtendedPose Cached_ExtendedPose;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Setting")
	float CardinalDirectionDeadZone = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Setting")
	float StrideWarpingBlendInDurationScaled = 0.2f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Setting")
	float StrideWarpingBlendInStartOffset = 0.15f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Setting")
	FName LocomotionDistanceCurveName = TEXT("Distance");
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Setting")
	FName JumpDistanceCurveName= TEXT("GroundDistance");
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Setting")
	bool DisableHandIK = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Setting")
	bool DisableLegIK;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Setting")
	FVector2D PlayRateClampStarts;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Setting")
	FVector2D PlayRateClampPivots;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Setting")
	FVector2D PlayRateClampCycle;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Setting")
	FVector2D PlayRateClampStarts_ForwardFacing;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Setting")
	FVector2D PlayRateClampStarts_Sprint;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category="Setting")
	FVector2D PlayRateClampCycle_Sprint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Montage")
	float UpperBodyAlpha;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Montage")
	float UpperBodyDynamicAdditiveWeight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Default")
	bool IsMounted;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Default")
	bool DisableLegIKWhileMoving;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Default")
	bool SprintChanged;

	FTimerHandle ExtendedPoseChangedTimer;
	FTimerHandle MovementModeChangedTimer;
	FTimerHandle CrouchStateChangeTimer;
	FTimerHandle AnimsetChangedTimer;
	FTimerHandle RotationModeChangedTimer;
	FTimerHandle MountIsControlledTimer;
};
