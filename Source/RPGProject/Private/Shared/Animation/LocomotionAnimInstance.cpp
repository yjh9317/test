// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/Animation/LocomotionAnimInstance.h"
#include "AnimationStateMachineLibrary.h"
#include "SequenceEvaluatorLibrary.h"
#include "SequencePlayerLibrary.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Header/DebugHeader.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/Components/LocomotionComponent.h"
#include "AnimDistanceMatchingLibrary.h"
#include "AnimCharacterMovementLibrary.h"
#include "KismetAnimationLibrary.h"

#pragma region Init


void ULocomotionAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	Initialization();
}

void ULocomotionAnimInstance::BindLocomotionEvents()
{
	LocomotionComponent->OnSetAnimsDelegate.AddUObject(this,&ULocomotionAnimInstance::OnSetAnimset);
	LocomotionComponent->OnSetMovementTypeDelegate.AddUObject(this,&ULocomotionAnimInstance::OnSetMovementType);
	LocomotionComponent->OnSprintChangedDelegate.AddUObject(this,&ULocomotionAnimInstance::OnSprintChanged);
	LocomotionComponent->OnSetRotationModeDelegate.AddUObject(this,&ULocomotionAnimInstance::OnSetRotationMode);
	LocomotionComponent->OnAccelerationChangedDelegate.AddUObject(this,&ULocomotionAnimInstance::OnAccelerationChanged);
	LocomotionComponent->OnSetOverrideNameDelegate.AddUObject(this,&ULocomotionAnimInstance::OnSetOverrideName);
	LocomotionComponent->OnRootYawOffsetStatusDelegate.AddUObject(this,&ULocomotionAnimInstance::OnRootYawOffsetStatus);
	LocomotionComponent->OnSetMountDelegate.AddUObject(this,&ULocomotionAnimInstance::OnSetMount);
	LocomotionComponent->OnSetGlobalExtendedPoseDelegate.AddUObject(this,&ULocomotionAnimInstance::OnSetGlobalExtendedPose);
}

void ULocomotionAnimInstance::InitializeEvents()
{
	OnAccelerationChanged(LocomotionComponent->HasAcceleration,LocalVelocityDirectionAngleWithOffset);
	OnSetMovementType(LocomotionComponent->MovementType);
	OnSetRotationMode(LocomotionComponent->RotationMode);
	OnSetAnimset(LocomotionComponent->AnimsetName);
	OnSprintChanged(LocomotionComponent->bIsSprinting);
	OnRootYawOffsetStatus(LocomotionComponent->DisableRootYawOffset);
	OnSetMount(LocomotionComponent->MountActor);
	OnSetOverrideName(LocomotionComponent->OverrideName);
}



#pragma endregion 

#pragma region Tick

void ULocomotionAnimInstance::NativeThreadSafeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeThreadSafeUpdateAnimation(DeltaSeconds);
	UpdateSkelControlData(DeltaSeconds);
	UpdateLocationData(DeltaSeconds);
	UpdateRotationData();
	UpdateVelocityData();
	UpdateAccelerationData();
	UpdateWallDetectionHeuristic();
	UpdateCharacterStateData(DeltaSeconds);
	UpdateUpperBodyAlpha(DeltaSeconds);
	UpdateBlendWeightData(DeltaSeconds);
	UpdateRootYawOffset(DeltaSeconds);
	UpdateAimingData(DeltaSeconds);
	UpdateJumpFallData_Layer(DeltaSeconds);
	UpdateJumpFallData();
	DisableControlRig = GetCurveValue("DisableLegIK") > 0 || DisableLegIK || (DisableLegIKWhileMoving && bHasAcceleration);
	bIsFirstUpdate = false;
	IKAlpha = UKismetMathLibrary::FInterpTo(
		IKAlpha,
		DisableControlRig ? 1.f : 0.f,
		DeltaSeconds,
		10.f
		);
	
}

void ULocomotionAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	SetGroundDistance();
	CheckIsRunningIntoWall();
}

void ULocomotionAnimInstance::SetGroundDistance()
{
	if(nullptr == OwnerCharacter || !IsFalling || GetWorld())
	{
		return;
	}

	const float CapsuleHalfHeight = OwnerCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() + 20.f;
	const FVector CharacterLocation = OwnerCharacter->GetActorLocation();
	const FVector CharacterUpVector = OwnerCharacter->GetActorUpVector();
	
	const FVector StartLocation = CharacterLocation + -CapsuleHalfHeight * CharacterUpVector;
	const FVector EndLocation = StartLocation + CharacterUpVector * -1000.f;
	
	FCollisionQueryParams QueryParams{};
	QueryParams.bTraceComplex = false;
	QueryParams.AddIgnoredActor(OwnerCharacter);

	FHitResult HitResult{};
	GetWorld()->LineTraceSingleByChannel(HitResult,StartLocation,EndLocation,ECC_Visibility,QueryParams);

	if(HitResult.bBlockingHit)
	{
		GroundDistance = HitResult.Distance;
	}
}

FLocomotionOverride_Sequence ULocomotionAnimInstance::GetStartOverride()
{
	if(OwnerCharacterMovement->bOrientRotationToMovement)
	{
		switch(LastDirection)
		{
		case ECardinalDirection::Forward:
			return Override_FacingStartsPose.Forward;
		case ECardinalDirection::Backward:
			if(LastVelocityDirection > 0.f)
			{
				return Override_FacingStartsPose.Forward_Left180;
			}
			else
			{
				return Override_FacingStartsPose.Forward_Right180;
			}
		case ECardinalDirection::Left:
			return Override_FacingStartsPose.Forward_Left90;
		case ECardinalDirection::Right:
			return Override_FacingStartsPose.Forward_Right90;
		}
	}
	return Override_MoveStartPose;
}

void ULocomotionAnimInstance::AfterDelay_CheckWallCollision()
{
	if (IsRunningIntoWall)
	{
		InWall = true;
	}
}

void ULocomotionAnimInstance::CheckIsRunningIntoWall()
{
	if(IsRunningIntoWall)
	{
		FLatentActionInfo LatentInfo;
		LatentInfo.CallbackTarget = this;
		LatentInfo.ExecutionFunction = FName("AfterDelay_CheckWallCollision");
		LatentInfo.Linkage = 0;
		LatentInfo.UUID = __LINE__;
		
		UKismetSystemLibrary::DelayUntilNextTick(GetWorld(),LatentInfo);
	}
	else
	{
		InWall = false;
	}
}

// TurnInPlace #3
// We clamp the offset because at large offsets the character has to aim too far backwards, which over twists the spine. The turn in place animations will usually keep up with the offset, but this clamp will cause the feet to slide if the user rotates the camera too quickly.
// If desired, this clamp could be replaced by having aim animations that can go up to 180 degrees or by triggering turn in place animations more aggressively.
void ULocomotionAnimInstance::SetRootYawOffset(float InRootYawOffset)
{
	float AngleDegrees = UKismetMathLibrary::NormalizeAxis(InRootYawOffset);

	if(DisableRootYawOffset)
	{
		if(RotateRootYawOffsetAngleClamp.X == RotateRootYawOffsetAngleClamp.Y)
		{
			RootYawOffset = UKismetMathLibrary::ClampAngle(AngleDegrees,RotateRootYawOffsetAngleClamp.X,RotateRootYawOffsetAngleClamp.Y);
		}
		else
		{
			RootYawOffset = AngleDegrees;
		}
	}
	else
	{
		RootYawOffset = 0.f;
	}
}

//The "TurnYawWeight" curve is set to 1 in TurnInPlace animations, so its current value from GetCurveValue will be the current weight of the TurnInPlace animation. 
// We can use this to "unweight" the TurnInPlace animation to get the full RemainingTurnYaw curve value. 
void ULocomotionAnimInstance::ProcessTurnYawCurve()
{
	float PreviousTurnYawCurveValue = TurnYawCurveValue;

	const float CurrentCurveValue = UAnimInstance::GetCurveValue("TurnYawWeight");
	if(UKismetMathLibrary::NearlyEqual_FloatFloat(CurrentCurveValue,0.f,0.0001))
	{
		TurnYawCurveValue = 0.f;
		PreviousTurnYawCurveValue = 0.f;
	}
	else
	{
		TurnYawCurveValue = UAnimInstance::GetCurveValue("RemainingTurnYaw") / CurrentCurveValue;

		// Avoid applying the curve delta when the curve first becomes relevant. E.g. When a turn animation starts, the previous curve value will be 0 and the current value will be 90, but no actual rotation has happened yet.
		if(PreviousTurnYawCurveValue != 0)
		{
			// Reduce the root yaw offset by the amount of rotation from the turn animation.
			SetRootYawOffset(RootYawOffset - (TurnYawCurveValue - PreviousTurnYawCurveValue));
		}
	}
}

//TurnInPlace #2
// This function handles updating the yaw offset depending on the current state of the Pawn owner.
void ULocomotionAnimInstance::UpdateRootYawOffset(float InDeltaTime)
{
	// When the feet aren't moving (e.g. during Idle), offset the root in the opposite direction to the Pawn owner's rotation to keep the mesh from rotating with the Pawn.
	if(RootYawOffsetMode == ERootYawOffsetMode::Accumulate)
	{
		SetRootYawOffset(RootYawOffset - YawDeltaSinceLastUpdate);
	}
	// When in motion, smoothly blend out the offset.
	if(RootYawOffsetMode == ERootYawOffsetMode::BlendOut || GameplayTag_IsDashing)
	{
		float InRootYawOffset = UKismetMathLibrary::FloatSpringInterp(RootYawOffset,0.f,RootYawOffsetSpringState,80.f,1.f,InDeltaTime,1.f,0.5f);
		SetRootYawOffset(InRootYawOffset);
	}

	// 	Reset to blending out the yaw offset. Each update, a state needs to request to accumulate or hold the offset. Otherwise, the offset will blend out.
	// This is primarily because the majority of states want the offset to blend out, so this saves on having to tag each state.
	RootYawOffsetMode = ERootYawOffsetMode::BlendOut;
}

