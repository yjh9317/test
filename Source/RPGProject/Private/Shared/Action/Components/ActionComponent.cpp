// Fill out your copyright notice in the Description page of Project Settings.


#include "Shared/Action/Components/ActionComponent.h"
#include "Shared/Action/BaseAction.h"


UActionComponent::UActionComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UActionComponent::BeginPlay()
{
    Super::BeginPlay();
    CreateActionInstances();
}

void UActionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
    FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    UpdateActiveActions(DeltaTime);
}

void UActionComponent::RegisterAction(const FGameplayTag& ActionTag, TSubclassOf<UBaseAction> ActionClass)
{
    if (!ActionTag.IsValid())
    {
        UE_LOG(LogTemp, Error, TEXT("ActionComponent::RegisterAction - Invalid ActionTag"));
        return;
    }

    if (!ActionClass)
    {
        UE_LOG(LogTemp, Error, TEXT("ActionComponent::RegisterAction - Invalid ActionClass for tag %s"), 
               *ActionTag.ToString());
        return;
    }

    if (ActionClasses.Contains(ActionTag))
    {
        UnregisterAction(ActionTag);
    }

    ActionClasses.Add(ActionTag, ActionClass);
    
    // 런타임에 등록된 경우 즉시 인스턴스 생성
    if (HasBegunPlay())
    {
        CreateActionInstance(ActionTag);
    }
}

void UActionComponent::UnregisterAction(const FGameplayTag& ActionTag)
{
    if (!ActionTag.IsValid())
        return;
    
    InterruptAction(ActionTag);

    if (UBaseAction* Action = ActionInstances.FindRef(ActionTag))
    {
        ActiveActions.Remove(Action);
        ActionInstances.Remove(ActionTag);
        
        Action->ConditionalBeginDestroy();
    }

    ActionClasses.Remove(ActionTag);

    UE_LOG(LogTemp, Log, TEXT("Unregistered action: %s"), *ActionTag.ToString());
}

UBaseAction* UActionComponent::GetAction(const FGameplayTag& ActionTag) const
{
    if (!ActionTag.IsValid())
        return nullptr;

    return ActionInstances.FindRef(ActionTag);
}

UBaseAction* UActionComponent::CreateActionInstance(const FGameplayTag& ActionTag)
{
    if (!ActionTag.IsValid())
    {
        return nullptr;
    }

    // 이미 인스턴스가 있다면 반환
    if (UBaseAction* ExistingAction = ActionInstances.FindRef(ActionTag))
    {
        return ExistingAction;
    }

    // 클래스 찾기
    TSubclassOf<UBaseAction> ActionClass = ActionClasses.FindRef(ActionTag);
    if (!ActionClass)
    {
        return nullptr;
    }

    // 새 인스턴스 생성
    UBaseAction* NewAction = NewObject<UBaseAction>(this, ActionClass);
    if (!NewAction)
    {
        UE_LOG(LogTemp, Error, TEXT("ActionComponent::CreateActionInstance - Failed to create instance for %s"), 
               *ActionTag.ToString());
        return nullptr;
    }

    // 초기화
    NewAction->Initialize(GetOwner());
    ActionInstances.Add(ActionTag, NewAction);

    return NewAction;
}

bool UActionComponent::ExecuteAction(const FGameplayTag& ActionTag)
{
    if (!ActionTag.IsValid())
        return false;

    UBaseAction* Action = ActionInstances.FindRef(ActionTag);
    if (!Action)
    {
        Action = CreateActionInstance(ActionTag);
        if (!Action)
            return false;
    }
    
    // 이미 활성이면 실패
    if (Action->IsActive())
        return false;
    
    // 태그 기반 우선순위 체크
    // if (Action->bCanInterruptOthers)
    // {
    //     InterruptActionsWithTags(Action->GrantedTags);
    // }
    
    // 태그 요구사항 체크
    // if (!CheckTagRequirements(Action))
    //     return false;
    
    return ExecuteActionInstance(Action);
}

bool UActionComponent::ExecuteActionInstance(UBaseAction* Action)
{
    if (ActiveActions.Contains(Action))
    {
        Action->Execute();
        return true;
    }
    
    // 새로운 액션
    if (!Action->CanExecute())
        return false;

    ActiveActions.AddUnique(Action);
    Action->Execute();
       
    return true;
}

void UActionComponent::InterruptAction(const FGameplayTag& ActionTag)
{
    if (!ActionTag.IsValid())
        return;

    UBaseAction* Action = ActionInstances.FindRef(ActionTag);
    if (!Action || !Action->IsActive())
        return;

    Action->Interrupt();
    ActiveActions.Remove(Action);

    UE_LOG(LogTemp, Log, TEXT("Interrupted action: %s"), *ActionTag.ToString());
}

void UActionComponent::InterruptAllActions()
{
    // 역순으로 순회하면서 중단 (안전한 제거를 위해)
    for (int32 i = ActiveActions.Num() - 1; i >= 0; --i)
    {
        UBaseAction* Action = ActiveActions[i];
        if (Action && Action->IsActive())
        {
            Action->Interrupt();
        }
    }
    
    ActiveActions.Empty();
}

bool UActionComponent::CheckTagRequirements(const UBaseAction* Action) const
{
    // 필수 태그 체크
    if (Action->RequiredTags.Num() > 0 && !ActiveTags.HasAll(Action->RequiredTags))
        return false;
    
    // 차단 태그 체크
    if (Action->BlockedTags.Num() > 0 && ActiveTags.HasAny(Action->BlockedTags))
        return false;
    
    return true;
}

