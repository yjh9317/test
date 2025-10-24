// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Components/AggroSystemComponent.h"

UAggroSystemComponent::UAggroSystemComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    AggroDropOffTime = 10.0f;
    AggroDropOffRate = 5.0f;
}

void UAggroSystemComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UAggroSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    UpdateAggroTable(DeltaTime);
    CheckForNewHighestAggroTarget();
}

FAggroEntry* UAggroSystemComponent::FindOrAddAggroEntry(AActor* Target)
{
    if (!Target) return nullptr;

    for (FAggroEntry& Entry : AggroTable)
    {
        if (Entry.TargetActor.Get() == Target)
        {
            return &Entry;
        }
    }
    // 새 항목 추가
    FAggroEntry NewEntry;
    NewEntry.TargetActor = Target;
    NewEntry.LastDamagedTime = GetWorld()->GetTimeSeconds(); // 현재 시간으로 초기화
    AggroTable.Add(NewEntry);
    return &AggroTable.Last();
}

void UAggroSystemComponent::AddAggro(AActor* Target, float Amount)
{
    if (!Target || Amount <= 0) return;

    FAggroEntry* Entry = FindOrAddAggroEntry(Target);
    if (Entry)
    {
        Entry->AggroValue += Amount;
        Entry->LastDamagedTime = GetWorld()->GetTimeSeconds();
        // DebugHeader::Log(FString::Printf(TEXT("%s added %f aggro to %s. Total: %f"), *GetOwner()->GetName(), Amount, *Target->GetName(), Entry->AggroValue));
    }
    SortAggroTable();
}

void UAggroSystemComponent::ModifyAggro(AActor* Target, float Amount)
{
    if (!Target) return;
    FAggroEntry* Entry = FindOrAddAggroEntry(Target);
    if (Entry)
    {
        Entry->AggroValue = FMath::Max(0.0f, Entry->AggroValue + Amount); // 음수가 되지 않도록
        Entry->LastDamagedTime = GetWorld()->GetTimeSeconds();
    }
    SortAggroTable();
}


float UAggroSystemComponent::GetAggroForTarget(AActor* Target) const
{
    if (!Target) return 0.0f;
    for (const FAggroEntry& Entry : AggroTable)
    {
        if (Entry.TargetActor.Get() == Target)
        {
            return Entry.AggroValue;
        }
    }
    return 0.0f;
}

void UAggroSystemComponent::UpdateAggroTable(float DeltaTime)
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    bool bNeedsSort = false;

    for (int32 i = AggroTable.Num() - 1; i >= 0; --i)
    {
        FAggroEntry& Entry = AggroTable[i];
        if (!Entry.TargetActor.IsValid()) // 대상이 사라졌으면 제거
        {
            AggroTable.RemoveAt(i);
            bNeedsSort = true;
            continue;
        }

        // 일정 시간 어그로 변동이 없으면 어그로 감소
        if (CurrentTime - Entry.LastDamagedTime > AggroDropOffTime)
        {
            Entry.AggroValue -= AggroDropOffRate * DeltaTime;
            if (Entry.AggroValue <= 0)
            {
                AggroTable.RemoveAt(i); // 어그로가 0 이하면 테이블에서 제거
            }
            bNeedsSort = true;
        }
    }

    if (bNeedsSort)
    {
        SortAggroTable();
    }
}

void UAggroSystemComponent::SortAggroTable()
{
    AggroTable.Sort([](const FAggroEntry& A, const FAggroEntry& B) {
        return A.AggroValue > B.AggroValue; // 내림차순 정렬
    });
}

void UAggroSystemComponent::CheckForNewHighestAggroTarget()
{
    AActor* NewTarget = nullptr;
    if (AggroTable.Num() > 0 && AggroTable[0].AggroValue > 0) // 가장 높은 어그로 대상
    {
        NewTarget = AggroTable[0].TargetActor.Get();
    }

    if (CurrentHighestAggroTarget.Get() != NewTarget)
    {
        CurrentHighestAggroTarget = NewTarget;
        OnNewHighestAggroTargetChanged.Broadcast(NewTarget);
        // if (NewTarget) DebugHeader::Log(FString::Printf(TEXT("%s new highest aggro target: %s"), *GetOwner()->GetName(), *NewTarget->GetName()));
        // else DebugHeader::Log(FString::Printf(TEXT("%s lost aggro target."),*GetOwner()->GetName()));
    }
}

AActor* UAggroSystemComponent::GetHighestAggroTarget() const
{
    return CurrentHighestAggroTarget.Get();
}