// TurnInPlace #5
// When the yaw offset gets too big, we trigger TurnInPlace animations to rotate the character back. E.g. if the camera is rotated 90 degrees to the right, it will be facing the character's right shoulder. If we play an animation that rotates the character 90 degrees to the left, the character will once again be facing away from the camera.
// We use the "TurnYawAnimModifier" animation modifier to generate the necessary curves on each TurnInPlace animation.
// See ABP_ItemAnimLayersBase for examples of triggering TurnInPlace animations.
void ULocomotionAnimInstance::ProcessTurnYawForwardFacing()
{
	float PreviousTurnYawCurveValue = TurnYawCurveValue;

	const float CurrentCurveValue = UAnimInstance::GetCurveValue("TurnYawWeight");
	if(UKismetMathLibrary::NearlyEqual_FloatFloat(CurrentCurveValue,0.f,0.0001))
	{
		TurnYawCurveValue = 0.f;
		PreviousTurnYawCurveValue = 0.f;

		// When the animation blends in, we could have near-zero weights. When this value is goes up, and then down to 0, we know
		if(!MaxTurnYawValue == 0)
		{
			ReachedEndOfTurn = true;
		}
	}
	else
	{
		TurnYawCurveValue = UAnimInstance::GetCurveValue("RemainingTurnYaw") / CurrentCurveValue;

		if(UKismetMathLibrary::Abs(TurnYawCurveValue) > UKismetMathLibrary::Abs(MaxTurnYawValue))
		{
			MaxTurnYawValue = TurnYawCurveValue;
		}

		// Avoid applying the curve delta when the curve first becomes relevant.
		// E.g. When a turn animation starts, the previous curve value will be 0 and the current value will be 90, but no actual rotation has happened yet.
		if(PreviousTurnYawCurveValue != 0)
		{
			// TurnScale: 캐릭터가 실제로 돌아야 할 각도와 애니메이션의 남은 회전 각도의 비율
			const float TurnScale = UKismetMathLibrary::SafeDivide(LocalVelocityDirectionAngleWithOffset, TurnYawCurveValue);

			// TurnYawDelta: 이번 프레임에서 애니메이션 커브가 만들어낸 회전량
			const float TurnYawDelta = TurnYawCurveValue - PreviousTurnYawCurveValue;

			// 클램핑 전의 원본 보정값을 계산합니다.
			const float UnclampedRootYawDelta = TurnScale - TurnYawDelta;
    
			// 이번 프레임의 Yaw 보정량은 캐릭터가 돌아야 할 전체 각도를 초과할 수 없습니다.
			const float MaxYawCorrection = UKismetMathLibrary::Abs(LocalVelocityDirectionAngleWithOffset);
			const float RootYawDelta = UKismetMathLibrary::Clamp(UnclampedRootYawDelta, -MaxYawCorrection, MaxYawCorrection);
    
			// 턴 애니메이션이 수행한 회전량만큼 루트 Yaw 오프셋을 줄여줍니다.
			SetRootYawOffset(RootYawOffset - RootYawDelta);
		}
	}
}

void ULocomotionAnimInstance::UpdateIdleState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if(!GetMovementComponent())
	{
		return;
	}
	FAnimationStateResultReference ResultReference{};
	bool TempResult = false;
	UAnimationStateMachineLibrary::ConvertToAnimationStateResultPure(Node,ResultReference,TempResult);
	if(UAnimationStateMachineLibrary::IsStateBlendingOut(Context,ResultReference))
	{
		if(!GetMovementComponent()->bOrientRotationToMovement)
		{
			TurnYawCurveValue = 0.f;
		}
	}
	else
	{
		RootYawOffsetMode = ERootYawOffsetMode::Accumulate;
		ProcessTurnYawCurve();
	}
}

void ULocomotionAnimInstance::SetUpStartState(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	StartDirection = LocalVelocityDirection;

	if(bIsSprint)
	{
		Override_MoveStartPose_Cached = Override_SprintStartPose;
	}
	else
	{
		Override_MoveStartPose_Cached = GetStartOverride();
	}

	if(GetMovementComponent()->bOrientRotationToMovement)
	{
		MaxTurnYawValue = 0.f;
		TurnYawCurveValue = 0.f;
		ReachedEndOfTurn = LastDirection == ECardinalDirection::Forward || StartCardinals.Forward == GetStartAnim();
	}
}

void ULocomotionAnimInstance::UpdateStartState(const FAnimUpdateContext& Context,const FAnimNodeReference& Node)
{
	if(Override_MoveStartPose_Cached.Override_Weight == 0.f)
	{
		if(bIsSprint)
		{
			Override_MoveStartPose_Cached = Override_SprintStartPose;
		}
		else
		{
			Override_MoveStartPose_Cached = GetStartOverride();
		}
	}

	FAnimationStateResultReference ResultReference{};
	bool TempResult = false;
	UAnimationStateMachineLibrary::ConvertToAnimationStateResultPure(Node,ResultReference,TempResult);
	if(!UAnimationStateMachineLibrary::IsStateBlendingOut(Context,ResultReference))
	{
		if(GetMovementComponent()->bOrientRotationToMovement)
		{
			ProcessTurnYawForwardFacing();
			RootYawOffsetMode = ReachedEndOfTurn ? ERootYawOffsetMode::BlendOut : ERootYawOffsetMode::Accumulate;
		}
		else
		{
			RootYawOffsetMode = ERootYawOffsetMode::Hold;
		}
	}
}

void ULocomotionAnimInstance::UpdateStopState(const FAnimUpdateContext& Context,const  FAnimNodeReference& Node)
{
	FAnimationStateResultReference ResultReference{};
	bool TempResult = false;
	UAnimationStateMachineLibrary::ConvertToAnimationStateResultPure(Node,ResultReference,TempResult);
	if(!UAnimationStateMachineLibrary::IsStateBlendingOut(Context,ResultReference))
	{
		RootYawOffsetMode = ERootYawOffsetMode::Accumulate; 
	}
}

void ULocomotionAnimInstance::SetUpPivotState(const FAnimUpdateContext& Context,const  FAnimNodeReference& Node)
{
	PivotInitialDirection = LocalVelocityDirection; 
}

void ULocomotionAnimInstance::UpdatePivotState(const FAnimUpdateContext& Context,const  FAnimNodeReference& Node)
{
	if(LastPivotTime > 0.f)
	{
		LastPivotTime -= Context.GetContext()->GetDeltaTime();
	}
}

ECardinalDirection ULocomotionAnimInstance::SelectCardinalDirectionFromAngle(float Angle, float DeadZone,
	ECardinalDirection CurrentDirection, bool UseCurrentDirection)
{
	const float AbsAngle = UKismetMathLibrary::Abs(Angle);
	
	// Favor playing Fwd/Bwd anims over strafing anims.
	float FwdDeadZone = DeadZone;
	float BwdDeadZone = DeadZone;

	if(UseCurrentDirection)
	{
		if(CurrentDirection == ECardinalDirection::Forward)
		{
			// If moving Fwd, double the Fwd dead zone.
			// It should be harder to leave Fwd when moving Fwd.
			// When moving Left/Right, the dead zone will be smaller so we don't rapidly toggle between directions.
			FwdDeadZone *= 2.f;
		}
		
		else if(CurrentDirection == ECardinalDirection::Backward)
		{
			// See comment above for Fwd.
			BwdDeadZone *= 2.f;
		}	
	}
	
	// Forward
	if(AbsAngle <= FwdDeadZone + 45.f)
	{
		return ECardinalDirection::Forward;
	}
	else if(AbsAngle >= 135 - BwdDeadZone)
	{
		return ECardinalDirection::Backward;
	}
	else if(Angle >0.f)
	{
		return ECardinalDirection::Right;
	}
	else
	{
		return ECardinalDirection::Left;
	}
	return ECardinalDirection::None;
}

ECardinalDirection ULocomotionAnimInstance::GetOppositeCardinalDirection(ECardinalDirection CurrentDirection)
{
	if(CurrentDirection == ECardinalDirection::Forward)
	{
		return ECardinalDirection::Backward;
	}
	if(CurrentDirection == ECardinalDirection::Backward)
	{
		return ECardinalDirection::Forward;
	}
	if(CurrentDirection == ECardinalDirection::Left)
	{
		return ECardinalDirection::Right;
	}
	if(CurrentDirection == ECardinalDirection::Right)
	{
		return ECardinalDirection::Left;
	}
	return ECardinalDirection::None;
}

bool ULocomotionAnimInstance::IsMovingPerpendicularToInitialPivot()
{
	// We stay in a pivot when pivoting along a line (e.g. triggering a left-right pivot while playing a right-left pivot)
	// but break out if the character makes a perpendicular change in direction.

	const bool bIsPivotingOnForwardAxis =
		PivotInitialDirection == ECardinalDirection::Forward ||
		PivotInitialDirection == ECardinalDirection::Backward;

	const bool bIsMovingOnSideAxis =
		LocalVelocityDirection == ECardinalDirection::Left ||
		LocalVelocityDirection == ECardinalDirection::Right;

	const bool bIsPivotingOnSideAxis =
		PivotInitialDirection == ECardinalDirection::Left ||
		PivotInitialDirection == ECardinalDirection::Right;

	const bool bIsMovingOnForwardAxis =
		LocalVelocityDirection == ECardinalDirection::Forward ||
		LocalVelocityDirection == ECardinalDirection::Backward;

	const bool bHasTurnedPerpendicularlyFromForwardAxis = (bIsPivotingOnForwardAxis && !bIsMovingOnForwardAxis);
	const bool bIsPivotOnSideOrMovingOnForwardAxis = (bIsPivotingOnSideAxis || !bIsMovingOnSideAxis);
	return bHasTurnedPerpendicularlyFromForwardAxis || bIsPivotOnSideOrMovingOnForwardAxis; 
}

