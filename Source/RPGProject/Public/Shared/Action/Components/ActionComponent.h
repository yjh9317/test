// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "ActionComponent.generated.h"

class UBaseAction;
class ABaseCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionExecuted, const FGameplayTag&, ActionTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionCompleted, const FGameplayTag&, ActionTag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionInterrupted, const FGameplayTag&, ActionTag);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UActionComponent : public UActorComponent
{
    GENERATED_BODY()
public:
    UActionComponent();
    
protected:
    // === 액션 데이터 ===
    UPROPERTY(EditDefaultsOnly, Category = "Actions", meta = (DisplayName = "Default Actions"))
    TMap<FGameplayTag, TSubclassOf<UBaseAction>> ActionClasses;

    UPROPERTY(BlueprintReadOnly, Category = "Action State")
    TMap<FGameplayTag, UBaseAction*> ActionInstances;

    UPROPERTY(BlueprintReadOnly, Category = "Action State")
    TArray<UBaseAction*> ActiveActions;

    // === 태그 시스템 ===
    UPROPERTY(BlueprintReadOnly, Category = "Tags")
    FGameplayTagContainer ActiveTags;

    // === 성능 설정 ===
    UPROPERTY(EditDefaultsOnly, Category = "Performance", meta = (ClampMin = "1"))
    int32 MaxConcurrentActions = 10;

    UPROPERTY(EditDefaultsOnly, Category = "Performance")
    bool bEnableDetailedLogging = true;

public:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    // === Action 관리 ===
    UFUNCTION(BlueprintCallable, Category = "Actions")
    void RegisterAction(const FGameplayTag& ActionTag, TSubclassOf<UBaseAction> ActionClass);

    UFUNCTION(BlueprintCallable, Category = "Actions")
    void UnregisterAction(const FGameplayTag& ActionTag);

    UFUNCTION(BlueprintCallable, Category = "Actions")
    UBaseAction* GetAction(const FGameplayTag& ActionTag) const;

    UFUNCTION(BlueprintCallable, Category = "Actions")
    UBaseAction* CreateActionInstance(const FGameplayTag& ActionTag);

    // === Action 실행 ===
    UFUNCTION(BlueprintCallable, Category = "Actions")
    bool ExecuteAction(const FGameplayTag& ActionTag);

    UFUNCTION(BlueprintCallable, Category = "Actions")
    bool ExecuteActionInstance(UBaseAction* Action);

    UFUNCTION(BlueprintCallable, Category = "Actions")
    void InterruptAction(const FGameplayTag& ActionTag);

    UFUNCTION(BlueprintCallable, Category = "Actions")
    void InterruptAllActions();
    
    // === 태그 관리 ===
    UFUNCTION(BlueprintCallable, Category = "Tags")
    void AddTags(const FGameplayTagContainer& Tags) { ActiveTags.AppendTags(Tags); }

    UFUNCTION(BlueprintCallable, Category = "Tags")
    void RemoveTags(const FGameplayTagContainer& Tags) { ActiveTags.RemoveTags(Tags); }

    UFUNCTION(BlueprintPure, Category = "Tags")
    FGameplayTagContainer GetActiveTags() const { return ActiveTags; }

    UFUNCTION(BlueprintPure, Category = "Tags")
    bool HasTag(const FGameplayTag& Tag) const { return ActiveTags.HasTag(Tag); }

    UFUNCTION(BlueprintPure, Category = "Tags")
    bool HasAllTags(const FGameplayTagContainer& Tags) const { return ActiveTags.HasAll(Tags); }

    UFUNCTION(BlueprintPure, Category = "Tags")
    bool HasAnyTags(const FGameplayTagContainer& Tags) const { return ActiveTags.HasAny(Tags); }

    UFUNCTION(BlueprintCallable, Category = "Tags")
    bool CheckTagRequirements(const UBaseAction* Action) const;

    UFUNCTION(BlueprintCallable, Category = "Tags")
    void InterruptActionsWithTags(const FGameplayTagContainer& Tags);
    // === 쿼리 ===
    UFUNCTION(BlueprintPure, Category = "Actions")
    bool IsActionActive(const FGameplayTag& ActionTag) const;

    UFUNCTION(BlueprintPure, Category = "Actions")
    bool IsActionRegistered(const FGameplayTag& ActionTag) const { return ActionClasses.Contains(ActionTag); }

    UFUNCTION(BlueprintPure, Category = "Actions")
    TArray<UBaseAction*> GetActiveActions() const { return ActiveActions; }

    UFUNCTION(BlueprintPure, Category = "Actions")
    TArray<FGameplayTag> GetRegisteredActionTags() const;

    UFUNCTION(BlueprintPure, Category = "Actions")
    TArray<UBaseAction*> GetActionsByCategory(const FGameplayTag& CategoryTag) const;

    UFUNCTION(BlueprintPure, Category = "Actions")
    int32 GetActiveActionCount() const { return ActiveActions.Num(); }

    UFUNCTION(BlueprintPure, Category = "Actions")
    bool HasAnyActiveActions() const { return ActiveActions.Num() > 0; }

    UFUNCTION(BlueprintCallable, Category = "Actions")
    void GetActiveActionTags(TArray<FGameplayTag>& OutTags) const;

   

    // === 디버깅 ===
    UFUNCTION(BlueprintCallable, Category = "Debug", CallInEditor)
    void DebugLogActiveActions() const;

    UFUNCTION(BlueprintCallable, Category = "Debug", CallInEditor)
    void DebugLogAllActions() const;

    UFUNCTION(BlueprintPure, Category = "Debug")
    FString GetDebugString() const;

protected:
    // === 내부 관리 함수들 ===
    void CreateActionInstances();
    void UpdateActiveActions(float DeltaTime);
    UFUNCTION()
    void OnActionCompleted(UBaseAction* Action);

#if WITH_EDITOR
    virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif
};


