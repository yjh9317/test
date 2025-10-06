#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ActionTypes.generated.h"

UENUM(BlueprintType)
enum class EActionFailure : uint8
{
	None, // 성공
	IsRunning, // 이미 실행 중
	IsBlocked, // 다른 태그에 의해 막힘
	NotEnoughResource, // 자원 부족 (마나, 스태미나 등)
	InCooldown // 쿨다운 중
};

USTRUCT(BlueprintType)
struct FActionCost
{
    GENERATED_BODY()

    // 예시: 비용으로 사용될 스탯 태그 (예: 'Attribute.Mana')
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTag CostAttributeTag;

    // 소모량
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float CostValue = 0.f;
};