bool ULocomotionAnimInstance::CanPlayIdleBreak()
{
	const bool bIsInBlockingState =	IsCrouching || GameplayTag_IsFiring || IsAnyMontagePlaying() || HasVelocity || IsJumping;
	if(Idle_Breaks.Num() > 0 && !bIsInBlockingState)
	{
		return true;
	}
	return false;
}

void ULocomotionAnimInstance::ResetIdleBreakTransitionLogic()
{
	TimeUntilNextIdleBreak = IdleBreakDelayTime;
}

void ULocomotionAnimInstance::ProcessIdleBreakTransitionLogic(float DeltaTime)
{
	if(CanPlayIdleBreak())
	{
		TimeUntilNextIdleBreak -= DeltaTime;
	}
	else
	{
		ResetIdleBreakTransitionLogic();
	}
}

void ULocomotionAnimInstance::ChooseIdleBreakDelayTime()
{
	// Use some logic that depends on the location of the Pawn owner to have roughly consistent behavior across clients without having every character play an idle break at the same time.
	const float PositionHash = UKismetMathLibrary::Abs(WorldLocation.X + WorldLocation.Y);
	const int32 BaseDelay = static_cast<int32>(PositionHash) % 10;
	IdleBreakDelayTime = static_cast<float>(BaseDelay + 6);
}

void ULocomotionAnimInstance::SetUpIdleState_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node)
{
	ChooseIdleBreakDelayTime();
	ResetIdleBreakTransitionLogic();
}

void ULocomotionAnimInstance::UpdateIdleState_Layer(const FAnimUpdateContext& Context,const  FAnimNodeReference& Node)
{
	FAnimationStateResultReference ResultReference{};
	bool TempResult = false;
	UAnimationStateMachineLibrary::ConvertToAnimationStateResultPure(Node,ResultReference,TempResult);
	if(!UAnimationStateMachineLibrary::IsStateBlendingOut(Context,ResultReference))
	{
		ProcessIdleBreakTransitionLogic(Context.GetContext()->GetDeltaTime());
	}
}

void ULocomotionAnimInstance::SetupIdleTransition_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node)
{
	FSequencePlayerReference SequencePlayer{};
	bool TempResult = false;
	USequencePlayerLibrary::ConvertToSequencePlayerPure(Node,SequencePlayer,TempResult);

	USequencePlayerLibrary::SetSequence(SequencePlayer, IsCrouching ? CrouchIdleEntry : CrouchIdleExit);
}

void ULocomotionAnimInstance::UpdateIdleAnim_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node)
{
	FSequencePlayerReference SequencePlayer{};
	bool TempResult = false;
	USequencePlayerLibrary::ConvertToSequencePlayerPure(Node,SequencePlayer,TempResult);

	if(nullptr == IdleAnim)
	{
		// DebugHeader::LogWarning(TEXT("UpdateIdleAnim_Layer : IdleAnim is nullptr"));
	}
	USequencePlayerLibrary::SetSequenceWithInertialBlending(Context,SequencePlayer,IdleAnim,0.3f);
}

void ULocomotionAnimInstance::SetUpIdleBreakAnim_Layer(const FAnimUpdateContext& Context,const  FAnimNodeReference& Node)
{
	FSequencePlayerReference SequencePlayer{};
	bool TempResult = false;
	USequencePlayerLibrary::ConvertToSequencePlayerPure(Node,SequencePlayer,TempResult);

	if(Idle_Breaks.IsValidIndex(CurrentIdleBreakIndex))
	{
		USequencePlayerLibrary::SetSequence(SequencePlayer,Idle_Breaks[CurrentIdleBreakIndex]);
		if(Idle_Breaks.Num() >= ++CurrentIdleBreakIndex)
		{
			CurrentIdleBreakIndex = 0;
		}
	}
	else
	{
		DebugHeader::LogWarning(TEXT("SetUpIdleBreakAnim_Layer : Idle_Breaks[CurrentIdleBreakIndex] is not valid index"));
	}
}

void ULocomotionAnimInstance::SetUpStartAnim_Layer(const FAnimUpdateContext& Context,const  FAnimNodeReference& Node)
{
	FSequenceEvaluatorReference SequenceEvaluator{};
	bool TempResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node,SequenceEvaluator,TempResult);

	FSequenceEvaluatorReference ResultSequenceEvaluator = USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator,GetStartAnim());
	USequenceEvaluatorLibrary::SetExplicitTime(ResultSequenceEvaluator,0.0f);
	StrideWarpingStartAlpha = 0.f;
	if(GetMovementComponent())
	{
		LastDirection = GetMovementComponent()->bOrientRotationToMovement ? ECardinalDirection::Forward : LastDirection;
	}
	else
	{
		DebugHeader::LogWarning(TEXT("SetUpStartAnim_Layer : GetMovementComponent() is nullptr"));
	}
}

void ULocomotionAnimInstance::UpdateStartAnim_Layer(const FAnimUpdateContext& Context,const  FAnimNodeReference& Node)
{
	FSequenceEvaluatorReference SequenceEvaluator{};
	bool TempResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node,SequenceEvaluator,TempResult);
	const float ExplicitTime = USequenceEvaluatorLibrary::GetAccumulatedTime(SequenceEvaluator);

	// Alpha = (ExplicitTime - Offset)/Duration
	StrideWarpingStartAlpha = UKismetMathLibrary::MapRangeClamped(ExplicitTime - StrideWarpingBlendInStartOffset,0.f,StrideWarpingBlendInDurationScaled,0.f,1.f);

	// AnimBP Tour #9
	// This is an example of using Distance Matching to ensure that the distance traveled by the Start animation matches the distance traveled by the Pawn owner. This prevents foot sliding by keeping the animation and the motion model in sync.
	// This effectively controls the play rate of the Start animation. We clamp the effective play rate to prevent the animation from playing too slowly or too quickly.
	// If the effective play rate is clamped, we will still see some sliding. To fix this, we use Stride Warping later to adjust the pose to correct for the remaining difference.
	// The Animation Locomotion Library plugin is required to have access to Distance Matching functions.

	// Smoothly increase the minimum playrate speed, as we blend in stride warping
	FVector2D TargetPlayRateClampStart = bIsSprint ? PlayRateClampStarts_Sprint : PlayRateClampStarts;;
	FVector2D BlendedPlayRateClampStart {
		UKismetMathLibrary::Lerp(StrideWarpingBlendInDurationScaled,TargetPlayRateClampStart.X,StrideWarpingStartAlpha),
		TargetPlayRateClampStart.Y
	};
	
	UAnimDistanceMatchingLibrary::AdvanceTimeByDistanceMatching(Context,SequenceEvaluator,DisplacementSinceLastUpdate,LocomotionDistanceCurveName,BlendedPlayRateClampStart);
}

void ULocomotionAnimInstance::UpdateCycleAnim_Layer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequencePlayerReference SequencePlayer{};
	bool TempResult = false;
	USequencePlayerLibrary::ConvertToSequencePlayerPure(Node,SequencePlayer,TempResult);

	UAnimSequence* Sequence = nullptr;
	switch (LocalVelocityDirectionNoOffset)
	{
	case ECardinalDirection::Forward:
		Sequence = bIsSprint ? SprintCycle : CycleCardinals.Forward;
		break;
	case ECardinalDirection::Backward:
		Sequence = CycleCardinals.Backward;
		break;
	case ECardinalDirection::Right:
		Sequence=	CycleCardinals.Right;
		break;
	case ECardinalDirection::Left:
		Sequence = CycleCardinals.Left;
		break;
	}
	
	FSequencePlayerReference PlayerReference = USequencePlayerLibrary::SetSequenceWithInertialBlending(Context,SequencePlayer,Sequence,0.25f);
	UAnimDistanceMatchingLibrary::SetPlayrateToMatchSpeed(PlayerReference,DisplacementSpeed,bIsSprint ? PlayRateClampCycle_Sprint : PlayRateClampCycle);
	
	StrideWarpingCycleAlpha = UKismetMathLibrary::FInterpTo(
		StrideWarpingCycleAlpha,
		IsRunningIntoWall? 1.f : 0.5f,
		Context.GetContext()->GetDeltaTime(),
		10.f);
}

void ULocomotionAnimInstance::SetUpStopAnim_Layer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequenceEvaluatorReference SequenceEvaluator{};
	bool TempResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node,SequenceEvaluator,TempResult);

	UAnimSequence* Sequence = nullptr;
	switch (LocalVelocityDirection)
	{
	case ECardinalDirection::Forward:
		Sequence = bIsSprint ? SprintStop : StopCardinals.Forward;
		break;
	case ECardinalDirection::Backward:
		Sequence = StopCardinals.Backward;
		break;
	case ECardinalDirection::Right:
		Sequence=	StopCardinals.Right;
		break;
	case ECardinalDirection::Left:
		Sequence = StopCardinals.Left;
		break;
	}
	USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator,Sequence);
	
	// If we got here, and we can't distance match a stop on start, match to 0 distance
	if(!ShouldDistanceMatchStop())
	{
		UAnimDistanceMatchingLibrary::DistanceMatchToTarget(SequenceEvaluator,0.f, LocomotionDistanceCurveName);
	}
}

void ULocomotionAnimInstance::UpdateStopAnim_Layer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequenceEvaluatorReference SequenceEvaluator{};
	bool TempResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, TempResult);
	
	const bool bWantsToDistanceMatch = ShouldDistanceMatchStop();
	const float DistanceToMatch = bWantsToDistanceMatch ? GetPredictedStopDistance() : 0.f;
	
	if (bWantsToDistanceMatch && DistanceToMatch > 0.f)
	{
		// Distance Match to the stop point
		UAnimDistanceMatchingLibrary::DistanceMatchToTarget(SequenceEvaluator, DistanceToMatch, LocomotionDistanceCurveName);
	}
	else
	{
		USequenceEvaluatorLibrary::AdvanceTime(Context, SequenceEvaluator, 1.f);
	}
}

