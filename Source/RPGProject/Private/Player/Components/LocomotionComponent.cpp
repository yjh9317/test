// LocomotionComponent.cpp
#include "Player/Components/LocomotionComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Animation/AnimInstance.h"
#include "Net/UnrealNetwork.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "KismetAnimationLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Header/DebugHeader.h"

ULocomotionComponent::ULocomotionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    SetIsReplicatedByDefault(true);
}

void ULocomotionComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // Cache references
    if (GetOwner())
    {
        OwnerCharacter = Cast<ACharacter>(GetOwner());
        if (OwnerCharacter)
        {
            OwnerCharacterMovement = OwnerCharacter->GetCharacterMovement();
            if (OwnerCharacter->GetMesh())
            {
                MainAnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
            }
        }
        
        check(OwnerCharacter);
        check(OwnerCharacterMovement);
    }
    
    // Initialize after a short delay
    GetWorld()->GetTimerManager().SetTimer(
        BeginPlayTimerHandle,
        [this]()
        {
            SetRotationMode(RotationMode);
            
            // StandAlone에서 초기 이벤트 발생
            if (GetNetMode() == NM_Standalone)
            {
                OnRep_MovementType();
                OnRep_AnimsetName();
                OnRep_OverrideName();
            }
        }, 
        0.1f, 
        false
    );
}

void ULocomotionComponent::TickComponent(float DeltaTime, ELevelTick TickType, 
                                         FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    // Only tick on locally controlled pawns
    if (OwnerCharacter && OwnerCharacter->IsLocallyControlled())
    {
        CheckAcceleration();
        UpdateSprintingValue();
    }
}

void ULocomotionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    
    DOREPLIFETIME(ULocomotionComponent, MovementType);
    DOREPLIFETIME(ULocomotionComponent, RotationMode);
    DOREPLIFETIME(ULocomotionComponent, OverrideName);
    DOREPLIFETIME(ULocomotionComponent, AnimsetName);
    DOREPLIFETIME(ULocomotionComponent, AnimsetDataTable);
    DOREPLIFETIME(ULocomotionComponent, HasAcceleration);
    DOREPLIFETIME(ULocomotionComponent, bIsSprinting);
    DOREPLIFETIME(ULocomotionComponent, DisableRootYawOffset);
    DOREPLIFETIME(ULocomotionComponent, GlobalExtendedPose);
}

// ===========================
// Core Set Functions - StandAlone 지원
// ===========================

void ULocomotionComponent::SetMovementType(EPlayerMovementMode InMovementType)
{
    if (MovementType == InMovementType)
    {
        return;
    }
    
    const ENetMode NetMode = GetNetMode();
    
    // StandAlone 또는 Server
    if (NetMode == NM_Standalone || NetMode == NM_ListenServer || NetMode == NM_DedicatedServer)
    {
        MovementType = InMovementType;
        
        // StandAlone과 Server에서 이벤트 직접 발생
        OnRep_MovementType();
        SetMovementConfig();
    }
    // Client
    else if (NetMode == NM_Client)
    {
        Server_SetMovementType(InMovementType);
    }
}

void ULocomotionComponent::Server_SetMovementType_Implementation(EPlayerMovementMode InMovementType)
{
    MovementType = InMovementType;
    // Server에서도 OnRep 호출
    OnRep_MovementType();
    SetMovementConfig();
}

void ULocomotionComponent::SetRotationMode(ERotationMode InRotationMode)
{
    if (RotationMode == InRotationMode)
    {
        return;
    }
    
    const ENetMode NetMode = GetNetMode();
    
    if (NetMode == NM_Standalone || NetMode == NM_ListenServer || NetMode == NM_DedicatedServer)
    {
        RotationMode = InRotationMode;
        
        // 직접 설정
        if (OwnerCharacterMovement)
        {
            OwnerCharacterMovement->bOrientRotationToMovement = (RotationMode == ERotationMode::ForwardFacing);
        }
        
        // StandAlone과 Server에서 이벤트 발생
        OnRep_RotationMode();
        SetMovementConfig();
    }
    else if (NetMode == NM_Client)
    {
        Server_SetRotationMode(InRotationMode);
    }
}

void ULocomotionComponent::Server_SetRotationMode_Implementation(ERotationMode InRotationMode)
{
    RotationMode = InRotationMode;
    
    if (OwnerCharacterMovement)
    {
        OwnerCharacterMovement->bOrientRotationToMovement = (RotationMode == ERotationMode::ForwardFacing);
    }
    
    OnRep_RotationMode();
    SetMovementConfig();
}

