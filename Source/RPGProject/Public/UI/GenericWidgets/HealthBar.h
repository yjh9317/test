// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "HealthBar.generated.h"

class UProgressBar;
class USizeBox;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UHealthBar : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	USizeBox* SizeBox_Root;
	
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar_Main;

public:
	FORCEINLINE void SetHealthBar(float Percent) const { ProgressBar_Main->SetPercent(Percent); };
	FORCEINLINE float GetHealthBarPercent() const { return ProgressBar_Main->GetPercent(); }
};
