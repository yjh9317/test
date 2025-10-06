#pragma once

#include "CoreMinimal.h"
#include "CameraDefinition.generated.h"

class UCurveVector;

UENUM()
enum ECameraAnimType
{
	ECT_UseCameraCurve UMETA(Display = UseCameraCurve),
	ECT_CameraFollow UMETA(Display = CameraFollow),
	ECT_CustomSpringArmRelativeRotationAndArmLength UMETA(Display = CustomSpringArmRelativeRotationAndArmLength),
};

UENUM()
enum EFollowType
{
	EFT_PlayerCharacterMesh UMETA(Display = PlayerCharacterMesh),
	EFT_PlayerCameraComponent UMETA(Display = PlayerCameraComponent),
	EFT_PlayerRootComponent UMETA(Display = PlayerRootComponent),
	EFT_AICharacterMesh UMETA(Display = AICharacterMesh),
	EFT_CustomComponent UMETA(Display = CustomComponent),
	EFT_CustomTransform UMETA(Display = CustomTransform),
};

USTRUCT(BlueprintType)
struct FCameraFollowSetting
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TEnumAsByte<EFollowType> FollowType;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	FName FollowSocketName;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	USceneComponent* CustomFollowComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	FTransform CustomFollowTransform;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool CameraLookAtSpringArmLocation;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	FTransform AddCameraRelativeTransform;
};

USTRUCT(BlueprintType)
struct FCameraLag
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool EnableCameraLocationLag;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	float CameraLocationLagSpeed;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool EnableCameraRotationLag;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	float CameraRotationLagSpeed;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool ShowDebugLagMarker;
};

USTRUCT(BlueprintType)
struct FSpringArmTransformingSetting
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TEnumAsByte<EFollowType> Type;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	FName FollowSocketName;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	USceneComponent* CustomFollowComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	FTransform CustomFollowTransform;
};

USTRUCT(BlueprintType)
struct FSpringArmAnimSetting
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	float Yaw;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	float Pitch;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	float SpringArmLength;
};


USTRUCT(BlueprintType)
struct FCameraSetting
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool ShouldChangeFOV;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	float TargetFOV;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool EnableBackgroundBlur;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TEnumAsByte<ECameraAnimType> CameraAnimationType;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UCurveVector* CameraAnimCurve;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	FCameraFollowSetting CameraFollowSetting;
		
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	FSpringArmAnimSetting CustomSpringArmRelativeRotationAndArmLength;
};


USTRUCT(BlueprintType)
struct FSpringArmSetting
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	TArray<FSpringArmTransformingSetting> SpringArmTransformSettings;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	FVector AddSpringArmRelativeLocation;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UCurveVector* AddSpringArmRelativeLocationByCurve;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	FCameraLag CameraLagSetting;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	bool AutoAdjustSpringArmLengthWhenPlayerOffScreen;
};

USTRUCT(BlueprintType)
struct FPlayerInputSetting
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool DisableGameInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool DisableMoveInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool DisableTurnInput;
};

USTRUCT(BlueprintType)
struct FCameraTotalSetting
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool UseMontageDuration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CustomDuration;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float BlendInTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float BlendOutTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanInterruptOtherCloseups;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool AffectControlRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CameraLevel;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool AutoBlendOutWhenPlayOtherAnimation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FPlayerInputSetting PlayerInputSetting;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FSpringArmSetting SpringArmSetting;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FCameraSetting CameraSetting;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool ShouldDoCollisionTest;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CollisionTestRadius;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool ShowDebugCameraLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool ShowDebugSpringArmLocation;
};