void ULocomotionComponent::SetAnimSet(const FName& InAnimSetName)
{
    if (AnimsetName == InAnimSetName)
    {
        return;
    }
    
    const ENetMode NetMode = GetNetMode();
    
    if (NetMode == NM_Standalone || NetMode == NM_ListenServer || NetMode == NM_DedicatedServer)
    {
        AnimsetName = InAnimSetName;
        OnRep_AnimsetName();
        SetMovementConfig();
    }
    else if (NetMode == NM_Client)
    {
        Server_SetAnimset(InAnimSetName);
    }
}

void ULocomotionComponent::Server_SetAnimset_Implementation(const FName& InAnimsetName)
{
    AnimsetName = InAnimsetName;
    OnRep_AnimsetName();
    SetMovementConfig();
}

void ULocomotionComponent::SetOverrideName(const FName& InOverrideName)
{
    if (OverrideName == InOverrideName)
    {
        return;
    }
    
    const ENetMode NetMode = GetNetMode();
    
    if (NetMode == NM_Standalone || NetMode == NM_ListenServer || NetMode == NM_DedicatedServer)
    {
        OverrideName = InOverrideName;
        OnRep_OverrideName();
        SetMovementConfig();
    }
    else if (NetMode == NM_Client)
    {
        Server_SetOverrideName(InOverrideName);
    }
}

void ULocomotionComponent::Server_SetOverrideName_Implementation(const FName& InOverrideName)
{
    OverrideName = InOverrideName;
    OnRep_OverrideName();
    SetMovementConfig();
}

void ULocomotionComponent::SetAnimsetDataTable(UDataTable* InDataTable)
{
    if (AnimsetDataTable == InDataTable)
    {
        return;
    }
    
    const ENetMode NetMode = GetNetMode();
    
    if (NetMode == NM_Standalone || NetMode == NM_ListenServer || NetMode == NM_DedicatedServer)
    {
        AnimsetDataTable = InDataTable;
        OnRep_AnimsetDataTable();
        SetMovementConfig();
    }
    else if (NetMode == NM_Client)
    {
        Server_SetAnimsetTable(InDataTable);
    }
}

void ULocomotionComponent::Server_SetAnimsetTable_Implementation(UDataTable* InAnimsetDataTable)
{
    AnimsetDataTable = InAnimsetDataTable;
    OnRep_AnimsetDataTable();
    SetMovementConfig();
}

void ULocomotionComponent::SetGlobalExtendedPose(EExtendedPose InExtendedPose)
{
    if (GlobalExtendedPose == InExtendedPose)
    {
        return;
    }
    
    const ENetMode NetMode = GetNetMode();
    
    if (NetMode == NM_Standalone || NetMode == NM_ListenServer || NetMode == NM_DedicatedServer)
    {
        GlobalExtendedPose = InExtendedPose;
        OnRep_GlobalExtendedPose();
    }
    else if (NetMode == NM_Client)
    {
        Server_SetGlobalExtendedPose(InExtendedPose);
    }
}

void ULocomotionComponent::Server_SetGlobalExtendedPose_Implementation(EExtendedPose InGlobalExtendedPose)
{
    GlobalExtendedPose = InGlobalExtendedPose;
    OnRep_GlobalExtendedPose();
}

// ===========================
// Movement Configuration - 간소화
// ===========================

void ULocomotionComponent::SetMovementConfig()
{
    float LocalMaxSpeed = 0.f;
    float LocalMaxAcceleration = 0.f;
    float LocalBrakingDecelerationWalking = 0.f;
    FRotator LocalRotationRate = FRotator::ZeroRotator;
    
    GetCurrentMovementConfig(LocalMaxSpeed, LocalMaxAcceleration, 
                            LocalBrakingDecelerationWalking, LocalRotationRate);
    
    // 모든 환경에서 직접 적용 (Server RPC 제거)
    ApplyMovementSettings(LocalMaxSpeed, LocalMaxAcceleration, 
                         LocalBrakingDecelerationWalking, LocalRotationRate);
}

