#include "Shared/Action/Actions/Combat/MeleeAttackAction.h"
#include "Animation/AnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Interface/ComponentManager.h"
#include "Player/Components/InputBufferComponent.h"
#include "Shared/Action/Data/ActionData.h"

UMeleeAttackAction::UMeleeAttackAction(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
    ActionName = FText::FromString(TEXT("Melee Attack"));
    
    bCanExecuteWhileMoving = false;
    bCanExecuteInAir = false;
    bInterruptible = true;
    bCanInterruptOthers = false;
    bMovementCancelable = true;
    
    BaseStaminaCost = 10.0f;
    CurrentComboIndex = 0;
    
    // 태그 설정 예시
    // GrantedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Attacking"));
    // BlockedTags.AddTag(FGameplayTag::RequestGameplayTag("State.Stunned"));
}

void UMeleeAttackAction::Initialize(AActor* InOwner)
{
    Super::Initialize(InOwner);

    if(OwnerCharacter && OwnerCharacter->GetController())
    {
        IComponentManager* ComponentManager = Cast<IComponentManager>(OwnerCharacter->GetController());
        if(ComponentManager)
        {
            CachedInputBuffer = ComponentManager->GetInputBufferComponent();
        }
    }
}

bool UMeleeAttackAction::CanExecute() const
{
    if(!CachedInputBuffer)
    {
        return false;
    }

    // 콤보 중이면 윈도우 체크
    if (bIsActive && bComboWindowOpen) 
    {
        return CurrentComboIndex < MeleeData.ComboMontages.Num() - 1;
    }
    
    return !bIsActive && Super::CanExecute();
}

void UMeleeAttackAction::OnExecute()
{
    if (!ValidateExecution())
    {
        OnComplete();
        return;
    }

    LastAttackTime = GetWorld()->GetTimeSeconds();
    PerformAttack();
}

bool UMeleeAttackAction::TryProcessComboInput()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    
    if (!bComboWindowOpen || CurrentComboIndex >= MeleeData.ComboMontages.Num() - 1)
        return false;
    
    LastAttackTime = CurrentTime;
    CurrentComboIndex++;
    PerformAttack();
    return true;
}


void UMeleeAttackAction::OnComboWindowClosed()
{
    if (CachedInputBuffer && 
        CachedInputBuffer->HasBufferedAction(ActionTag))
    {
        CurrentComboIndex++;
        LastAttackTime = GetWorld()->GetTimeSeconds();
        PerformAttack();
        CachedInputBuffer->ConsumeBufferedInput(ActionTag);
    }
}

void UMeleeAttackAction::CancelToMovement(const FVector& Direction)
{
    if (!bMovementCancelable)
        return;
    
    // 부드러운 전환
    if (UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance())
    {
        AnimInstance->Montage_Stop(BlendOutTime,CurrentAnimMontage);
    }
    
    // 이동 시작
    if (UCharacterMovementComponent* Movement = OwnerCharacter->GetCharacterMovement())
    {
        Movement->SetMovementMode(MOVE_Walking);
        OwnerCharacter->AddMovementInput(Direction);
    }
    
    // 액션 종료
   Interrupt();
}

void UMeleeAttackAction::OnAttackMontageEnded()
{
    if (!CachedInputBuffer || !CachedInputBuffer->HasBufferedAction(ActionTag))
    {
        ResetCombo();
        OnComplete();
    }
}

void UMeleeAttackAction::PerformAttack()
{
    float StaminaCost = GetStaminaCostForCombo();
    
    if (!PlayAttackMontage())
    {
        OnComplete();
        return;
    }
    CurrentAnimMontage = MeleeData.ComboMontages[CurrentComboIndex];
    AlreadyHitActors.Empty();
    bComboWindowOpen = false;
    bMovementCancelable = false;
}

bool UMeleeAttackAction::PlayAttackMontage()
{
    if (!OwnerCharacter || MeleeData.ComboMontages.Num() == 0)
        return false;
    
    int32 MontageIndex = FMath::Clamp(CurrentComboIndex, 0, MeleeData.ComboMontages.Num() - 1);
    UAnimMontage* AttackMontage = MeleeData.ComboMontages[MontageIndex];
    
    if (!AttackMontage)
        return false;
    
    UAnimInstance* AnimInstance = OwnerCharacter->GetMesh()->GetAnimInstance();
    if (!AnimInstance)
        return false;
    
    float PlayResult = AnimInstance->Montage_Play(AttackMontage, 1.0f);
    return PlayResult > 0.0f;
}

void UMeleeAttackAction::ResetCombo()
{
    CurrentComboIndex = 0;
    bComboWindowOpen = false;
    bMovementCancelable = false;
    CurrentAnimMontage = nullptr;
    
    AlreadyHitActors.Empty();
}

void UMeleeAttackAction::OnComplete()
{
    if (bComboWindowOpen && CurrentComboIndex < MeleeData.ComboMontages.Num() - 1)
    {
        return;
    }
    
    ResetCombo();
}

void UMeleeAttackAction::OnInterrupt()
{
    ResetCombo();
    CancelToMovement({});
}

bool UMeleeAttackAction::ValidateExecution() const
{
    return OwnerCharacter && 
           OwnerCharacter->GetMesh()->GetAnimInstance() && 
           MeleeData.IsValid();
}

float UMeleeAttackAction::GetStaminaCostForCombo() const
{
    float ComboMultiplier = 1.0f + (CurrentComboIndex * 0.2f);
    return BaseStaminaCost * ComboMultiplier;
}

void UMeleeAttackAction::HandleActionEvent(EActionEvent EventType, float Value)
{
    switch (EventType)
    {
        case EActionEvent::ActionComplete:
            bComboWindowOpen = false;
            OnComplete();
        break;
    }
}

bool UMeleeAttackAction::CanHandleActionEvent(EActionEvent EventType) const
{
    return true;
}
