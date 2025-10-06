// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "RPG_PlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API ARPG_PlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	ARPG_PlayerCameraManager();

	virtual void InitializeFor(class APlayerController* PC) override;

	UFUNCTION(BlueprintCallable, Category = "RPG|Camera Manager")
	void RefreshLevelSequences();

	UFUNCTION(BlueprintPure, Category = "RPG|Camera Manager")
	bool IsPlayingAnyLevelSequence() const { return NbrActiveLevelSequences > 0; }

	// Plays a single new camera animation sequence. Any subsequent calls while this animation runs will interrupt the current animation.
	// This variation can be used in contexts where async nodes aren't allowd (e.g., AnimNotifies).
	UFUNCTION(BlueprintCallable, Category = "RPG|Camera Manager")
	void PlayCameraAnimation(class UCameraAnimationSequence* CameraSequence, struct FRPGCameraAnimationParams const& Params);

	/**
	 *  Enables/Disables all camera modifiers ONLY if they inherit from RPG Camera Modifier.
	 *  @param  bEnabled    - true to enable all RPG camera modifiers, false to disable.
	 *  @param  bImmediate  - If bEnabled is false: true to disable immediately with no blend out, false (default) to allow blend out
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG|Camera Manager")
	void ToggleRPGCameraModifiers(bool const bEnabled, bool const bImmediate = true);

	/**
	 *  Enables/Disables all camera modifiers, regardless of whether they inherit from RPG Camera Modifier.
	 *  @param  bEnabled    - true to enable all camera modifiers, false to disable.
	 *  @param  bImmediate  - If bEnabled is false: true to disable immediately with no blend out, false (default) to allow blend out
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG|Camera Manager")
	void ToggleCameraModifiers(bool const bEnabled, bool const bImmediate = true);

	/**
	 *  Enables/Disables all debugging of camera modifiers ONLY if they inherit from RPG Camera Modifier.
	 *  @param  bEnabled    - true to enable all debugging of RPG camera modifiers, false to disable.
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG|Camera Manager")
	void ToggleAllRPGModifiersDebug(bool const bEnabled);
	/**
	 *
	 *  Enables/Disables all debugging of camera modifiers regardless of whether they inherit from RPG Camera Modifier.
	 *  @param  bEnabled    - true to enable all debugging of all camera modifiers, false to disable.
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG|Camera Manager")
	void ToggleAllModifiersDebug(bool const bEnabled);

	UFUNCTION(BlueprintPure, Category = "RPG|Camera Manager")
	class USpringArmComponent* GetOwnerSpringArmComponent() const { return CameraArm.Get(); }

	UFUNCTION(BlueprintPure, Category = "RPG|Camera Manager")
	class URPG_CameraDataAssetBase* GetCurrentCameraDataAsset() const
	{
		return CameraDataStack.IsEmpty() ? nullptr : CameraDataStack[CameraDataStack.Num() - 1];
	}

	UFUNCTION(BlueprintCallable, Category = "RPG|Camera Manager")
	void PushCameraData(class URPG_CameraDataAssetBase* CameraDA);

	// Pops the most recent Camera DA.
	UFUNCTION(BlueprintCallable, Category = "RPG|Camera Manager")
	void PopCameraDataHead();

	// Pops the given Camera DA. If it isn't in stack, it returns false.
	UFUNCTION(BlueprintCallable, Category = "RPG|Camera Manager")
	void PopCameraData(class URPG_CameraDataAssetBase* CameraDA);

	UFUNCTION(BlueprintNativeEvent, Category = "RPG|Camera Manager")
	void OnCameraDataStackChanged(class URPG_CameraDataAssetBase* CameraDA);

	/** Draw a debug camera shape. */
	UFUNCTION(BlueprintCallable, Category = "RPG|Camera Manager")
	void DrawDebRPGamera(float Duration, bool bDrawCamera, FLinearColor CameraColor, bool bDrawSpringArm,
	                     FLinearColor SpringArmColor, float Thickness);

	FVector GetCameraTurnRate() const { return FVector(YawTurnRate, PitchTurnRate, 0.f); }

	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "RPG|Camera Manager")
	bool IsPlayingAnyCameraAnimation() const;

	UFUNCTION(BlueprintPure, Category = "RPG|Camera Manager|Movement")
	FVector GetOwnerVelocity() const;

	UFUNCTION(BlueprintPure, Category = "RPG|Camera Manager|Movement")
	void ComputeOwnerFloorDist(float SweepDistance, float CapsuleRadius, bool& bOutFloorExists,
	                           float& OutFloorDistance) const;

	UFUNCTION(BlueprintPure, Category = "RPG|Camera Manager|Movement")
	bool IsOwnerFalling() const;

	UFUNCTION(BlueprintPure, Category = "RPG|Camera Manager|Movement")
	bool IsOwnerStrafing() const;

	UFUNCTION(BlueprintPure, Category = "RPG|Camera Manager|Movement")
	bool IsOwnerMovingOnGround() const;

	UFUNCTION(BlueprintPure, Category = "RPG|Camera Manager|Movement")
	void ComputeOwnerFloorNormal(float SweepDistance, float CapsuleRadius, bool& bOutFloorExists,
	                             FVector& OutFloorNormal) const;

	UFUNCTION(BlueprintPure, Category = "RPG|Camera Modifier|Movement")
	void ComputeOwnerSlopeAngle(float& OutSlopePitchDegrees, float& OutSlopeRollDegrees);

	/*
	 * Returns value betwen 1 (the character is looking where they're moving) or -1 (looking in the opposite direction they're moving).
	 * Will return 0 if the character isn't moving.
	 */
	UFUNCTION(BlueprintPure, Category = "RPG|Camera Modifier|Movement")
	float ComputeOwnerLookAndMovementDot();