void ULocomotionComponent::ApplyMovementSettings(float MaxSpeed, float MaxAcceleration, 
                                                 float BrakingDeceleration, FRotator RotationRate)
{
    if (!OwnerCharacterMovement)
    {
        return;
    }
    
    OwnerCharacterMovement->MaxWalkSpeed = MaxSpeed;
    OwnerCharacterMovement->MaxWalkSpeedCrouched = MaxSpeed;
    OwnerCharacterMovement->MaxFlySpeed = MaxSpeed;
    OwnerCharacterMovement->MaxAcceleration = MaxAcceleration;
    OwnerCharacterMovement->BrakingDecelerationWalking = BrakingDeceleration;
    OwnerCharacterMovement->RotationRate = RotationRate;
}

// ===========================
// Sprint System
// ===========================

void ULocomotionComponent::SetSprint(bool IsSprint)
{
    EnableSprint = IsSprint;
    
    // Sprint 비활성화 시 즉시 중단
    if (!EnableSprint && bIsSprinting)
    {
        const ENetMode NetMode = GetNetMode();
        
        if (NetMode == NM_Standalone || NetMode == NM_ListenServer || NetMode == NM_DedicatedServer)
        {
            bIsSprinting = false;
            OnRep_IsSprinting();
            SetMovementConfig();
        }
        else if (NetMode == NM_Client)
        {
            Server_SetSprintValue(false);
        }
    }
}

void ULocomotionComponent::UpdateSprintingValue()
{
    if (EnableSprint)
    {
        bool NewSprintState = GetIsSprint();
        if (bIsSprinting != NewSprintState)
        {
            const ENetMode NetMode = GetNetMode();
            
            if (NetMode == NM_Standalone || NetMode == NM_ListenServer || NetMode == NM_DedicatedServer)
            {
                bIsSprinting = NewSprintState;
                OnRep_IsSprinting();
                SetMovementConfig();
            }
            else if (NetMode == NM_Client)
            {
                Server_SetSprintValue(NewSprintState);
            }
        }
    }
    else if (bIsSprinting)
    {
        const ENetMode NetMode = GetNetMode();
        
        if (NetMode == NM_Standalone || NetMode == NM_ListenServer || NetMode == NM_DedicatedServer)
        {
            bIsSprinting = false;
            OnRep_IsSprinting();
            SetMovementConfig();
        }
        else if (NetMode == NM_Client)
        {
            Server_SetSprintValue(false);
        }
    }
}

void ULocomotionComponent::Server_SetSprintValue_Implementation(bool InIsSprinting)
{
    bIsSprinting = InIsSprinting;
    OnRep_IsSprinting();
    SetMovementConfig();
}

// ===========================
// Acceleration
// ===========================

void ULocomotionComponent::CheckAcceleration()
{
    bool LocalIsAccelerating = GetIsAccelerating();
    
    if (HasAcceleration != LocalIsAccelerating)
    {
        const ENetMode NetMode = GetNetMode();
        
        if (NetMode == NM_Standalone || NetMode == NM_ListenServer || NetMode == NM_DedicatedServer)
        {
            HasAcceleration = LocalIsAccelerating;
            LastVelocityDirection = GetLastVelocityDirection();
            OnRep_HasAcceleration();
        }
        else if (NetMode == NM_Client)
        {
            Server_SetAccelerationValue(LocalIsAccelerating, GetLastVelocityDirection());
        }
    }
}

void ULocomotionComponent::Server_SetAccelerationValue_Implementation(bool InHasAcceleration, float InLastVelocityDirection)
{
    HasAcceleration = InHasAcceleration;
    LastVelocityDirection = InLastVelocityDirection;
    OnRep_HasAcceleration();
}

// ===========================
// OnRep Functions - 모든 환경에서 델리게이트 호출
// ===========================

void ULocomotionComponent::OnRep_MovementType()
{
    // UE_LOG(LogTemp, Log, TEXT("OnRep_MovementType called - NetMode: %d, MovementType: %d"),(int32)GetNetMode(), (int32)MovementType);
    
    OnSetMovementTypeDelegate.Broadcast(MovementType);
    SetOverrideExtendPose();
}

void ULocomotionComponent::OnRep_OverrideName()
{
    // UE_LOG(LogTemp, Log, TEXT("OnRep_OverrideName called - NetMode: %d, OverrideName: %s"),(int32)GetNetMode(), *OverrideName.ToString());
    
    OnSetOverrideNameDelegate.Broadcast(OverrideName);
    SetOverrideExtendPose();
}

void ULocomotionComponent::OnRep_RotationMode()
{
    // UE_LOG(LogTemp, Log, TEXT("OnRep_RotationMode called - NetMode: %d, RotationMode: %d"), (int32)GetNetMode(), (int32)RotationMode);
    
    OnSetRotationModeDelegate.Broadcast(RotationMode);
}

