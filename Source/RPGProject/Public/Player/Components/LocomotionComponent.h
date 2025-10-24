// LocomotionComponent.h
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/Data/LocomotionEnum.h"
#include "Player/Data/LocomotionStructs.h"
#include "LocomotionComponent.generated.h"

// Delegates
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSetMovementType, EPlayerMovementMode);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSprintChanged, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSetRotationMode, ERotationMode);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSetOverrideName, const FName&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnAccelerationChanged, bool, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSetAnimSet, const FName&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRiderStatusChanged, bool, AActor*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRootYawOffsetStatus, bool);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSetMount, AActor*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSetGlobalExtendedPose, EExtendedPose);

class UCharacterMovementComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RPGPROJECT_API ULocomotionComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    ULocomotionComponent();

    // Delegates
    FOnSetMovementType OnSetMovementTypeDelegate;
    FOnSprintChanged OnSprintChangedDelegate;
    FOnSetRotationMode OnSetRotationModeDelegate;
    FOnSetOverrideName OnSetOverrideNameDelegate;
    FOnAccelerationChanged OnAccelerationChangedDelegate;
    FOnSetAnimSet OnSetAnimsDelegate;
    FOnRiderStatusChanged OnRiderStatusChangedDelegate;
    FOnRootYawOffsetStatus OnRootYawOffsetStatusDelegate;
    FOnSetMount OnSetMountDelegate;
    FOnSetGlobalExtendedPose OnSetGlobalExtendedPoseDelegate;

    // Public Functions
    UFUNCTION(BlueprintCallable, Category="Locomotion")
    void SetMovementType(EPlayerMovementMode InMovementType);

    UFUNCTION(BlueprintCallable, Category="Locomotion")
    void SetAnimSet(const FName& InAnimSetName);

    UFUNCTION(BlueprintCallable, Category="Locomotion")
    void SetSprint(bool IsSprint);

    UFUNCTION(BlueprintCallable, Category="Locomotion")
    void SetRotationMode(ERotationMode InRotationMode);

    UFUNCTION(BlueprintCallable, Category="Locomotion")
    void SetOverrideName(const FName& InOverrideName);

    UFUNCTION(BlueprintCallable, Category="Locomotion")
    void SetAnimsetDataTable(UDataTable* InDataTable);

    UFUNCTION(BlueprintCallable, Category="Locomotion")
    void SetGlobalExtendedPose(EExtendedPose InExtendedPose);

    // Getters
    FLocomotionMovement GetCurrentMovementStruct() const;
    FLocomotionOverride GetCurrentOverrideStruct() const;
    void GetCurrentMovementConfig(float& OutMaxSpeed, float& OutMaxAcceleration, 
                                  float& OutBrakingDecelerationWalking, FRotator& OutRotationRate);
    
    float GetSprintSpeed() const;
    FAnimSetting GetAnimSet(const FName& RowName) const;
    ERotationMode GetRotationMode() const;
    EPlayerMovementMode GetMovementMode() const;
    bool GetIsAccelerating() const;
    float GetLastVelocityDirection() const;
    bool GetIsSprint() const;
    ACharacter* GetCharacter() const;
    bool HasMount() const;
    EExtendedPose GetExtendedPose() const;
    bool IsRotationMode(ERotationMode InRotationMode) const;
    bool IsMovementType(EPlayerMovementMode InMovementMode);

    // Settings
    void SetMovementConfig();
    void SetRiderStatus(bool IsRider, AActor* InMountActor);
    void SetMount(AActor* InMountActor);
    void SetRootYawOffsetStatus(bool Disable);
    void SetOverrideExtendPose();

    UFUNCTION(BlueprintCallable, Category = "Locomotion|Mount")
    void ResetMountState();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, 
                               FActorComponentTickFunction* ThisTickFunction) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    // Internal functions
    void CheckAcceleration();
    void UpdateSprintingValue();
    void ApplyMovementSettings(float MaxSpeed, float MaxAcceleration, 
                               float BrakingDeceleration, FRotator RotationRate);
    
    // OnRep functions - CRITICAL: StandAlone에서 수동 호출 필요
    UFUNCTION()
    void OnRep_MovementType();
    
    UFUNCTION()
    void OnRep_OverrideName();
    
    UFUNCTION()
    void OnRep_RotationMode();
    
    UFUNCTION()
    void OnRep_AnimsetName();
    
    UFUNCTION()
    void OnRep_IsSprinting();
    
    UFUNCTION()
    void OnRep_AnimsetDataTable();
    
    UFUNCTION()
    void OnRep_HasAcceleration();
    
    UFUNCTION()
    void OnRep_DisableRootYawOffset();
    
    UFUNCTION()
    void OnRep_GlobalExtendedPose();

    // Simplified RPC Functions - 필요한 것만 남김
    UFUNCTION(Server, Reliable)
    void Server_SetMovementType(EPlayerMovementMode InMovementType);
    
    UFUNCTION(Server, Reliable)
    void Server_SetRotationMode(ERotationMode InRotationMode);
    
    UFUNCTION(Server, Reliable)
    void Server_SetAnimset(const FName& InAnimsetName);
    
    UFUNCTION(Server, Reliable)
    void Server_SetOverrideName(const FName& InOverrideName);
    
    UFUNCTION(Server, Reliable)
    void Server_SetAnimsetTable(UDataTable* InAnimsetDataTable);
    
    UFUNCTION(Server, Reliable)
    void Server_SetSprintValue(bool InIsSprinting);
    
    UFUNCTION(Server, Reliable)
    void Server_SetGlobalExtendedPose(EExtendedPose InGlobalExtendedPose);
    
    UFUNCTION(Server, Reliable)
    void Server_SetAccelerationValue(bool InHasAcceleration, float InLastVelocityDirection);