protected:
	void DoForEachRPGModifier(TFunction<void(class URPG_CameraModifier*)> const& Function);

	// Breaks when the function returns true.
	void DoForEachRPGModifierWithBreak(TFunction<bool(class URPG_CameraModifier*)> const& Function);

	virtual UCameraModifier* AddNewCameraModifier(TSubclassOf<UCameraModifier> ModifierClass) override;
	virtual bool RemoveCameraModifier(UCameraModifier* ModifierToRemove) override;

	virtual void SetViewTarget(AActor* NewViewTarget, FViewTargetTransitionParams TransitionParams) override;

	/**
	 * Returns camera modifier for this camera of the given class, if it exists.
	 * Looks for inherited classes too. If there are multiple modifiers which fit, the first one is returned.
	 */
	UFUNCTION(BlueprintCallable, Category = "RPG|Camera Manager")
	UCameraModifier* FindCameraModifierOfClass(TSubclassOf<UCameraModifier> ModifierClass, bool bIncludeInherited);

	UCameraModifier const* FindCameraModifierOfClass(TSubclassOf<UCameraModifier> ModifierClass,
	                                                 bool bIncludeInherited) const;

	template <typename T>
	T* FindCameraModifierOfType()
	{
		bool constexpr bIncludeInherited = true;
		UCameraModifier* Modifier = FindCameraModifierOfClass(T::StaticClass(), bIncludeInherited);
		return Modifier != nullptr ? Cast<T>(Modifier) : nullptr;
	}

	template <typename T>
	T const* FindCameraModifierOfType() const
	{
		bool constexpr bIncludeInherited = true;
		UCameraModifier const* Modifier = FindCameraModifierOfClass(T::StaticClass(), bIncludeInherited);
		return Modifier != nullptr ? Cast<T>(Modifier) : nullptr;
	}

	/**
	 * Called to give PlayerCameraManager a chance to adjust view rotation updates before they are applied.
	 * e.g. The base implementation enforces view rotation limits using LimitViewPitch, et al.
	 * @param DeltaTime - Frame time in seconds.
	 * @param OutViewRotation - In/out. The view rotation to modify.
	 * @param OutDeltaRot - In/out. How much the rotation changed this frame.
	 */
	virtual void ProcessViewRotation(float DeltaTime, FRotator& OutViewRotation, FRotator& OutDeltaRot);

	/**
	 * Called to give PlayerCameraManager a chance to adjust both the yaw turn rate and pitch turn rate.
	 *
	 * @param DeltaTime - Frame time in seconds.
	 * @param InLocalControlRotation - The difference between the actor rotation and the control rotation.
	 * @param OutPitchTurnRate - Out. New value of the pitch turn rate (between 0 and 1).
	 * @param OutYawTurnRate - Out. New value of the yaw turn rate (between 0 and 1).
	 * @return Return true to prevent subsequent (lower priority) modifiers to further adjust rotation, false otherwise.
	 */
	virtual void ProcessTurnRate(float DeltaTime, FRotator InLocalControlRotation, float& OutPitchTurnRate,
	                             float& OutYawTurnRate);

	virtual void Tick(float DeltaTime) override;

	virtual void LimitViewYaw(FRotator& ViewRotation, float InViewYawMin, float InViewYawMax) override;

	// This updates the internal variables of the RPG Player Camera Manager. Make sure to call the parent function if you override this in BP.
	UFUNCTION(BlueprintNativeEvent, Category = "RPG|Camera Manager|Internal")
	void UpdateInternalVariables(float DeltaTime);

	// Usually uses the RPG Pawn Interface to fetch the rotation input of the camera (Mouse or Right Thumbstick). Override this if you want to provide your own way of getting the camera rotation input.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Camera Manager|Internal")
	FRotator GetRotationInput() const;

	// Usually uses the RPG Pawn Interface to fetch the movement input of the character (WASD or Left Thumbstick). Override this if you want to provide your own way of getting the camera rotation input.
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "RPG|Camera Manager|Internal")
	FVector GetMovementControlInput() const;

	UFUNCTION()
	void OnLevelSequenceStarted();

	UFUNCTION()
	void OnLevelSequenceEnded();

