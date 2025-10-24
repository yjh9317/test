// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/Action/BaseAction.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Header/DebugHeader.h"
#include "Interface/ComponentManager.h"
#include "Shared/Action/Components/ActionComponent.h"

UBaseAction::UBaseAction(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
    // 기본값 설정
    bCanExecuteWhileMoving = true;
    bCanExecuteInAir = false;
    bInterruptible = false;
    CooldownDuration = 0.0f;
}

void UBaseAction::Initialize(AActor* InOwner)
{
    if (!InOwner)  {  return; }

    OwnerCharacter = Cast<ACharacter>(InOwner);

    if(OwnerCharacter)
    {
        IComponentManager* ComponentManager = Cast<IComponentManager>(OwnerCharacter);
    
        if(ComponentManager)
        {
            CachedActionComponent = ComponentManager->GetActionComponent();
        }
    }

    OnInitialized();
}

void UBaseAction::Execute()
{
    if (!CanExecute())
        return;

    bIsActive = true;
    TimeActive = 0.0f;
    LastExecutionTime = GetWorld()->GetTimeSeconds();

    ConsumeResources();
    ApplyGrantedTags();

    OnExecute();
}

void UBaseAction::Tick(float DeltaTime)
{
    if (!bIsActive)
        return;

    TimeActive += DeltaTime;
    OnTick(DeltaTime);
}

void UBaseAction::Interrupt()
{
    if (!bIsActive)
        return;

    if (!bInterruptible)
    {
        UE_LOG(LogTemp, Warning, TEXT("Action %s is not interruptible"), *ActionTag.ToString());
        return;
    }

    bIsActive = false;
    
    // 리소스 일부 환불 (50%)
    // RefundResources(0.5f);

    // 태그 제거
    RemoveGrantedTags();

    OnInterrupt();
    // OnActionFinished.Broadcast();
}

void UBaseAction::Complete()
{
    if (!bIsActive)
        return;

    bIsActive = false;

    RemoveGrantedTags();

    OnComplete();
    // OnActionFinished.Broadcast();
}

bool UBaseAction::CanExecute() const
{
    if (bIsActive)
    {
        // DebugHeader::LogWarning(TEXT("UBaseAction::CanExecute() : Active"));
        return false;
    }

    if (!HasRequiredResources())
    {
        // DebugHeader::LogWarning(TEXT("UBaseAction::CanExecute() : Resource"));
        return false;
    }

    // 이동 중 실행 체크
    if (!bCanExecuteWhileMoving && OwnerCharacter && OwnerCharacter->GetVelocity().Size() > 0.1f)
    {
        // DebugHeader::LogWarning(TEXT("UBaseAction::CanExecute() : Moving"));
        return false;
    }

    // 공중 실행 체크
    if (!bCanExecuteInAir && OwnerCharacter && OwnerCharacter->GetCharacterMovement()->IsFalling())
    {
        // DebugHeader::LogWarning(TEXT("UBaseAction::CanExecute() : Air"));
        return false;
    }

    return true;
}

bool UBaseAction::HasRequiredResources() const
{
    // TODO: 속성 시스템과 연동하여 리소스 체크
    // 현재는 단순 구현
    for (const FActionCost& Cost : ResourceCosts)
    {
        // 실제 구현에서는 AttributeComponent를 통해 체크
        // float CurrentValue = GetAttributeValue(Cost.CostAttributeTag);
        // if (CurrentValue < Cost.CostValue)
        //     return false;
    }
    
    return true;
}

float UBaseAction::GetCooldownRemaining() const
{
    if (CooldownDuration <= 0.0f)
        return 0.0f;

    float CurrentTime = GetWorld()->GetTimeSeconds();
    float ElapsedTime = CurrentTime - LastExecutionTime;
    
    return FMath::Max(0.0f, CooldownDuration - ElapsedTime);
}


void UBaseAction::ConsumeResources()
{
    // TODO: 속성 시스템과 연동
    for (const FActionCost& Cost : ResourceCosts)
    {
        // GetAttributeComponent()->ModifyAttribute(Cost.CostAttributeTag, -Cost.CostValue);
        UE_LOG(LogTemp, Verbose, TEXT("Consuming %f of %s"), 
               Cost.CostValue, *Cost.CostAttributeTag.ToString());
    }
}

void UBaseAction::RefundResources(float Percentage)
{
    // TODO: 속성 시스템과 연동
    for (const FActionCost& Cost : ResourceCosts)
    {
        float RefundAmount = Cost.CostValue * Percentage;
        // GetAttributeComponent()->ModifyAttribute(Cost.CostAttributeTag, RefundAmount);
        UE_LOG(LogTemp, Verbose, TEXT("Refunding %f of %s"), 
               RefundAmount, *Cost.CostAttributeTag.ToString());
    }
}

void UBaseAction::ApplyGrantedTags()
{
    // if (OwnerStateComponent && GrantedTags.Num() > 0)
    // {
    //     OwnerStateComponent->AddTags(GrantedTags);
    //     UE_LOG(LogTemp, Verbose, TEXT("Applied granted tags for action %s"), *ActionTag.ToString());
    // }
}

void UBaseAction::RemoveGrantedTags()
{
    // if (OwnerStateComponent && GrantedTags.Num() > 0)
    // {
    //     OwnerStateComponent->RemoveTags(GrantedTags);
    //     UE_LOG(LogTemp, Verbose, TEXT("Removed granted tags for action %s"), *ActionTag.ToString());
    // }
}

void UBaseAction::StartCooldown()
{
    
}

APlayerController* UBaseAction::GetOwnerController() const
{
    if (OwnerCharacter)
    {
        return Cast<APlayerController>(OwnerCharacter->GetController());
    }
    return nullptr;
}