void UActionComponent::InterruptActionsWithTags(const FGameplayTagContainer& Tags)
{
    for (int32 i = ActiveActions.Num() - 1; i >= 0; --i)
    {
        UBaseAction* ActiveAction = ActiveActions[i];
        if (ActiveAction && ActiveAction->bInterruptible)
        {
            // 활성 액션의 GrantedTags와 새 액션의 BlockedTags 비교
            if (ActiveAction->GrantedTags.HasAny(Tags))
            {
                ActiveAction->Interrupt();
            }
        }
    }
}

bool UActionComponent::IsActionActive(const FGameplayTag& ActionTag) const
{
    if (!ActionTag.IsValid())
        return false;

    UBaseAction* Action = ActionInstances.FindRef(ActionTag);
    return Action && Action->IsActive();
}

void UActionComponent::CreateActionInstances()
{
    for (const auto& ActionPair : ActionClasses)
    {
        const FGameplayTag& ActionTag = ActionPair.Key;
        TSubclassOf<UBaseAction> ActionClass = ActionPair.Value;
        
        if (!ActionTag.IsValid() || !ActionClass)
        {
            UE_LOG(LogTemp, Warning, TEXT("ActionComponent::CreateActionInstances - Invalid action data"));
            continue;
        }

        CreateActionInstance(ActionTag);
    }
}

void UActionComponent::UpdateActiveActions(float DeltaTime)
{
    // 역순으로 순회하면서 업데이트 (안전한 제거를 위해)
    for (int32 i = ActiveActions.Num() - 1; i >= 0; --i)
    {
        UBaseAction* Action = ActiveActions[i];
        
        if (!Action)
        {
            ActiveActions.RemoveAt(i);
            continue;
        }

        if (Action->IsActive())
        {
            Action->Tick(DeltaTime);
        }
        else
        {
            ActiveActions.RemoveAt(i);
            OnActionCompleted(Action);
        }
    }
}

void UActionComponent::OnActionCompleted(UBaseAction* Action)
{
    if (!Action) return;

    // 활성 목록에서 제거
    ActiveActions.Remove(Action);
}


// 추가 유틸리티 함수들
TArray<FGameplayTag> UActionComponent::GetRegisteredActionTags() const
{
    TArray<FGameplayTag> Tags;
    ActionClasses.GetKeys(Tags);
    return Tags;
}

TArray<UBaseAction*> UActionComponent::GetActionsByCategory(const FGameplayTag& CategoryTag) const
{
    TArray<UBaseAction*> CategoryActions;
    
    for (const auto& ActionPair : ActionInstances)
    {
        UBaseAction* Action = ActionPair.Value;
        if (Action && Action->GetActionTag().MatchesTag(CategoryTag))
        {
            CategoryActions.Add(Action);
        }
    }
    
    return CategoryActions;
}

void UActionComponent::GetActiveActionTags(TArray<FGameplayTag>& OutTags) const
{
    OutTags.Empty();
    
    for (const UBaseAction* Action : ActiveActions)
    {
        if (Action && Action->IsActive())
        {
            OutTags.Add(Action->GetActionTag());
        }
    }
}

// 디버깅용 유틸리티 함수들
void UActionComponent::DebugLogActiveActions() const
{
    UE_LOG(LogTemp, Log, TEXT("=== Active Actions Debug ==="));
    UE_LOG(LogTemp, Log, TEXT("Active Actions Count: %d"), ActiveActions.Num());
    
    for (int32 i = 0; i < ActiveActions.Num(); ++i)
    {
        UBaseAction* Action = ActiveActions[i];
        if (Action)
        {
            UE_LOG(LogTemp, Log, TEXT("[%d] %s - Active: %s, TimeActive: %.2f"), 
                   i, 
                   *Action->GetActionTag().ToString(), 
                   Action->IsActive() ? TEXT("TRUE") : TEXT("FALSE"),
                   Action->GetTimeActive());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("[%d] NULL ACTION"), i);
        }
    }
    UE_LOG(LogTemp, Log, TEXT("=== End Debug ==="));
}

void UActionComponent::DebugLogAllActions() const
{
    UE_LOG(LogTemp, Log, TEXT("=== All Actions Debug ==="));
    UE_LOG(LogTemp, Log, TEXT("Registered Actions Count: %d"), ActionInstances.Num());
    
    for (const auto& ActionPair : ActionInstances)
    {
        const FGameplayTag& Tag = ActionPair.Key;
        UBaseAction* Action = ActionPair.Value;
        
        if (Action)
        {
            UE_LOG(LogTemp, Log, TEXT("%s - Active: %s, Cooldown: %.2f"), 
                   *Tag.ToString(), 
                   Action->IsActive() ? TEXT("TRUE") : TEXT("FALSE"),
                   Action->GetCooldownRemaining());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("%s - NULL ACTION"), *Tag.ToString());
        }
    }
    UE_LOG(LogTemp, Log, TEXT("=== End Debug ==="));
}

FString UActionComponent::GetDebugString() const
{
    return {};
}


void UActionComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);
}

bool UActionComponent::CanEditChange(const FProperty* InProperty) const
{
    return Super::CanEditChange(InProperty);
}