void ULocomotionComponent::OnRep_AnimsetName()
{
    // UE_LOG(LogTemp, Log, TEXT("OnRep_AnimsetName called - NetMode: %d, AnimsetName: %s"),(int32)GetNetMode(), *AnimsetName.ToString());
    
    OnSetAnimsDelegate.Broadcast(AnimsetName);
    SetOverrideExtendPose();
}

void ULocomotionComponent::OnRep_IsSprinting()
{
    // UE_LOG(LogTemp, Log, TEXT("OnRep_IsSprinting called - NetMode: %d, IsSprinting: %s"), (int32)GetNetMode(), bIsSprinting ? TEXT("True") : TEXT("False"));
    
    OnSprintChangedDelegate.Broadcast(bIsSprinting);
}

void ULocomotionComponent::OnRep_AnimsetDataTable()
{
    // UE_LOG(LogTemp, Log, TEXT("OnRep_AnimsetDataTable called - NetMode: %d"), (int32)GetNetMode());
    
    OnSetAnimsDelegate.Broadcast(AnimsetName);
    SetOverrideExtendPose();
}

void ULocomotionComponent::OnRep_HasAcceleration()
{
    // UE_LOG(LogTemp, Log, TEXT("OnRep_HasAcceleration called - NetMode: %d, HasAcceleration: %s"), (int32)GetNetMode(), HasAcceleration ? TEXT("True") : TEXT("False"));
    
    OnAccelerationChangedDelegate.Broadcast(HasAcceleration, LastVelocityDirection);
}

void ULocomotionComponent::OnRep_DisableRootYawOffset()
{
    // UE_LOG(LogTemp, Log, TEXT("OnRep_DisableRootYawOffset called - NetMode: %d"), (int32)GetNetMode());
    
    OnRootYawOffsetStatusDelegate.Broadcast(DisableRootYawOffset);
}

void ULocomotionComponent::OnRep_GlobalExtendedPose()
{
    // UE_LOG(LogTemp, Log, TEXT("OnRep_GlobalExtendedPose called - NetMode: %d"), (int32)GetNetMode());
    
    OnSetGlobalExtendedPoseDelegate.Broadcast(GlobalExtendedPose);
}

// ===========================
// Getter Functions (변경 없음)
// ===========================

float ULocomotionComponent::GetSprintSpeed() const
{
    return SprintSpeed;
}

FAnimSetting ULocomotionComponent::GetAnimSet(const FName& RowName) const
{
    if (nullptr == AnimsetDataTable)
    {
        DebugHeader::LogWarning(TEXT("ULocomotionComponent::GetAnimSet : AnimSetDataTable is nullptr"));
        return FAnimSetting(); 
    }
    
    const FAnimSetting* FoundRow = AnimsetDataTable->FindRow<FAnimSetting>(RowName, "");
    if (FoundRow)
    {
        return *FoundRow;
    }

    return FAnimSetting();
}

FLocomotionMovement ULocomotionComponent::GetCurrentMovementStruct() const
{
    FAnimSetting Setting = GetAnimSet(AnimsetName);
    const auto& Map = Setting.Movement;

    if (Map.Find(MovementType))
    {
        return Map[MovementType];
    }
    return FLocomotionMovement();
}

FLocomotionOverride ULocomotionComponent::GetCurrentOverrideStruct() const
{
    FLocomotionMovement CurrentMovement = GetCurrentMovementStruct();
    const auto& Map = CurrentMovement.Override;

    if (Map.Find(OverrideName))
    {
        return Map[OverrideName];
    }

    FName DefaultName{"Default"};
    if (Map.Find(DefaultName))
    {
        return Map[DefaultName];
    }
    
    DebugHeader::LogWarning(TEXT("GetCurrentOverrideStruct : return Empty Struct"));
    return FLocomotionOverride();
}

void ULocomotionComponent::GetCurrentMovementConfig(float& OutMaxSpeed, float& OutMaxAcceleration,
    float& OutBrakingDecelerationWalking, FRotator& OutRotationRate)
{
    FLocomotion_Config FinalConfig{};
    
    if (bIsSprinting)
    {
        FAnimSetting AnimSetting = GetAnimSet(AnimsetName);
        FinalConfig = AnimSetting.General.Sprint.Config;
    }
    else
    {
        const FLocomotionOverride CurrentOverride = GetCurrentOverrideStruct();
        const FLocomotionOverride_Config& Override_Config = CurrentOverride.Config;
        
        if (Override_Config.bEnable)
        {
            FinalConfig = Override_Config.Config;
        }
        else
        {
            FinalConfig = GetCurrentMovementStruct().Config;
        }
    }

    OutMaxSpeed = FinalConfig.MaxSpeed;
    OutMaxAcceleration = FinalConfig.MaxAcceleration;
    OutBrakingDecelerationWalking = FinalConfig.BrakingDecelerationWalking;
    OutRotationRate = FinalConfig.RotationRate;
}

