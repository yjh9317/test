// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BaseCharacter.h"
#include "Interface/InteractableInterface.h"
#include "NPCCharacter.generated.h"

/**
 * 
 */

class UQuestComponent;
class UInteractableComponent;
class UInventoryCoreComponent;

UENUM(BlueprintType)
enum class ENPCState : uint8
{
	Idle        UMETA(DisplayName = "Idle"),
	Patrolling  UMETA(DisplayName = "Patrolling"),
	Interacting UMETA(DisplayName = "Interacting"),
	Combat      UMETA(DisplayName = "Combat"),
	Dead        UMETA(DisplayName = "Dead")
};

UENUM(BlueprintType)
enum class ENPCType : uint8
{
	Generic     UMETA(DisplayName = "Generic"),
	Vendor      UMETA(DisplayName = "Vendor"),
	QuestGiver  UMETA(DisplayName = "Quest Giver"),
	Guard       UMETA(DisplayName = "Guard"),
	Civilian    UMETA(DisplayName = "Civilian")
};

USTRUCT(BlueprintType)
struct FNPCData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString NPCName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENPCType NPCType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InteractionRadius;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// TArray<FString> DialogueKeys;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsEssential; // 죽일 수 없는 NPC 여부
};

UCLASS()
class RPGPROJECT_API ANPCCharacter : public ABaseCharacter,public IInteractableInterface
{
	GENERATED_BODY()
public:
	ANPCCharacter(const FObjectInitializer& Initializer);

protected:
	virtual void PostInitializeComponents() override;
	// NPC 기본 정보
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|Data")
	FNPCData NPCData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "NPC|Data")
	FString NPCID;

	// 상태 관리
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC|State")
	ENPCState CurrentState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC|State")
	ENPCState DefaultState;

private:
	void ActivateComponentsByType();

	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Component", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UQuestComponent> QuestComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Component", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInventoryCoreComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Component", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UInteractableComponent> InteractableComponent;
};

