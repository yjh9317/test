#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "Character/BaseCharacter.h"
#include "Interface/ComponentManager.h"
#include "Shared/Action/BaseAction.h"
#include "MainCharacter.generated.h"

class UInputBufferComponent;
class ULocomotionComponent;
// Comp
class UActionComponent;

class UStatsComponent;
class UEquipmentComponent;
class UGrappleComponent;

class UDataAsset_InputConfig;
class AMainHUD;
class UMainAnimInstance;
class AMainPlayerController;
class UTimelineComponent;
class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
class UCustomMovementComponent;
class UMotionWarpingComponent;


UCLASS(BlueprintType)
class RPGPROJECT_API AMainCharacter : public ABaseCharacter, public IComponentManager
{
	GENERATED_BODY()

public:
	AMainCharacter(const FObjectInitializer& ObjectInitializer);
	virtual bool CanJumpInternal_Implementation() const override;

	void LearnNewSkill(FGameplayTag SkillTag, TSubclassOf<UBaseAction> ActionClass);
	// void OnWeaponChanged(AWeapon* NewWeapon);
protected:
	// ─────────────────────────────────────────────────────────────────────
	// Virtual Function
	// ─────────────────────────────────────────────────────────────────────
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void PostInitializeComponents() override;
	virtual void FinishDeath() override;
	
	// ─────────────────────────────────────────────────────────────────────
	// TickFunctions
	// ─────────────────────────────────────────────────────────────────────
	void HideCameraIfCharacterClose();
	void ScreenCenterLineTrace(FHitResult& HitResult);

private:
	// ─────────────────────────────────────────────────────────────────────
	// Defaults
	// ─────────────────────────────────────────────────────────────────────
	void Look(const FInputActionValue& Value);
	void HandleGroundMovementInput(const FInputActionValue& Value);
	void HandleClimbMovementInput(const FInputActionValue& Value);

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* ViewCamera;

	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* CameraBoom;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta = (AllowPrivateAccess = "true"))
	float CameraThreshold = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Movement", meta = (AllowPrivateAccess = "true"))
	FVector MovementDirectionInput;
	
	// ─────────────────────────────────────────────────────────────────────
	// InputActions
	// ─────────────────────────────────────────────────────────────────────
	void AddInputMappingContext(UInputMappingContext* ContextToAdd,int32 InPriority);
	void RemoveInputMappingContext(UInputMappingContext* ContextToAdd);
	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputMappingContext* ClimbMappingContext;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	UDataAsset_InputConfig* InputConfigDataAsset;
	

	// ─────────────────────────────────────────────────────────────────────
	// Components
	// ─────────────────────────────────────────────────────────────────────

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEquipmentComponent> EquipmentComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGrappleComponent> GrappleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<ULocomotionComponent> LocomotionComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputBufferComponent> InputBufferComponent;
protected:
	// Grapple
	void GrappleUse();

public:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	AMainPlayerController* MainPlayerController;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UMainAnimInstance* MainAnimInstance;


private:
	//	About Climb
	// ─────────────────────────────────────────────────────────────────────
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Climb",meta = (AllowPrivateAccess = "true"))
	UCustomMovementComponent* CustomMovementComponent;
	
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Climb",meta = (AllowPrivateAccess = "true"))
	UMotionWarpingComponent* MotionWarpingComponent;
	
	void OnClimbActionStarted(const FInputActionValue& Value);
	void OnClimbHopActionStarted(const FInputActionValue& Value);
	void OnPlayerEnterClimbState();
	void OnPlayerExitClimbState();

protected:
	// ─────────────────────────────────────────────────────────────────────
	// Input
	// ─────────────────────────────────────────────────────────────────────
	
	// 입력 처리 함수
	void HandleLeftClickInput(const FInputActionValue& Value);
	void HandleRightClickInput(const FInputActionValue& Value);
	// ─────────────────────────────────────────────────────────────────────
	// FOV
	// ─────────────────────────────────────────────────────────────────────

	float DefaultFOV;
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFOV = 30.f;
	float CurrentFOV;
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;
	

public:
	// ─────────────────────────────────────────────────────────────────────
	// GET Function
	// ─────────────────────────────────────────────────────────────────────
	FORCEINLINE UCameraComponent* GetViewCamera() const { return ViewCamera; }
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCustomMovementComponent* GetCustomMovementComponent() const { return CustomMovementComponent; }
	FORCEINLINE UMainAnimInstance* GetMainAnimInstance() const { return MainAnimInstance; }
	FORCEINLINE UMotionWarpingComponent* GetMotionWarpingComponent() const { return MotionWarpingComponent; }
	FORCEINLINE AMainPlayerController* GetMainPlayerController() const { return MainPlayerController; }
	FORCEINLINE UEquipmentComponent* GetEquipmentComponent() const { return EquipmentComponent; }

	// ─────────────────────────────────────────────────────────────────────
	// Component Manager
	// ─────────────────────────────────────────────────────────────────────
	virtual UActionComponent* GetActionComponent() override {return ActionComponent;}
	virtual UMotionWarpingComponent* GetMotionWarpingComponent() override { return MotionWarpingComponent;}
	
	
	bool IsWeaponEquipped() const;
	UFUNCTION(BlueprintPure, Category = "Character|Movement")
	FVector GetMovementDirectionInput() const { return MovementDirectionInput; }
};