ERotationMode ULocomotionComponent::GetRotationMode() const
{
    return RotationMode;
}

EPlayerMovementMode ULocomotionComponent::GetMovementMode() const
{
    return MovementType;
}

bool ULocomotionComponent::GetIsAccelerating() const
{
    return OwnerCharacterMovement && OwnerCharacterMovement->GetCurrentAcceleration().Size2D() > 0;
}

float ULocomotionComponent::GetLastVelocityDirection() const
{
    if (!MainAnimInstance || !OwnerCharacterMovement || !OwnerCharacter) 
    {
        return 0.f;
    }

    const FVector LastInputVector = OwnerCharacterMovement->GetLastInputVector();
    const FRotator CharacterRotator = OwnerCharacter->GetActorRotation();
    
    return UKismetAnimationLibrary::CalculateDirection(LastInputVector, CharacterRotator);
}

bool ULocomotionComponent::GetIsSprint() const
{
    const float LocalLastVelocityDirection = GetLastVelocityDirection();
    
    if (GetRotationMode() == ERotationMode::ForwardFacing)
    {
        return true;
    }
    
    return UKismetMathLibrary::InRange_FloatFloat(LocalLastVelocityDirection, -50.f, 50.f, true, true);	
}

ACharacter* ULocomotionComponent::GetCharacter() const
{
    return OwnerCharacter;
}

bool ULocomotionComponent::HasMount() const
{
    return IsValid(MountActor);
}

EExtendedPose ULocomotionComponent::GetExtendedPose() const
{
    if (GlobalExtendedPose != EExtendedPose::None)
    {
        return GlobalExtendedPose;
    }

    return OverrideExtendPose;
}

bool ULocomotionComponent::IsRotationMode(ERotationMode InRotationMode) const
{
    return RotationMode == InRotationMode;
}

bool ULocomotionComponent::IsMovementType(EPlayerMovementMode InMovementMode)
{
    return MovementType == InMovementMode;
}

// ===========================
// Mount/Rider System
// ===========================

void ULocomotionComponent::SetRiderStatus(bool IsRider, AActor* InMountActor)
{
    bIsRider = IsRider;
    MountActor = InMountActor;
    
    OnRiderStatusChangedDelegate.Broadcast(IsRider, InMountActor);
}

void ULocomotionComponent::SetMount(AActor* InMountActor)
{
    MountActor = InMountActor;
    OnSetMountDelegate.Broadcast(InMountActor);
    
}

void ULocomotionComponent::ResetMountState()
{
    bIsMountControlRotationSet = false;
}

// ===========================
// Other Settings
// ===========================

void ULocomotionComponent::SetRootYawOffsetStatus(bool Disable)
{
    if (DisableRootYawOffset != Disable)
    {
        DisableRootYawOffset = Disable;
        
        const ENetMode NetMode = GetNetMode();
        if (NetMode == NM_Standalone || NetMode == NM_ListenServer || NetMode == NM_DedicatedServer)
        {
            OnRep_DisableRootYawOffset();
        }
    }
}

void ULocomotionComponent::SetOverrideExtendPose()
{
    FLocomotionOverride LocomotionOverride = GetCurrentOverrideStruct();
    FLocomotionOverride_ExtendedPose LocalOverrideExtendedPose = LocomotionOverride.ExtendedPose;
    FName ExtendedPoseName = LocalOverrideExtendedPose.ExtendedPoseName;
    
    const UEnum* EnumPtr = StaticEnum<EExtendedPose>();
    if (!EnumPtr)
    {
        return;
    }

    for (int32 i = 0; i < EnumPtr->NumEnums() - 1; ++i)
    {
        FName Name = EnumPtr->GetNameByIndex(i);
        if (Name == ExtendedPoseName)
        {
            EExtendedPose CurrentValue = static_cast<EExtendedPose>(EnumPtr->GetValueByIndex(i));
            OverrideExtendPose = CurrentValue;
            break;
        }
    }
}