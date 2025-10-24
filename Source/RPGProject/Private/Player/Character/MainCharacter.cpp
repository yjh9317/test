#pragma once

#include "Player/Character/MainCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EnhancedInputComponent.h"
#include "MotionWarpingComponent.h"
#include "Shared/Components/CustomInputComponent.h"
#include "Shared/Components/CustomMovementComponent.h"
#include "Engine/LocalPlayer.h"
#include "Grapple/Components/GrappleComponent.h"
#include "Player/Character/MainAnimInstance.h"
#include "Player/Controller/MainPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "RPGGameplayTags.h"
#include "Header/DebugHeader.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Components/InputBufferComponent.h"
#include "Player/Components/LocomotionComponent.h"
#include "Shared/Action/Components/ActionComponent.h"

AMainCharacter::AMainCharacter(const FObjectInitializer& ObjectInitializer)
// Custom Movement Component 초기화
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCustomMovementComponent>(
		ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;
	CustomMovementComponent = Cast<UCustomMovementComponent>(GetCharacterMovement());
	MotionWarpingComponent = CreateDefaultSubobject<UMotionWarpingComponent>(TEXT("MotionWarping"));
	LocomotionComponent = CreateDefaultSubobject<ULocomotionComponent>(TEXT("LocomotionComponent"));
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	// GetMesh()->SetCollisionObjectType(ECC_Player);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 400.f;
	CameraBoom->bUsePawnControlRotation = true;

	ViewCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("ViewCamera"));
	ViewCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	ViewCamera->bUsePawnControlRotation = false;
	
	GrappleComponent = CreateDefaultSubobject<UGrappleComponent>(TEXT("GrappleComp"));
	// TargetSystemComponent = CreateDefaultSubobject<UTargetSystemComponent>(TEXT("TargetSystemComponent"));
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
	
	BaseEyeHeight = 75.f;
	bReplicates = true;
}

#pragma region Basic

void AMainCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}


void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	AddInputMappingContext(DefaultMappingContext, 0);

	MainAnimInstance = Cast<UMainAnimInstance>(GetMesh()->GetAnimInstance());

	if (CustomMovementComponent)
	{
		CustomMovementComponent->OnEnterClimbStateDelegate.BindUObject(this, &ThisClass::OnPlayerEnterClimbState);
		CustomMovementComponent->OnExitClimbStateDelegate.BindUObject(this, &ThisClass::OnPlayerExitClimbState);
	}

	if(ViewCamera)
	{
		DefaultFOV = ViewCamera->FieldOfView;
		CurrentFOV = DefaultFOV;
	}

}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HideCameraIfCharacterClose();
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	checkf(InputConfigDataAsset,TEXT("Forgot to assign a valid data asset as input config"));
	ULocalPlayer* LocalPlayer = GetController<APlayerController>()->GetLocalPlayer();
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer);
	check(Subsystem);
	Subsystem->AddMappingContext(InputConfigDataAsset->DefaultMappingContext,0);
	
	if (UCustomInputComponent* CustomInputComponent = CastChecked<UCustomInputComponent>(PlayerInputComponent))
	{
		//CustomInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainCharacter::Look);
		CustomInputComponent->BindNativeInputAction(InputConfigDataAsset,RPGGameplayTags::Input_Move,ETriggerEvent::Triggered,this,&ThisClass::HandleGroundMovementInput);
		CustomInputComponent->BindNativeInputAction(InputConfigDataAsset,RPGGameplayTags::Input_Look,ETriggerEvent::Triggered,this,&ThisClass::Look);
		
		CustomInputComponent->BindNativeInputAction(InputConfigDataAsset,RPGGameplayTags::Input_Jump,ETriggerEvent::Triggered,this,&ACharacter::Jump);
		CustomInputComponent->BindNativeInputAction(InputConfigDataAsset,RPGGameplayTags::Input_Jump,ETriggerEvent::Completed,this,&ACharacter::StopJumping);

		CustomInputComponent->BindNativeInputAction(InputConfigDataAsset,RPGGameplayTags::Input_LeftClick,ETriggerEvent::Triggered,this,&AMainCharacter::HandleLeftClickInput);
		CustomInputComponent->BindNativeInputAction(InputConfigDataAsset,RPGGameplayTags::Input_RightClick,ETriggerEvent::Triggered,this,&AMainCharacter::HandleRightClickInput);
		
		CustomInputComponent->BindNativeInputAction(InputConfigDataAsset,RPGGameplayTags::Input_ClimbMove,ETriggerEvent::Triggered,this,&ThisClass::HandleClimbMovementInput);
		CustomInputComponent->BindNativeInputAction(InputConfigDataAsset,RPGGameplayTags::Input_ClimbAction,ETriggerEvent::Started,this,&ThisClass::OnClimbActionStarted);
		CustomInputComponent->BindNativeInputAction(InputConfigDataAsset,RPGGameplayTags::Input_ClimbHop,ETriggerEvent::Started,this,&ThisClass::OnClimbHopActionStarted);
		CustomInputComponent->BindNativeInputAction(InputConfigDataAsset,RPGGameplayTags::Input_Grapple,ETriggerEvent::Started,this,&ThisClass::GrappleUse);
		// CustomInputComponent->BindNativeInputAction(InputConfigDataAsset,RPGGameplayTags::Input_TargetLock,ETriggerEvent::Started,this,&ThisClass::StartTargetSystem);
	}
}


void AMainCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	MainPlayerController = Cast<AMainPlayerController>(NewController);
	InputBufferComponent = MainPlayerController->GetInputBufferComponent();
	// InputBufferComponent = MainPlayerController->Execute_GetInputBufferComponent(this);
}

void AMainCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	// if (TargetSystemComponent) TargetSystemComponent->OwnerCharacter = this;
}



void AMainCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookAxisVector.Y);
	AddControllerYawInput(LookAxisVector.X);

}

void AMainCharacter::HandleGroundMovementInput(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();

	const FRotator Rotation = GetController()->GetControlRotation();
	const FRotator YawRotation = FRotator(0.f, Rotation.Yaw, 0.f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardDirection, MovementVector.Y);

	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightDirection, MovementVector.X);
	
	MovementDirectionInput = (ForwardDirection * MovementVector.Y + RightDirection * MovementVector.X).GetSafeNormal();
}

void AMainCharacter::HandleClimbMovementInput(const FInputActionValue& Value)
{
	const FVector2D MovementVector = Value.Get<FVector2D>();
	const FVector ForwardDirection = FVector::CrossProduct(
		-CustomMovementComponent->GetClimbableSurfaceNormal(),
		GetActorRightVector()
	);

	const FVector RightDirection = FVector::CrossProduct(
		-CustomMovementComponent->GetClimbableSurfaceNormal(),
		-GetActorUpVector()
	);

	AddMovementInput(ForwardDirection, MovementVector.Y);
	AddMovementInput(RightDirection, MovementVector.X);
}




void AMainCharacter::AddInputMappingContext(UInputMappingContext* ContextToAdd, int32 InPriority)
{
	if (!ContextToAdd) return;
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(ContextToAdd, InPriority);
		}
	}
}

void AMainCharacter::RemoveInputMappingContext(UInputMappingContext* ContextToAdd)
{
	if (!ContextToAdd) return;
	if (const APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(ContextToAdd);
		}
	}
}


#pragma endregion



void AMainCharacter::HideCameraIfCharacterClose()
{
	if (!IsLocallyControlled()) return;

	if (ViewCamera->IsActive())
	{
		if ((ViewCamera->GetComponentLocation() - GetActorLocation()).Size() < CameraThreshold)
		{
			GetMesh()->SetVisibility(false);
			// if (CombatCompo && CombatCompo->EquippedWeapon)
			// {
			// 	CombatCompo->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = true;
			// }
		}
		else
		{
			GetMesh()->SetVisibility(true);
			// if (CombatCompo && CombatCompo->EquippedWeapon && CombatCompo->EquippedWeapon->GetWeaponMesh())
			// {
			// 	CombatCompo->EquippedWeapon->GetWeaponMesh()->bOwnerNoSee = false;
			// }
		}
	}
}



void AMainCharacter::FinishDeath()
{
	Super::FinishDeath();
	if (MainPlayerController)
	{
		DisableInput(MainPlayerController);
	}
}

bool AMainCharacter::CanJumpInternal_Implementation() const
{
	bool bCanJumpFromBase = GetCharacterMovement() && GetCharacterMovement()->IsJumpAllowed() && GetCharacterMovement()->IsMovingOnGround();
	return bCanJumpFromBase;
}



#pragma region Weapon


#pragma endregion

