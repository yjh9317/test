// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ComponentManager.generated.h"

class UStationManagerComponent;
class UMotionWarpingComponent;
class ULevelingComponent;
class UBuffComponent;
class UStatsComponent;
class UEquipmentComponent;
class UInventoryComponent;
class UInteractionComponent;
class AMainCharacter;
class UWidgetManager;
class UActionComponent;
class UInputBufferComponent;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UComponentManager : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class RPGPROJECT_API IComponentManager
{
	GENERATED_BODY()

public:
	// Component
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Component Manager")
	// UInteractionComponent* GetInteractionComponent();
	// virtual UInteractionComponent* GetInteractionComponent_Implementation() {return nullptr;}
	//
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Component Manager")
	// UInventoryComponent* GetInventoryComponent();
	// virtual UInventoryComponent* GetInventoryComponent_Implementation() { return nullptr;}
	//
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Component Manager")
	// UActionComponent* GetActionComponent();
	// virtual UActionComponent* GetActionComponent_Implementation() {return nullptr;}
	//
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Component Manager")
	// UEquipmentComponent* GetEquipmentComponent();
	// virtual UEquipmentComponent* GetEquipmentComponent_Implementation() { return nullptr;}
	//
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Component Manager")
	// UStatsComponent* GetStatsComponent();
	// virtual UStatsComponent* GetStatsComponent_Implementation() {return nullptr;}
	//
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Component Manager")
	// ULevelingComponent* GetLevelingComponent();
	// virtual ULevelingComponent* GetLevelingComponent_Implementation() {return nullptr;}
	//
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Component Manager")
	// UInputBufferComponent* GetInputBufferComponent();
	// virtual UInputBufferComponent* GetInputBufferComponent_Implementation() {return nullptr;}
	//
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Component Manager")
	// UBuffComponent* GetBuffComponent();
	// virtual UBuffComponent* GetBuffComponent_Implementation() {return nullptr;}
	//
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Component Manager")
	// UStationManagerComponent* GetStationsManagerComponent();
	// virtual UStationManagerComponent* GetStationsManagerComponent_Implementation() {return nullptr;}
	//
	// UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Component Manager")
	// UMotionWarpingComponent* GetMotionWarpingComponent();
	// virtual UMotionWarpingComponent* GetMotionWarpingComponent_Implementation() {return nullptr;}

	virtual UInteractionComponent* GetInteractionComponent() { return nullptr; }
	virtual UInventoryComponent* GetInventoryComponent() { return nullptr;}
	virtual UActionComponent* GetActionComponent() {return nullptr;}
	virtual UEquipmentComponent* GetEquipmentComponent() { return nullptr;}
	virtual UStatsComponent* GetStatsComponent() {return nullptr;}
	virtual ULevelingComponent* GetLevelingComponent() {return nullptr;}
	virtual UInputBufferComponent* GetInputBufferComponent() {return nullptr;}
	virtual UBuffComponent* GetBuffComponent() {return nullptr;}
	virtual UMotionWarpingComponent* GetMotionWarpingComponent() {return nullptr;}
	virtual UStationManagerComponent* GetStationManagerComponent() {return nullptr;}
};

