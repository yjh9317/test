// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestSummaryButtonWidget.generated.h"

class URPGQuest;
class UTextBlock;
class UButton;
class UQuestSummaryButtonWidget;
/**
 * 
 */
DECLARE_DELEGATE_OneParam(FOnQuestSelected,UQuestSummaryButtonWidget* QuestButton);

UCLASS()
class RPGPROJECT_API UQuestSummaryButtonWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UQuestSummaryButtonWidget(const FObjectInitializer& ObjectInitializer);
	
	FOnQuestSelected OnQuestSelected;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UButton> Button_Quest;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_QuestTitle;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Specifier;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Quest | Variable")
	FButtonStyle SelectedStyle;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Quest | Variable")
	bool IsSelected;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Quest | Variable")
	FSlateColor SelectedColor;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Quest | Variable")
	FSlateColor NormalColor;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | Variable")
	TObjectPtr<URPGQuest> Quest;

protected:
	virtual void NativeConstruct() override;;

private:
	UFUNCTION()
	void QuestButtonClicked();

	UFUNCTION()
	void QuestButtonHovered();

	UFUNCTION()
	void QuestButtonUnhovered();
};
