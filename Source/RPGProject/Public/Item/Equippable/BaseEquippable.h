// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/Equippable/AbstractEquippable.h"
#include "BaseEquippable.generated.h"

enum class EItemSlot : uint8;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API ABaseEquippable : public AAbstractEquippable
{
	GENERATED_BODY()

public:
	ABaseEquippable();

	virtual void AttachToNewSocket(FName NewSocket) override;
	virtual void ToggleCombatMode(bool CombatMode) override;

//   UPROPERTY(EditDefaultsOnly, Category = "Mesh Settings", meta=(EditCondition="MeshType==EMeshType::Static", EditConditionHides))
// protected:
// 	virtual void PostLoad() override; // 로드 후 초기화
// 	virtual void PostInitProperties() override; // 속성 초기화 후
//
// #if WITH_EDITOR
// 	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override; // 에디터에서 속성 변경 시
// #endif
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USceneComponent> ParentMesh;

	UPROPERTY(EditDefaultsOnly)
	TMap<EItemSlot,FName> NonCombatSocketNames;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<EItemSlot,FName> CombatSocketNames;
	
	UPROPERTY(EditDefaultsOnly)
	EItemSlot CurrentSlot;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UParticleSystem> ParticleSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Effects)
	UParticleSystemComponent* ParticleComponent;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USkeletalMeshComponent> SkeletalMesh;

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UStaticMeshComponent> StaticMesh;
};
