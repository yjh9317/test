#pragma once

#include "CoreMinimal.h"
#include "Character/BaseAnimInstance.h"
#include "EnemyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UEnemyAnimInstance : public UBaseAnimInstance
{
	GENERATED_BODY()
	
public:
	UEnemyAnimInstance(const FObjectInitializer& ObjectInitializer);
	friend class AEnemyCharacter;
	
	virtual void NativeBeginPlay() override;
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
};