void ULocomotionAnimInstance::SetUpPivotAnim_Layer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequenceEvaluatorReference SequenceEvaluator{};
	bool TempResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, TempResult);
	
	PivotStartingAcceleration = LocalAcceleration2D;

	UAnimSequence* Sequence = nullptr;
	switch (CardinalDirectionFromAcceleration)
	{
	case ECardinalDirection::Forward:
		Sequence = PivotCardinals.Forward;
		break;
	case ECardinalDirection::Backward:
		Sequence = PivotCardinals.Backward;
		break;
	case ECardinalDirection::Right:
		Sequence=	PivotCardinals.Right;
		break;
	case ECardinalDirection::Left:
		Sequence = PivotCardinals.Left;
		break;
	}
	FSequenceEvaluatorReference ResultSequenceEvaluator = USequenceEvaluatorLibrary::SetSequence(SequenceEvaluator, Sequence);
	USequenceEvaluatorLibrary::SetExplicitTime(ResultSequenceEvaluator, 0.f);
	StrideWarpingPivotAlpha = 0.f;
	TimeAtPivotStop = 0.f;
	LastPivotTime = 0.2f;
}


void ULocomotionAnimInstance::UpdatePivotAnim_Layer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequenceEvaluatorReference SequenceEvaluator{};
	bool TempResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, TempResult);

	float ExplicitTime = USequenceEvaluatorLibrary::GetAccumulatedTime(SequenceEvaluator);

	// Allow switching the selected pivot for a short duration at the beginning
	if(LastPivotTime > 0.f)
	{
		UAnimSequence* Sequence = nullptr;
		switch (CardinalDirectionFromAcceleration)
		{
		case ECardinalDirection::Forward:
			Sequence = PivotCardinals.Forward;
			break;
		case ECardinalDirection::Backward:
			Sequence = PivotCardinals.Backward;
			break;
		case ECardinalDirection::Right:
			Sequence=	PivotCardinals.Right;
			break;
		case ECardinalDirection::Left:
			Sequence = PivotCardinals.Left;
			break;
		}
		if(Sequence != USequenceEvaluatorLibrary::GetSequence(SequenceEvaluator))
		{
			USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context,SequenceEvaluator,Sequence);
			PivotStartingAcceleration = LocalVelocity2D;
		}
	}

	// Does acceleration oppose velocity?
	if(LocalVelocity2D.Dot(LocalAcceleration2D) < 0.f && GetMovementComponent())
	{
		const FVector PivotLocation = UAnimCharacterMovementLibrary::PredictGroundMovementPivotLocation(
			GetMovementComponent()->GetCurrentAcceleration(),
			GetMovementComponent()->GetLastUpdateVelocity(),
			GetMovementComponent()->GroundFriction);
		UAnimDistanceMatchingLibrary::DistanceMatchToTarget(SequenceEvaluator,PivotLocation.Size2D(),LocomotionDistanceCurveName);
		TimeAtPivotStop = ExplicitTime;
	}
	else
	{
		// Alpha = (ExplicitTime - StopTime - Offset)/Duration
		// We want the blend in to start after we've already stopped, and just started accelerating
		float ElapsedTimeSinceBlendStart = ExplicitTime - TimeAtPivotStop -StrideWarpingBlendInStartOffset;
		StrideWarpingPivotAlpha = UKismetMathLibrary::MapRangeClamped(ElapsedTimeSinceBlendStart,0.f,StrideWarpingBlendInDurationScaled,0.f,1.f);

		// Smoothly increase the minimum playrate speed, as we blend in stride warping
		FVector2D BlendedPlayRateClamp{UKismetMathLibrary::Lerp(0.2f,PlayRateClampPivots.X,StrideWarpingPivotAlpha),PlayRateClampPivots.Y};
		
		// Once acceleration and velocity are aligned, the character is accelerating away from the pivot point,
		// so we just advance time by distance traveled for the rest of the animation.
		UAnimDistanceMatchingLibrary::AdvanceTimeByDistanceMatching(Context,SequenceEvaluator,DisplacementSinceLastUpdate,LocomotionDistanceCurveName,BlendedPlayRateClamp);
	}
}

void ULocomotionAnimInstance::SetUpFallLandAnim_Layer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequenceEvaluatorReference SequenceEvaluator{};
	bool TempResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, TempResult);
	
	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator, 0.0f);
}

void ULocomotionAnimInstance::UpdateFallLandAnim_Layer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequenceEvaluatorReference SequenceEvaluator{};
	bool TempResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node, SequenceEvaluator, TempResult);

	UAnimDistanceMatchingLibrary::DistanceMatchToTarget(SequenceEvaluator,GroundDistance,JumpDistanceCurveName);
}

void ULocomotionAnimInstance::UpdateJumpFallData_Layer(float InDeltaTime)
{
	if(IsFalling)
	{
		TimeFalling += InDeltaTime;
	}
	else
	{
		if(IsJumping)
		{
			TimeFalling = 0.f;
		}
	}
}

void ULocomotionAnimInstance::LandRecoveryStartLayer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	float ClampedTime = UKismetMathLibrary::MapRangeClamped(TimeFalling,0.f,0.4f,0.1f,1.f);
	LandRecoveryAlpha = IsCrouching ? ClampedTime * 0.5f : ClampedTime;
}

float ULocomotionAnimInstance::GetPredictedStopDistance()
{
	if(UCharacterMovementComponent* MovementComponent = GetMovementComponent())
	{
		FVector StopLocation = UAnimCharacterMovementLibrary::PredictGroundMovementStopLocation(
			MovementComponent->GetLastUpdateVelocity(),
			MovementComponent->bUseSeparateBrakingFriction,
			MovementComponent->BrakingFriction,
			MovementComponent->GroundFriction,
			MovementComponent->BrakingFrictionFactor,
			MovementComponent->BrakingDecelerationWalking
			);
		return StopLocation.Size2D();
	}
	return 0.f;
}

bool ULocomotionAnimInstance::ShouldDistanceMatchStop()
{
	if(!TryGetPawnOwner())
	{
		return false;
	}
	
	bool bShouldPreventStop  = TryGetPawnOwner()->IsLocallyControlled() || TryGetPawnOwner()->HasAuthority() ? bHasAcceleration : PlayStartAnim;
	return !bShouldPreventStop  && HasVelocity;
}

void ULocomotionAnimInstance::SetupTurnInPlaceAnim_Layer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	TurnInPlaceAnimTime = 0.f;
	
	FSequenceEvaluatorReference SequenceEvaluator{};
	bool TempResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node,SequenceEvaluator,TempResult);

	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator,0.f);
	TurnInPlaceB = !TurnInPlaceB;
}

UAnimSequence* ULocomotionAnimInstance::SelectTurnInPlaceAnimation(float Direction)
{
	return Direction > 0.f ? TurnInPlace.Right_90 : TurnInPlace.Left_90; 
}

void ULocomotionAnimInstance::UpdateTurnInPlaceAnim_Layer(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FSequenceEvaluatorReference SequenceEvaluator{};
	bool TempResult = false;
	USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node,SequenceEvaluator,TempResult);
	
	USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(
		Context,
		SequenceEvaluator,
		SelectTurnInPlaceAnimation(TurnInPlaceRotationDirection),
		0.2f);

	TurnInPlaceAnimTime = TurnInPlace.PlayRate * Context.GetContext()->GetDeltaTime() + TurnInPlaceAnimTime;

	USequenceEvaluatorLibrary::SetExplicitTime(SequenceEvaluator,TurnInPlaceAnimTime);
}

void ULocomotionAnimInstance::SetUpTurnInPlaceRotationState_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node)
{
	TurnInPlaceRotationDirection = UKismetMathLibrary::SignOfFloat(RootYawOffset) * -1.f;
}

void ULocomotionAnimInstance::SetUpTurnInPlaceRecoveryState_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node)
{
	TurnInPlaceRecoveryDirection = TurnInPlaceRotationDirection;
}

void ULocomotionAnimInstance::UpdateTurnInPlaceRecoveryAnim_Layer(const FAnimUpdateContext& Context,const FAnimNodeReference& Node)
{
	FSequencePlayerReference SequencePlayer{};
	bool TempResult = false;
	USequencePlayerLibrary::ConvertToSequencePlayerPure(Node,SequencePlayer,TempResult);
	
	USequencePlayerLibrary::SetSequenceWithInertialBlending(Context,SequencePlayer,SelectTurnInPlaceAnimation(TurnInPlaceRecoveryDirection),0.2f);
}

void ULocomotionAnimInstance::UpdateOverrideGlobalPose(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if(Override_GlobalPose.Override_Weight > 0.f)
	{
		FSequenceEvaluatorReference SequenceEvaluator{};
		bool TempResult = false;
		USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node,SequenceEvaluator,TempResult);

		FSequenceEvaluatorReference ResultSequenceEvaluator = USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context,SequenceEvaluator,Override_GlobalPose.Sequence,0.2f);
		USequenceEvaluatorLibrary::SetExplicitTime(ResultSequenceEvaluator,Override_GlobalPose.StartPosition);
	}
}

void ULocomotionAnimInstance::UpdateOverrideIdlePose(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if(Override_IdlePose.Override_Weight > 0.f)
	{
		FSequenceEvaluatorReference SequenceEvaluator{};
		bool TempResult = false;
		USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node,SequenceEvaluator,TempResult);
		
		FSequenceEvaluatorReference ResultSequenceEvaluator = USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context,SequenceEvaluator,Override_IdlePose.Sequence,0.2f);
   		USequenceEvaluatorLibrary::SetExplicitTime(ResultSequenceEvaluator,Override_IdlePose.StartPosition);
	}
}