public:
    // Replicated Properties
    UPROPERTY(ReplicatedUsing=OnRep_MovementType, EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion")
    EPlayerMovementMode MovementType;

    UPROPERTY(ReplicatedUsing=OnRep_RotationMode, EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion")
    ERotationMode RotationMode;

    UPROPERTY(ReplicatedUsing=OnRep_OverrideName, EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion")
    FName OverrideName = FName(TEXT("Default"));

    UPROPERTY(ReplicatedUsing=OnRep_AnimsetName, EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion")
    FName AnimsetName = FName(TEXT("Default"));

    UPROPERTY(ReplicatedUsing=OnRep_AnimsetDataTable, EditDefaultsOnly, BlueprintReadOnly, Category = "Locomotion")
    TObjectPtr<UDataTable> AnimsetDataTable;

    UPROPERTY(ReplicatedUsing=OnRep_HasAcceleration, VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    bool HasAcceleration;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    float LastVelocityDirection;

    UPROPERTY(ReplicatedUsing=OnRep_IsSprinting, VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    bool bIsSprinting;

    UPROPERTY(ReplicatedUsing=OnRep_DisableRootYawOffset, VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    bool DisableRootYawOffset;

    UPROPERTY(ReplicatedUsing=OnRep_GlobalExtendedPose, VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    EExtendedPose GlobalExtendedPose;

    // Non-replicated
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    bool EnableSprint;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
    EExtendedPose OverrideExtendPose;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement")
    float SprintSpeed = 700.f;

    // References
    UPROPERTY()
    TObjectPtr<ACharacter> OwnerCharacter;

    UPROPERTY()
    TObjectPtr<UCharacterMovementComponent> OwnerCharacterMovement;

    UPROPERTY()
    TObjectPtr<UAnimInstance> MainAnimInstance;

    // Mount System
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rider")
    bool bIsRider;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rider")
    TObjectPtr<AActor> MountActor;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Rider", meta=(AllowPrivateAccess))
    FRotator MountControlRotation;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta=(AllowPrivateAccess))
   	FRotator BaseAimRotation;
    
    // Timer
    FTimerHandle BeginPlayTimerHandle;
    bool bIsMountControlRotationSet;
};