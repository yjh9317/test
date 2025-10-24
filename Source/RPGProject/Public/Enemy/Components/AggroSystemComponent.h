// Shared/Components/AI/AggroSystemComponent.h (또는 Enemy/Components/AI/)
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AggroSystemComponent.generated.h"

// 어그로 정보를 담는 구조체
USTRUCT(BlueprintType)
struct FAggroEntry
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aggro")
    TWeakObjectPtr<AActor> TargetActor; // 어그로 대상 액터 (TWeakObjectPtr로 순환 참조 방지)

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aggro")
    float AggroValue = 0.0f; // 현재 어그로 수치

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aggro")
    float LastDamagedTime = 0.0f; // 마지막으로 데미지를 준/받은 시간

    // 기타 필요한 정보 (거리, 시야 여부 등)
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewHighestAggroTarget, AActor*, NewTarget);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class RPGPROJECT_API UAggroSystemComponent : public UActorComponent // 또는 URPGProjectBaseComponent
{
    GENERATED_BODY()

public:
    UAggroSystemComponent();

protected:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aggro System")
    float AggroDropOffTime; // 어그로가 감소되기 시작하는 시간 (초)

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Aggro System")
    float AggroDropOffRate; // 초당 어그로 감소량

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aggro System|Runtime")
    TArray<FAggroEntry> AggroTable; // 현재 어그로 테이블

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aggro System|Runtime")
    TWeakObjectPtr<AActor> CurrentHighestAggroTarget;

public:
    // 데미지를 주거나 받을 때 호출되어 어그로를 추가/수정
    UFUNCTION(BlueprintCallable, Category = "Aggro System")
    void AddAggro(AActor* Target, float Amount);

    UFUNCTION(BlueprintCallable, Category = "Aggro System")
    void ModifyAggro(AActor* Target, float Amount);

    UFUNCTION(BlueprintPure, Category = "Aggro System")
    AActor* GetHighestAggroTarget() const;

    UFUNCTION(BlueprintPure, Category = "Aggro System")
    float GetAggroForTarget(AActor* Target) const;

    // 가장 높은 어그로 대상이 변경되었을 때 호출되는 델리게이트
    UPROPERTY(BlueprintAssignable, Category = "Aggro System")
    FOnNewHighestAggroTarget OnNewHighestAggroTargetChanged;

private:
    void UpdateAggroTable(float DeltaTime);
    void SortAggroTable();
    void CheckForNewHighestAggroTarget();

    FAggroEntry* FindOrAddAggroEntry(AActor* Target);
};