void ULocomotionAnimInstance::UpdateOverrideJumpPose(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if(Override_JumpPose.Override_Weight > 0.f)
	{
		FSequenceEvaluatorReference SequenceEvaluator{};
		bool TempResult = false;
		USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node,SequenceEvaluator,TempResult);
		
		FSequenceEvaluatorReference ResultSequenceEvaluator = USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context,SequenceEvaluator,Override_JumpPose.Sequence,0.2f);
		USequenceEvaluatorLibrary::SetExplicitTime(ResultSequenceEvaluator,Override_JumpPose.StartPosition);
	}
}

void ULocomotionAnimInstance::UpdateOverrideMoveStartPose(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if(Override_MoveStartPose.Override_Weight > 0.f)
	{
		FSequenceEvaluatorReference SequenceEvaluator{};
		bool TempResult = false;
		USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node,SequenceEvaluator,TempResult);
		
		FSequenceEvaluatorReference ResultSequenceEvaluator = USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context,SequenceEvaluator,Override_MoveStartPose.Sequence,0.2f);
		USequenceEvaluatorLibrary::SetExplicitTime(ResultSequenceEvaluator,Override_MoveStartPose.StartPosition);
	}
}

void ULocomotionAnimInstance::UpdateOverrideMoveStopPose(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if(Override_MoveStopPose.Override_Weight > 0.f)
	{
		FSequenceEvaluatorReference SequenceEvaluator{};
		bool TempResult = false;
		USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node,SequenceEvaluator,TempResult);
		
		FSequenceEvaluatorReference ResultSequenceEvaluator = USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context,SequenceEvaluator,Override_MoveStopPose.Sequence,0.2f);
		USequenceEvaluatorLibrary::SetExplicitTime(ResultSequenceEvaluator,Override_MoveStopPose.StartPosition);
	}
}

void ULocomotionAnimInstance::UpdateOverrideMoveCyclePose(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	FLocomotionOverride_Sequence Sequence = bIsSprint ? Override_SprintCyclePose : Override_MoveCyclePose;
	if(Sequence.Override_Weight > 0.f)
	{
		FSequenceEvaluatorReference SequenceEvaluator{};
		bool TempResult = false;
		USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node,SequenceEvaluator,TempResult);
		
		FSequenceEvaluatorReference ResultSequenceEvaluator = USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context,SequenceEvaluator,Sequence.Sequence,0.2f);
		USequenceEvaluatorLibrary::SetExplicitTime(ResultSequenceEvaluator,Sequence.StartPosition);
	}
}

void ULocomotionAnimInstance::UpdateOverrideMovePivotPose(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if(Override_MovePivotPose.Override_Weight > 0.f)
	{
		FSequenceEvaluatorReference SequenceEvaluator{};
		bool TempResult = false;
		USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node,SequenceEvaluator,TempResult);
		
		FSequenceEvaluatorReference ResultSequenceEvaluator = USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context,SequenceEvaluator,Override_MovePivotPose.Sequence,0.2f);
		USequenceEvaluatorLibrary::SetExplicitTime(ResultSequenceEvaluator,Override_MovePivotPose.StartPosition);
	}
}

void ULocomotionAnimInstance::UpdateOverrideTurnPose(const FAnimUpdateContext& Context, const FAnimNodeReference& Node)
{
	if(Override_TurnPose.Override_Weight > 0.f)
	{
		FSequenceEvaluatorReference SequenceEvaluator{};
		bool TempResult = false;
		USequenceEvaluatorLibrary::ConvertToSequenceEvaluatorPure(Node,SequenceEvaluator,TempResult);
		
		FSequenceEvaluatorReference ResultSequenceEvaluator = USequenceEvaluatorLibrary::SetSequenceWithInertialBlending(Context,SequenceEvaluator,Override_TurnPose.Sequence,0.2f);
		USequenceEvaluatorLibrary::SetExplicitTime(ResultSequenceEvaluator,Override_TurnPose.StartPosition);
	}
}

FLocomotionMovement ULocomotionAnimInstance::GetMovementStruct()
{
	if(LS_AnimSet.Movement.Find(LS_MovementType))
	{
		return LS_AnimSet.Movement[LS_MovementType];
	}
	return {};
}

FLocomotionOverride_Sequence ULocomotionAnimInstance::GetOverride_Global()
{
	if(GetMovementStruct().Override.Find(LS_OverrideName))
	{
		return GetMovementStruct().Override[LS_OverrideName].GlobalPose;
	}
	return {};
}

void ULocomotionAnimInstance::SetAnimValid()
{
	IsValid_FwdStart =		IsValid(StartCardinals.Forward);
	IsValid_BwdStart =		IsValid(StartCardinals.Backward);
	IsValid_LeftStart =		IsValid(StartCardinals.Left);
	IsValid_RightStart =	IsValid(StartCardinals.Right);

	IsValid_StopFwd =      IsValid(StopCardinals.Forward);
	IsValid_StopBwd =      IsValid(StopCardinals.Backward);
	IsValid_StopLeft =      IsValid(StopCardinals.Left);
	IsValid_StopRight =      IsValid(StopCardinals.Right);

	IsValid_PivotFwd =      IsValid(PivotCardinals.Forward);
	IsValid_PivotBwd =      IsValid(PivotCardinals.Backward);
	IsValid_PivotLeft =      IsValid(PivotCardinals.Left);
	IsValid_PivotRight =      IsValid(PivotCardinals.Right);
}

bool ULocomotionAnimInstance::IsValidStartAnim()
{
	switch (LocalVelocityDirection)
	{
	case ECardinalDirection::Forward:
		return IsValid_FwdStart;
	case ECardinalDirection::Backward:
		return IsValid_BwdStart;
	case ECardinalDirection::Left:
		return IsValid_LeftStart;
	case ECardinalDirection::Right:
		return IsValid_RightStart;
	}
	return false;
}

bool ULocomotionAnimInstance::IsValidStopAnim()
{
	switch (LocalVelocityDirection)
	{
	case ECardinalDirection::Forward:
		return IsValid_StopFwd;
	case ECardinalDirection::Backward:
		return IsValid_StopBwd;
	case ECardinalDirection::Left:
		return IsValid_StopLeft;
	case ECardinalDirection::Right:
		return IsValid_StopRight;
	}
	return false;
}

bool ULocomotionAnimInstance::IsValidPivotAnim()
{
	switch (LocalVelocityDirection)
	{
	case ECardinalDirection::Forward:
		return IsValid_PivotFwd;
	case ECardinalDirection::Backward:
		return IsValid_PivotBwd;
	case ECardinalDirection::Left:
		return IsValid_PivotLeft;
	case ECardinalDirection::Right:
		return IsValid_PivotRight;
	}
	return false;
}

UAnimSequenceBase* ULocomotionAnimInstance::GetStartAnim()
{
	if(nullptr == GetMovementComponent())
	{
		return nullptr;
	}
	UAnimSequence* FinalSequence = nullptr;
	
	UAnimSequence* LastDirectionSequence = nullptr;
	switch (LastDirection)
	{
	case ECardinalDirection::Forward:
		LastDirectionSequence = bIsSprint ? SprintStart : LookingDirection.Start_Forward;
		break;
	case ECardinalDirection::Backward:
		LastDirectionSequence = LastVelocityDirection > 0.f ? LookingDirection.Start_Forward_Right180 :	LookingDirection.Start_Forward_Left180;
		break;
	case ECardinalDirection::Left:
		LastDirectionSequence = LookingDirection.Start_Forward_Left90;
		break;
	case ECardinalDirection::Right:
		LastDirectionSequence = LookingDirection.Start_Forward_Right90;
		break;
	}
	
	LastDirectionSequence = CachedTurnYawCurveValue != 0.f ? bIsSprint ? SprintStart : LookingDirection.Start_Forward : LastDirectionSequence;

	UAnimSequence* LastVelocityDirectionSequence = nullptr;
	switch (LocalVelocityDirection)
	{
	case ECardinalDirection::Forward:
		LastVelocityDirectionSequence = bIsSprint ? SprintStart : StartCardinals.Forward;
		break;
	case ECardinalDirection::Backward:
		LastVelocityDirectionSequence = StartCardinals.Backward;
		break;
	case ECardinalDirection::Left:
		LastVelocityDirectionSequence = StartCardinals.Left;
		break;
	case ECardinalDirection::Right:
		LastVelocityDirectionSequence = StartCardinals.Right;
		break;
	}
	
	return GetMovementComponent()->bOrientRotationToMovement ? LastDirectionSequence : LastVelocityDirectionSequence; 
}

void ULocomotionAnimInstance::UpdateLocationData(float DeltaTime)
{
	if(nullptr == GetOwningActor())
	{
		return;
	}
	DisplacementSinceLastUpdate = (GetOwningActor()->GetActorLocation() - WorldLocation).Size2D();
	WorldLocation = GetOwningActor()->GetActorLocation();
	DisplacementSpeed = UKismetMathLibrary::SafeDivide(DisplacementSinceLastUpdate,DeltaTime);

	// Ignore distance delta on first tick. We need at least two updates to get a delta.
	if(bIsFirstUpdate)
	{
		DisplacementSinceLastUpdate = 0.f;
		DisplacementSpeed = 0.f;
	}
}

void ULocomotionAnimInstance::UpdateRotationData()
{
	if(nullptr == GetOwningActor())
	{
		return;
	}
	YawDeltaSinceLastUpdate = GetOwningActor()->GetActorRotation().Yaw - WorldRotation.Yaw;
	YawDeltaSpeed = UKismetMathLibrary::SafeDivide(YawDeltaSinceLastUpdate,GetDeltaSeconds());
	WorldRotation = GetOwningActor()->GetActorRotation();

	float FloatValue = 0.f;
	if(LS_MovementType == EPlayerMovementMode::Walk || IsCrouching)
	{
		FloatValue = 0.025;
	}
	else
	{
		FloatValue = 0.0375;
	}
	AdditiveLeanAngle = FloatValue * YawDeltaSpeed;

	if(bIsFirstUpdate)
	{
		// Ignore yaw delta on first tick. We need at least two updates to get a delta.
		YawDeltaSinceLastUpdate = 0.f;
		AdditiveLeanAngle = 0.f;
	}
}