protected:
	friend class URPG_CameraModifier;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "RPG|Camera Manager|Internal")
	TArray<class URPG_CameraDataAssetBase*> CameraDataStack;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "RPG|Camera Manager|Internal")
	TObjectPtr<class ACharacter> OwnerCharacter;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "RPG|Camera Manager|Internal")
	TObjectPtr<class APawn> OwnerPawn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "RPG|Camera Manager|Internal")
	TObjectPtr<class USpringArmComponent> CameraArm;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "RPG|Camera Manager|Internal")
	TObjectPtr<class UCharacterMovementComponent> MovementComponent;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "RPG|Camera Manager|Internal")
	TArray<class ALevelSequenceActor*> LevelSequences;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "RPG|Camera Manager|Angle Constraints",
		meta = (UIMin = 0.f, UIMax = 1.f, ClampMin = 0.f, ClampMax = 1.f))
	float PitchTurnRate = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "RPG|Camera Manager|Angle Constraints",
		meta = (UIMin = 0.f, UIMax = 1.f, ClampMin = 0.f, ClampMax = 1.f))
	float YawTurnRate = 1.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "RPG|Camera Manager|Internal")
	float AspectRatio = 0.f;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "RPG|Camera Manager|Internal")
	float VerticalFOV = 0.f;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "RPG|Camera Manager|Internal")
	float HorizontalFOV = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG|Camera Manager|Internal")
	bool bHasMovementInput = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG|Camera Manager|Internal")
	FVector MovementInput = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG|Camera Manager|Internal")
	float TimeSinceMovementInput = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG|Camera Manager|Internal")
	bool bHasRotationInput = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG|Camera Manager|Internal")
	FRotator RotationInput = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG|Camera Manager|Internal")
	float TimeSinceRotationInput = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "RPG|Camera Manager|Internal")
	TArray<class URPG_CameraModifier*> RPGModifiersList;

	UPROPERTY(Transient)
	int NbrActiveLevelSequences = 0;
};
