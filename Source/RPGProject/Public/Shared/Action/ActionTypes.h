#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "ActionTypes.generated.h"

UENUM(BlueprintType)
enum class EActionFailure : uint8
{
	None,
	IsRunning,
	IsBlocked,
	NotEnoughResource,
	InCooldown
};

USTRUCT(BlueprintType)
struct FActionCost
{
    GENERATED_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    FGameplayTag CostAttributeTag;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
    float CostValue = 0.f;
};