void ULocomotionAnimInstance::UpdateVelocityData()
{
	if(nullptr == TryGetPawnOwner())
	{
		return;
	}
	bool WasMovingLastUpdate = !LocalVelocity2D.IsZero();
	WorldVelocity = TryGetPawnOwner()->GetVelocity();
	FVector WorldVelocity2D = {WorldVelocity.X,WorldVelocity.Y,0.f};
	LocalVelocity2D = WorldRotation.UnrotateVector(WorldVelocity2D);
	LocalVelocityDirectionAngle = UKismetAnimationLibrary::CalculateDirection(WorldVelocity2D,WorldRotation);
	LocalVelocityDirectionAngleWithOffset = LocalVelocityDirectionAngle - RootYawOffset;
	LocalVelocityDirection = SelectCardinalDirectionFromAngle(LocalVelocityDirectionAngleWithOffset,CardinalDirectionDeadZone,LocalVelocityDirection,WasMovingLastUpdate);
	LocalVelocityDirectionNoOffset = SelectCardinalDirectionFromAngle(LocalVelocityDirectionAngle,CardinalDirectionDeadZone,LocalVelocityDirectionNoOffset,WasMovingLastUpdate);
	HasVelocity = !UKismetMathLibrary::NearlyEqual_FloatFloat(LocalVelocity2D.SizeSquared2D(),0.f,0.000001);
}

void ULocomotionAnimInstance::UpdateAccelerationData()
{
	if(nullptr == GetMovementComponent())
	{
		return;
	}
	FVector WorldAcceleration2D{GetMovementComponent()->GetCurrentAcceleration().X,GetMovementComponent()->GetCurrentAcceleration().Y,0.f};
	LocalAcceleration2D = WorldRotation.UnrotateVector(WorldAcceleration2D);
	HasVelocity = !UKismetMathLibrary::NearlyEqual_FloatFloat(LocalAcceleration2D.SizeSquared2D(),0.f,0.000001);
	// Calculate a cardinal direction from acceleration to be used for pivots. Acceleration communicates player intent better for that purpose than velocity does.
	PivotDirection2D = UKismetMathLibrary::VLerp(PivotDirection2D,WorldAcceleration2D.GetSafeNormal2D(0.0001),0.5f).GetSafeNormal2D();
	float CalculatedDirection = UKismetAnimationLibrary::CalculateDirection(PivotDirection2D,WorldRotation);
	CardinalDirectionFromAcceleration = GetOppositeCardinalDirection(SelectCardinalDirectionFromAngle(CalculatedDirection,CardinalDirectionDeadZone,ECardinalDirection::Forward,false));
}

void ULocomotionAnimInstance::UpdateCharacterStateData(float DeltaTime)
{
	if(nullptr == GetMovementComponent())
	{
		// DebugHeader::LogWarning("UpdateCharacterStateData : GetMovementComponent() is nullptr");
		return;
	}
	// On ground state
	IsOnGround = GetMovementComponent()->IsMovingOnGround();

	//Crouch state
	bool WasCrouchingLastUpdate = IsCrouching;
	IsCrouching = GetMovementComponent()->IsCrouching();
	CrouchStateChange = IsCrouching != WasCrouchingLastUpdate;

	// Weapon fired state
	if(GameplayTag_IsFiring)
	{
		TimeSinceFiredWeapon = 0.f;
	}
	else
	{
		TimeSinceFiredWeapon += DeltaTime;
	}

	// In air state
	IsJumping = false;
	IsFalling = false;
	if(GetMovementComponent()->MovementMode == MOVE_Falling)
	{
		if(WorldVelocity.Z > 0)
		{
			IsJumping = true;
		}
		else
		{
			IsFalling = true;
		}
	}
}

void ULocomotionAnimInstance::UpdateJumpFallData()
{
	// 1. 필요한 포인터를 한 번만 호출하여 변수에 저장합니다.
	const ACharacter* LocalCharacter = Cast<ACharacter>(TryGetPawnOwner());
	const UCharacterMovementComponent* LocalMovementComponent = LocalCharacter ? LocalCharacter->GetCharacterMovement() : nullptr;


	if (!OwnerCharacter || !LocalMovementComponent)
	{
		return;
	}

	if (IsJumping)
	{
		const float GravityZ = LocalMovementComponent->GetGravityZ();
		if (FMath::IsNearlyZero(GravityZ))
		{
			TimeToJumpApex = 0.0f;
		}
		else
		{
			TimeToJumpApex = -OwnerCharacter->GetVelocity().Z / GravityZ;
		}
	}
	else
	{
		TimeToJumpApex = 0.0f;
	}
}

void ULocomotionAnimInstance::UpdateWallDetectionHeuristic()
{
	// This logic guesses if the character is running into a wall by checking if there's a large angle between velocity and acceleration
	// (i.e. the character is pushing towards the wall but actually sliding to the side) and if the character is trying to accelerate but speed is relatively low.

	// 캐릭터가 가속하려는 의도가 있는지 확인
	bool bHasSignificantAcceleration = LocalAcceleration2D.Size2D() > 0.1f;

	// 가속 중임에도 속도가 느린지 확인 (벽에 막혔을 가능성)
	bool bIsMovingSlowly = LocalVelocity2D.Size2D() < 200.f;

	// 가속 방향과 실제 이동 방향이 크게 달라 움직임이 굴절되는지 확인 (벽에 미끄러지는 현상)
	bool bIsDeflected = UKismetMathLibrary::InRange_FloatFloat(
	LocalAcceleration2D.GetSafeNormal(0.0001f).Dot(LocalVelocity2D.GetSafeNormal(0.0001f)),
	-0.6f,
	0.6f,
	true,
	true
	);

	IsRunningIntoWall = bHasSignificantAcceleration && bIsMovingSlowly && bIsDeflected;
}

void ULocomotionAnimInstance::UpdateAimingData(float DeltaTime)
{
	float LocalInterpSpeed = AimOffset_InterpSpeed;
	AimPitch = UKismetMathLibrary::FInterpTo(
		AimPitch,
		FMath::Lerp(GetAimOffsetPitch(),0.f,GetCurveValue("DisableAimOffset")),
		DeltaTime,
		LocalInterpSpeed);

	AimYaw = UKismetMathLibrary::FInterpTo(
	AimYaw,
	FMath::Lerp(GetAimOffsetYaw(),0.f,GetCurveValue("DisableAimOffset")),
	DeltaTime,
	LocalInterpSpeed);
}

void ULocomotionAnimInstance::UpdateUpperBodyAlpha(float DeltaTime)
{
	float TargetUpperBodyAlpha  = 1.f;
	if(GetCurveValue("DisableLowerBody") > 0.f || GetCurveValue("TurnYawWeight") > 0.f || bHasAcceleration)
	{
		TargetUpperBodyAlpha  = 0.f;
	}
	UpperBodyAlpha = UKismetMathLibrary::FInterpTo(UpperBodyAlpha,TargetUpperBodyAlpha ,DeltaTime,10.f);
}

void ULocomotionAnimInstance::UpdateSkelControlData(float DeltaTime)
{
	const bool bIsGlobalHandIKDisabled = GetCurveValue("DisableHandIK") > 0.001f;

	const float InterpolatedRightHandAlpha = UKismetMathLibrary::FInterpTo(
		HandIK_Right_Alpha,
		DisableRHandIK ? 0.f : 1.f,
		DeltaTime,
		5.f);
    
	const float RightHandIKDisableWeight = bIsGlobalHandIKDisabled ? 1.f : GetCurveValue("DisableRHandIK");
	HandIK_Right_Alpha = FMath::Clamp(InterpolatedRightHandAlpha - RightHandIKDisableWeight, 0.f, 1.f);

	// --- Left Hand IK ---
	const float InterpolatedLeftHandAlpha = UKismetMathLibrary::FInterpTo(
		HandIK_Left_Alpha,
		DisableLHandIK ? 0.f : 1.f,
		DeltaTime,
		5.f);
	
	const float LeftHandIKDisableWeight = bIsGlobalHandIKDisabled ? 1.f : GetCurveValue("DisableLHandIK");
	
	HandIK_Left_Alpha = FMath::Clamp(InterpolatedLeftHandAlpha - LeftHandIKDisableWeight, 0.f, 1.f);
}

void ULocomotionAnimInstance::UpdateBlendWeightData(float DeltaTime)
{
	bool bShouldApplyUpperBodyAdditive  = IsOnGround && IsAnyMontagePlaying();
	
	UpperBodyDynamicAdditiveWeight = bShouldApplyUpperBodyAdditive  ? 1.f :
	UKismetMathLibrary::FInterpTo(UpperBodyDynamicAdditiveWeight,0.f,DeltaTime,6.f);
}

UCharacterMovementComponent* ULocomotionAnimInstance::GetMovementComponent()
{
	if(nullptr == TryGetPawnOwner())
	{
		// DebugHeader::LogWarning(TEXT("GetMovementComponent : TryGetPawnOwner() is nullptr"));
		return nullptr;
	}
	return Cast<UCharacterMovementComponent>(TryGetPawnOwner()->GetMovementComponent());
}

