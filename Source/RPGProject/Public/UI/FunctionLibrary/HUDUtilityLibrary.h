// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HUDUtilityLibrary.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API UHUDUtilityLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	// HUD
	UFUNCTION(BlueprintCallable, Category="HUD", meta=(WorldContext="WorldContextObject"))
	static UUserWidget* GetHUDContainer(UObject* WorldContextObject, TSubclassOf<UUserWidget> HudWidgetClass);

	// HUD BPI 만들 예정
	UFUNCTION(BlueprintCallable, Category="HUD", meta=(WorldContext="WorldContextObject"))
	static UUserWidget* GetHUDManager(UObject* WorldContextObject, int32 PlayerIndex = 0);

	// StyleSheet
	UFUNCTION(BlueprintCallable, Category="StyleSheet", meta=(WorldContext="WorldContextObject"))
	static FLinearColor GetBaseColor1(UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category="StyleSheet", meta=(WorldContext="WorldContextObject"))
	static bool GetUseColorStyleSheet(UObject* WorldContextObject);
};
