// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestTrackerObjectivesWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UQuestTrackerObjectivesWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UQuestTrackerObjectivesWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION()
	void OnObjectiveUpdate();

	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Objective;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Variable")
	int32 CurrentObjectiveAmount = 0;
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UImage> Image_GlowyLine;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | Animation", meta = (BindWidgetAnim),Transient)
	TObjectPtr<UWidgetAnimation> OnUpdateAnimation;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variable")
	int32 NumLoopsPlay = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variable")
	float PlaybackSpeed = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variable")
	float TickIntervals = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variable")
	float AccelerationRate = 1.25f;

private:
	FTimerHandle TickTimer;
};