float ULocomotionAnimInstance::GetAimOffsetYaw()
{
	if(nullptr == LocomotionComponent || nullptr == MountCharacter || nullptr == TryGetPawnOwner())
	{
		return 0.f;
	}
	// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rider", meta=(AllowPrivateAccess))
	// FRotator MountControlRotation;
	const FRotator MountAimRotation = MountIsControlled ? MountCharacter->GetBaseAimRotation() : LocomotionComponent->MountControlRotation;
	const FRotator MountedAimOffset = UKismetMathLibrary::NormalizedDeltaRotator(MountAimRotation,MountCharacter->GetActorRotation());

	const FRotator CharacterAimRotation = TryGetPawnOwner()->IsPawnControlled() ? TryGetPawnOwner()->GetBaseAimRotation() : LocomotionComponent->BaseAimRotation;
	const FRotator UnmountedAimOffset = UKismetMathLibrary::NormalizedDeltaRotator(CharacterAimRotation,TryGetPawnOwner()->GetActorRotation());

	const FRotator FinalAimOffset = IsMounted ? MountedAimOffset : UnmountedAimOffset;

	return UKismetMathLibrary::Clamp(DisableRootYawOffset ? FinalAimOffset.Yaw : -RootYawOffset,
	                                 YawRange.X,
	                                 YawRange.Y
	);
}

float ULocomotionAnimInstance::GetAimOffsetPitch()
{
	if(nullptr == LocomotionComponent || nullptr == MountCharacter || nullptr == TryGetPawnOwner())
	{
		return 0.f;
	}
	float MountedAimPitch = MountIsControlled ? LocomotionComponent->MountControlRotation.Pitch : MountCharacter->GetControlRotation().Pitch;
	FRotator UnmountedAimOffset = UKismetMathLibrary::NormalizedDeltaRotator(TryGetPawnOwner()->GetBaseAimRotation(),TryGetPawnOwner()->GetActorRotation());

	float FinalAimPitch = IsMounted ? MountedAimPitch : UnmountedAimOffset.Pitch;
	return UKismetMathLibrary::Clamp(UKismetMathLibrary::NormalizeAxis(FinalAimPitch),PitchRange.X,PitchRange.Y);
}

void ULocomotionAnimInstance::Initialization()
{
	APawn* LocalPawn = TryGetPawnOwner();
	if(nullptr == LocalPawn)
	{
		DebugHeader::LogWarning(TEXT("Initialization : LocalPawn is nullptr"));
		return;
	}
	OwnerCharacter = Cast<ACharacter>(LocalPawn);
	if(OwnerCharacter)
	{
		OwnerCharacterMovement = OwnerCharacter->GetCharacterMovement();
		if(nullptr == OwnerCharacterMovement)
		{
			DebugHeader::LogWarning(TEXT("Initialization : OwnerCharacterMovement is nullptr"));
			return;
		}

		LocomotionComponent = TryGetPawnOwner()->FindComponentByClass<ULocomotionComponent>();
		if(nullptr == LocomotionComponent)
		{
			DebugHeader::LogWarning(TEXT("Initialization : LocomotionComponent is nullptr"));
			return;
		}
		
		BindLocomotionEvents();
		OwnerCharacter->MovementModeChangedDelegate.AddDynamic(this,&ULocomotionAnimInstance::OnMovementModeChanged);
	}
	else
	{
		DebugHeader::LogWarning(TEXT("Initialization : OwnerCharacter is nullptr"));
		return;
	}

	InitializeEvents();
}

#pragma endregion 

#pragma region LocomotionComponentEvent

void ULocomotionAnimInstance::OnSprintChanged(bool IsSprint)
{
	bIsSprint = IsSprint;
	SprintChanged = true;
	SetAnimationDatas();
	SprintChanged = false;
}

void ULocomotionAnimInstance::OnSetOverrideName(const FName& Name)
{
	LS_OverrideName = Name;
	Cached_ExtendedPose = ExtendedPose;
	SetAnimationDatas();

	if(ExtendedPose != Cached_ExtendedPose)
	{
		ExtendedPoseChanged = true;
		GetWorld()->GetTimerManager().SetTimer(ExtendedPoseChangedTimer,
			[this]()
		{
			ExtendedPoseChanged = false;
		},
		0.1f,
		false);
	}
}

void ULocomotionAnimInstance::OnSetMovementType(EPlayerMovementMode MovementMode)
{
	if(MovementMode == LS_MovementType || nullptr == OwnerCharacterMovement) return;

	LS_MovementType = MovementMode;
	SetAnimationDatas();
	MovementModeChanged = true;

	GetWorld()->GetTimerManager().SetTimer(MovementModeChangedTimer,
	                                       [this]()
	                                       {
		                                       MovementModeChanged = false;
	                                       }, 0.1f,
	                                       false);

	if(IsCrouching != OwnerCharacterMovement->IsCrouching())
	{
		IsCrouching =  OwnerCharacterMovement->IsCrouching();
		CrouchStateChange = true;
		GetWorld()->GetTimerManager().SetTimer(CrouchStateChangeTimer,
		                                       [this]()
		                                       {
			                                       CrouchStateChange = false;
		                                       }, 0.1f,
		                                       false);
	}
}

void ULocomotionAnimInstance::OnSetAnimset(const FName& InAnimsetName)
{
	LS_AnimsetName = InAnimsetName;
	LS_AnimSet = LocomotionComponent->GetAnimSet(InAnimsetName);
	IsValid_Animset = true;
	CurrentIdleBreakIndex = 0;
	SetAnimationDatas();
	AnimsetChanged = true;
	GetWorld()->GetTimerManager().SetTimer(AnimsetChangedTimer,
[this]()
	{
		AnimsetChanged = false;
	},0.1f,
	false);
	
}

void ULocomotionAnimInstance::OnSetRotationMode(ERotationMode RotationMode)
{
	if( LS_RotationMode == RotationMode) return;

	RotationModeChanged = true;
	LS_RotationMode = RotationMode;
	SetAnimationDatas();
	GetWorld()->GetTimerManager().SetTimer(RotationModeChangedTimer,
	                                       [this]()
	                                       {
		                                       RotationModeChanged = false;
	                                       }, 0.1f,
	                                       false);
}

void ULocomotionAnimInstance::OnAccelerationChanged(bool InHasAcceleration, float InLastVelocityDirection)
{
	PlayStartAnim = InHasAcceleration;
	LastVelocityDirection = InLastVelocityDirection;
	LastDirection = SelectCardinalDirectionFromAngle(LastVelocityDirection,CardinalDirectionDeadZone,LocalVelocityDirection,!LocalVelocity2D.IsZero());

	if(PlayStartAnim)
	{
		CachedTurnYawCurveValue = TurnYawCurveValue;
	}
}

void ULocomotionAnimInstance::OnSetMount(AActor* MountActor)
{
	MountCharacter = Cast<ACharacter>(MountActor);
	if(MountCharacter)
	{
		SetAnimationDatas();
		IsMounted = true;
		GetWorld()->GetTimerManager().SetTimer(MountIsControlledTimer,
									   [this]()
									   {
										   if(MountCharacter->IsPawnControlled())
										   {
											   MountIsControlled = true;
										   }
										   else
										   {
											   MountIsControlled = false;
										   }
									   }, 1.f,
									   false);
	}
	else
	{
		MountIsControlled = false;
		MountCharacter = nullptr;
		IsMounted = false;
		SetAnimationDatas();
	}
}

void ULocomotionAnimInstance::OnRootYawOffsetStatus(bool Disable)
{
	DisableRootYawOffset = Disable;
}

void ULocomotionAnimInstance::OnSetGlobalExtendedPose(EExtendedPose Pose)
{
	GlobalExtendedPose = Pose;
}

void ULocomotionAnimInstance::OnMovementModeChanged(ACharacter* InCharacter, EMovementMode PrevMovementMode,
	uint8 PreviousCustomMode)
{
	MovementModeChanged = true;
	GetWorld()->GetTimerManager().SetTimer(MovementModeChangedTimer,
									   [this]()
									   {
										   MovementModeChanged = false;
									   }, 0.1f,
									   false);
}

void ULocomotionAnimInstance::SetAnimationDatas()
{
	if (!LocomotionComponent || LS_AnimSet.Movement.IsEmpty())
	{
		return;
	}
	
	FLocomotionMovement LocalLocomotionMovement{};
	FLocomotionOverride LocalLocomotionOverride{};
	FLocomotion_AimOffset LocalAimOffset{};

	const FLocomotionMovement* FoundMovement = LS_AnimSet.Movement.Find(LS_MovementType);
	if (!FoundMovement)
	{
		UE_LOG(LogTemp, Error, TEXT("Movement type not found in AnimSet. MovementType: %d, AnimsetName: %s"), 
			   static_cast<int32>(LS_MovementType), *LS_AnimsetName.ToString());
		return;
	}
	LocalLocomotionMovement = *FoundMovement;
	
	const FLocomotionOverride* FoundOverride = LocalLocomotionMovement.Override.Find(LS_OverrideName);
	if (FoundOverride)
	{
		LocalLocomotionOverride = *FoundOverride;
	}
	else
	{
		const FLocomotionOverride* DefaultOverride = LocalLocomotionMovement.Override.Find("Default");
		if (DefaultOverride)
		{
			LocalLocomotionOverride = *DefaultOverride;
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("No override found for name '%s' and no 'Default' override available"), 
				   *LS_OverrideName.ToString());
		}
	}

	CrouchIdleEntry = LS_AnimSet.General.CrouchIdleEntry;
	CrouchIdleExit = LS_AnimSet.General.CrouchIdleExit;
	Leans = LS_AnimSet.General.Leans;
	//
	IdleAnim = LocalLocomotionMovement.Idle;
	Idle_Breaks = LocalLocomotionMovement.IdleBreaks;
	TurnInPlace = LocalLocomotionMovement.TurnInPlace;
	if(LocalLocomotionOverride.AimOffset.bEnable)
	{
		LocalAimOffset = LocalLocomotionOverride.AimOffset.AimOffset;
	}
	//
	Jump_Start = LocalLocomotionMovement.Jump.Jump_Start;
	Jump_Start_Loop = LocalLocomotionMovement.Jump.Jump_StartLoop;
	IsValid_JumpStartLoop = IsValid(Jump_Start_Loop);
	Jump_Apex = LocalLocomotionMovement.Jump.Jump_Apex;
	IsValid_JumpApex = IsValid(Jump_Apex);
	Jump_Fall_Loop = LocalLocomotionMovement.Jump.Jump_FallLoop;
	Jump_Fall_Land = LocalLocomotionMovement.Jump.Jump_FallLand;
	IsValid_JumpFallLand = IsValid(Jump_Fall_Land);
	Jump_Recovery_Additive = LocalLocomotionMovement.Jump.Jump_RecoveryAdditive;