#pragma region Climb

void AMainCharacter::OnClimbActionStarted(const FInputActionValue& Value)
{
	if (!CustomMovementComponent) return;

	if (!CustomMovementComponent->IsClimbing())
	{
		CustomMovementComponent->ToggleClimbing(true);
	}
	else
	{
		CustomMovementComponent->ToggleClimbing(false);
	}
}


void AMainCharacter::OnClimbHopActionStarted(const FInputActionValue& Value)
{
	if (CustomMovementComponent)
	{
		CustomMovementComponent->RequestHopping();
	}
}


void AMainCharacter::OnPlayerEnterClimbState()
{
	AddInputMappingContext(ClimbMappingContext, 1);
}

void AMainCharacter::OnPlayerExitClimbState()
{
	RemoveInputMappingContext(ClimbMappingContext);
}

void AMainCharacter::HandleLeftClickInput(const FInputActionValue& Value)
{
	if (InputBufferComponent)
	{
		InputBufferComponent->BufferInput(FGameplayTag::RequestGameplayTag("Shared.Input.LeftClick"));
	}
}

void AMainCharacter::HandleRightClickInput(const FInputActionValue& Value)
{
	if (InputBufferComponent)
	{
		InputBufferComponent->BufferInput(FGameplayTag::RequestGameplayTag("Shared.Input.RightClick"));
	}
}


#pragma endregion


void AMainCharacter::ScreenCenterLineTrace(FHitResult& HitResult)
{
	APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	FVector2D ViewportSize;
	GEngine->GameViewport->GetViewportSize(ViewportSize);
	ViewportSize.X /= 2;
	ViewportSize.Y /= 2;

	FVector WorldPosition;
	FVector WorldDirection;

	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(PlayerController, ViewportSize, WorldPosition,
	                                                               WorldDirection);
	
	if (bScreenToWorld)
	{
		FVector Start = WorldPosition;

		float DistanceToCharacter = (GetActorLocation() - Start).Size();
		Start += WorldDirection * (DistanceToCharacter + 100.f);
		FVector End = Start + WorldDirection * 100000.f;

		GetWorld()->LineTraceSingleByChannel(
			HitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);
	}
}



void AMainCharacter::GrappleUse()
{
	if (GrappleComponent->IsUse())
	{
		GrappleComponent->CancelGrapple();
	}
	else
	{
		FHitResult HitResult;
		ScreenCenterLineTrace(HitResult);
		if (HitResult.bBlockingHit)
		{
			FVector LocalOffset{0, 0, 0};
			GrappleComponent->GrappleFire(HitResult.Location, LocalOffset);
		}
	}
}

bool AMainCharacter::IsWeaponEquipped() const
{
	return false;//(CombatCompo && CombatCompo->EquippedWeapon);
}

void AMainCharacter::LearnNewSkill(FGameplayTag SkillTag, TSubclassOf<UBaseAction> ActionClass)
{
	if (!ActionComponent || !ActionClass)
		return;
    
	// 새 액션 등록
	ActionComponent->RegisterAction(SkillTag, ActionClass);
    
	// DataAsset이 있다면 설정
	if (UBaseAction* NewAction = ActionComponent->GetAction(SkillTag))
	{
		// 스킬 레벨에 따른 설정
		// if (USkillDataAsset* SkillData = GetSkillData(SkillTag))
		// {
		// 	ApplySkillData(NewAction, SkillData);
		// }
	}
    
	// UI 업데이트
	// UpdateSkillUI(SkillTag);
}

// void AMyCharacter::OnWeaponChanged(AWeapon* NewWeapon)
// {
// 	if (!NewWeapon || !ActionComponent)
// 		return;
//     
// 	// 기존 무기 액션 제거
// 	ActionComponent->UnregisterAction(CurrentWeaponActionTag);
//     
// 	// 새 무기 액션 추가
// 	CurrentWeaponActionTag = NewWeapon->GetActionTag();
// 	ActionComponent->RegisterAction(CurrentWeaponActionTag, NewWeapon->GetActionClass());
//     
// 	// 무기별 콤보 데이터 설정
// 	if (UMeleeAttackAction* AttackAction = Cast<UMeleeAttackAction>(
// 		ActionComponent->GetAction(CurrentWeaponActionTag)))
// 	{
// 		AttackAction->MeleeData = NewWeapon->GetMeleeData();
// 	}
// }