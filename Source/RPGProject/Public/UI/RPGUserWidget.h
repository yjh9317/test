// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RPGUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class RPGPROJECT_API URPGUserWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();

	void SetFunctionTimer(FTimerDelegate& DelegateToSet,float Delay);
protected:
	
	UFUNCTION()
	virtual void InitializeWidget() {};

	FTimerHandle InitTimerHandle;
	
	UPROPERTY(EditDefaultsOnly)
	float TimerRate = 0.5f;
};