//
	StartCardinals =LocalLocomotionMovement.Start_Cardinals;
	StopCardinals = LocalLocomotionMovement.Stop_Cardinals;
	PivotCardinals = LocalLocomotionMovement.Pivot_Cardinals;
	CycleCardinals = LocalLocomotionMovement.Cycle_Cardinals;

	FLocomotion_Config LocalConfig = LocalLocomotionMovement.Config;
	if(LocalLocomotionOverride.Config.bEnable)
	{
		LocalConfig = LocalLocomotionOverride.Config.Config;
	}
//
	PlayRateClampStarts = LocalConfig.PlayRateClampStarts;
	PlayRateClampPivots = LocalConfig.PlayRateClampPivots;
	PlayRateClampCycle = LocalConfig.PlayRateClampCycle;
	DisableLegIK = LocalConfig.DisableLegIK;
	DisableLegIKWhileMoving = LocalConfig.DisableLegIKWhileMoving;
	JumpRecoveryAdditiveAlpha = LocalConfig.JumpRecoveryAdditiveAlpha;
	RotateRootYawOffsetAngleClamp = LocalConfig.RotateRootYawOffsetAngleClamp;
	DisableRootYawOffset = RotateRootYawOffsetAngleClamp.X == 0 && RotateRootYawOffsetAngleClamp.Y == 0;
	DisableIdleBreaks = LocalConfig.DisableIdleBreaks;
	FLocomotion_HandIKSettings LocalHandIKSettings = LocalConfig.HandIKSettings;
//
	Override_GlobalPose = LocalLocomotionOverride.GlobalPose;
	Override_IdlePose = LocalLocomotionOverride.IdlePose;
	Override_JumpPose = LocalLocomotionOverride.JumpPose;
	Override_FacingStartsPose = LocalLocomotionOverride.StartsPose;
	Override_TurnPose = LocalLocomotionOverride.TurnPose;
	Override_SprintStartPose = LocalLocomotionOverride.SprintPose.Start;
	Override_SprintStopPose = LocalLocomotionOverride.SprintPose.Stop;
	Override_SprintCyclePose = LocalLocomotionOverride.SprintPose.Cycle;
	Override_MoveStartPose = LocalLocomotionOverride.MovePose.Start;
	Override_MoveStopPose = LocalLocomotionOverride.MovePose.Stop;
	Override_MoveCyclePose = LocalLocomotionOverride.MovePose.Cycle;
	Override_MovePivotPose = LocalLocomotionOverride.MovePose.Pivot;
	FLocomotionOverride_ExtendedPose LocalOverrideExtendedPose = LocalLocomotionOverride.ExtendedPose;
//
	AimOffset = LocalAimOffset.AimOffsetBlendSpace;
	IsValid_AimOffset = IsValid(AimOffset);
	YawRange = LocalAimOffset.YawRange;
	PitchRange = LocalAimOffset.PitchRange;
	AimingOffset = LocalAimOffset.Offset;
	AimOffset_InterpSpeed = LocalAimOffset.InterpSpeed;
	AimOffset_BlendName = LocalAimOffset.BlendName;
	AimOffset_BlendWeight = LocalAimOffset.BlendWeight;

	if(TryGetPawnOwner() && !TryGetPawnOwner()->IsPawnControlled() && MountCharacter &&!MountCharacter->IsPawnControlled())
	{
		AimingOffset = FVector2D::ZeroVector;
	}
//
	FLocomotion_Sprint TempSprint = LS_AnimSet.General.Sprint;
	FLocomotion_Config TempSprintConfig = TempSprint.Config;
	SprintCycle = TempSprint.Cycle;
	SprintStart = TempSprint.Start;
	SprintStop = TempSprint.Stop;
	PlayRateClampStarts_Sprint = TempSprintConfig.PlayRateClampStarts;
	PlayRateClampCycle_Sprint = TempSprintConfig.PlayRateClampCycle;
	if(bIsSprint)
	{
		LocalHandIKSettings = TempSprintConfig.HandIKSettings;
		DisableLegIK = TempSprintConfig.DisableLegIK;
		DisableLegIKWhileMoving = TempSprintConfig.DisableLegIKWhileMoving;
		JumpRecoveryAdditiveAlpha = TempSprintConfig.JumpRecoveryAdditiveAlpha;
	}
	if(nullptr == SprintStart)
	{
		SprintStart = StartCardinals.Forward;
	}
	if(nullptr == SprintStop)
	{
		SprintStop = StopCardinals.Forward;
	}
	if(nullptr == SprintCycle)
	{
		SprintCycle = CycleCardinals.Forward;
	}
//
	LookingDirection = LocalLocomotionMovement.Start_ForwardFacing;
	SetDefaultStarts();

	if(AimOffsetList.IsValidIndex(AimOffsetIndex) && AimOffsetList[AimOffsetIndex] != AimOffset && IsValid_AimOffset)
	{
		AimOffsetIndex = AimOffsetIndex == 0 ? 1 : 0;
		AimOffsetList[AimOffsetIndex] = AimOffset;
	}
//
	DisableLHandIK = LocalHandIKSettings.DisableLHandIK;
	EffectorLocationHandL = LocalHandIKSettings.EffectorLocation_HandL;
	JointTargetLocationHandL = LocalHandIKSettings.JointTargetLocation_HandL;
	JointTargetBoneHandL = LocalHandIKSettings.JointTargetBone_HandL;
	DisableRHandIK = LocalHandIKSettings.DisableRHandIK;
	EffectorLocationHandR = LocalHandIKSettings.EffectorLocation_HandR;
	JointTargetLocationHandR = LocalHandIKSettings.JointTargetLocation_HandR;
	JointTargetBoneHandR = LocalHandIKSettings.JointTargetBone_HandR;
	HandFKWeight = LocalHandIKSettings.HandFKWeight;
//
	Extended_DisableMoveAnim = LocalOverrideExtendedPose.DisableMoveAnim;
	Extended_OverrideWeight = LocalOverrideExtendedPose.OverrideWeight;
	Extended_InterpSpeed = LocalOverrideExtendedPose.InterpSpeed;
	Extended_BlendTime = LocalOverrideExtendedPose.BlendTime;
	Extended_BlendName = LocalOverrideExtendedPose.BlendName;

	const UEnum* EnumPtr = StaticEnum<EExtendedPose>();
	if (EnumPtr)
	{
		// ExtendedPoseName 문자열에 해당하는 Enum 값을 int64 형태로 직접 찾습니다.
		const FName PoseNameToFind = FName(LocalOverrideExtendedPose.ExtendedPoseName);
		const int64 EnumValue = EnumPtr->GetValueByName(PoseNameToFind);

		// 해당하는 값을 찾았는지 확인합니다. (못 찾으면 INDEX_NONE을 반환)
		if (EnumValue != INDEX_NONE)
		{
			// 찾은 값을 EExtendedPose 타입으로 안전하게 변환하여 할당합니다.
			ExtendedPose = static_cast<EExtendedPose>(EnumValue);
		}
		else
		{
			// Optional: 일치하는 Enum 값을 찾지 못했을 때의 처리 (예: 경고 로그 출력)
			UE_LOG(LogTemp, Warning, TEXT("Could not find a matching EExtendedPose for name: %s"),
			       *PoseNameToFind.ToString());
		}
	}
	

	if(Extended_DisableMoveAnim)
	{
		SprintCycle = IdleAnim;
		SprintStart = IdleAnim;
		SprintStop = IdleAnim;

		FLocomotion_CardinalDirections TempLocomotionCardinalDirection{IdleAnim,IdleAnim,IdleAnim,IdleAnim};
		StartCardinals = TempLocomotionCardinalDirection;
		PivotCardinals = TempLocomotionCardinalDirection;
		CycleCardinals = TempLocomotionCardinalDirection;
		StopCardinals = TempLocomotionCardinalDirection;
		
		FLocomotion_Starts TempLocomotionStarts{IdleAnim,IdleAnim,IdleAnim,IdleAnim,IdleAnim};
		LookingDirection = TempLocomotionStarts;
		
		Override_MoveStartPose = Override_IdlePose;
		Override_MoveStopPose = Override_IdlePose;
		Override_MoveCyclePose = Override_IdlePose;
		Override_MovePivotPose = Override_IdlePose;
		Override_SprintStartPose = Override_IdlePose;
		Override_SprintStopPose = Override_IdlePose;
		Override_SprintCyclePose = Override_IdlePose;
	}
//
	if(!SprintChanged)
	{
		GetStartOverride();
	}
	SetAnimValid();
}

void ULocomotionAnimInstance::SetDefaultStarts()
{
	if(!LookingDirection.Start_Forward)
	{
		LookingDirection.Start_Forward = StartCardinals.Forward;
	}
	
	if(!LookingDirection.Start_Forward_Left90)
	{
		LookingDirection.Start_Forward_Left90 = StartCardinals.Forward;
	}
	if(!LookingDirection.Start_Forward_Right90)
	{
		LookingDirection.Start_Forward_Right90 = StartCardinals.Forward;
	}
	if(!LookingDirection.Start_Forward_Left180)
	{
		LookingDirection.Start_Forward_Left180 = StartCardinals.Forward;
	}
	if(!LookingDirection.Start_Forward_Right180)
	{
		LookingDirection.Start_Forward_Right180 = StartCardinals.Forward;
	}
	
}

#pragma endregion 

