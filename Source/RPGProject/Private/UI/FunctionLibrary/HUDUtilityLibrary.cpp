// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/FunctionLibrary/HUDUtilityLibrary.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Kismet/GameplayStatics.h"

UUserWidget* UHUDUtilityLibrary::GetHUDContainer(UObject* WorldContextObject, TSubclassOf<UUserWidget> HUDWidgetClass)
{
	if (!WorldContextObject) return nullptr;
	
	TArray<UUserWidget*> HudWidgets; 
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(WorldContextObject,HudWidgets,HUDWidgetClass);
	
	if(!HudWidgets.IsEmpty())
	{
		return HudWidgets[0];
	}
	return nullptr;
}

UUserWidget* UHUDUtilityLibrary::GetHUDManager(UObject* WorldContextObject, int32 PlayerIndex)
{
	if (!WorldContextObject) return nullptr;

	UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject);
	if (!World) { return nullptr; }

	APlayerController* PC = UGameplayStatics::GetPlayerController(World, PlayerIndex);
	if (!PC)
	{
		return nullptr;
	}
	
	return nullptr;
}

FLinearColor UHUDUtilityLibrary::GetBaseColor1(UObject* WorldContextObject)
{
	return FLinearColor{};
}

bool UHUDUtilityLibrary::GetUseColorStyleSheet(UObject* WorldContextObject)
{
	return false;
}



