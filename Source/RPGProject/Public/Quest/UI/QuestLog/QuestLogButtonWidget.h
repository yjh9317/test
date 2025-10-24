// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Quest/RPGQuest.h"
#include "QuestLogButtonWidget.generated.h"

class UTextBlock;
class URPGQuest;
class UScrollBox;
class UImage;
class UQuestListWidget;
class UQuestTrackerWidget;
class UQuestLogDisplayWidget;
class UQuestLogButtonWidget;
class UButton;
/**
 * 
 */

DECLARE_DELEGATE_OneParam(FOnQuestLogButtonSelected,UQuestLogButtonWidget* QuestButton);
DECLARE_DELEGATE_OneParam(FOnQuestFollowed,UQuestLogButtonWidget* QuestButton);
UCLASS()
class RPGPROJECT_API UQuestLogButtonWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UQuestLogButtonWidget(const FObjectInitializer& ObjectInitializer);

	FOnQuestLogButtonSelected OnQuestLogButtonSelected;
	FOnQuestFollowed OnQuestFollowed;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variable")
	TObjectPtr<URPGQuest> Quest;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_QuestTitle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variable")
	bool IsSelected;
	
	UFUNCTION()
	void OnDeselect();
	
	void UpdateFollowButtonStyle(bool IsFollowed);
	void UpdateSpecifier();
	
protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UButton> Button_Follow;
	
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UButton> Button_Quest;
	
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Specifier;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variable")
	FButtonStyle EmptyShieldStyle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variable")
	FButtonStyle FillShieldStyle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variable")
	FButtonStyle SelectedStyle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variable")
	FButtonStyle NormalStyle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Variable")
	bool IsQuestFollowed;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variable")
	FSlateColor SelectedColor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variable")
	FSlateColor NormalColor;

private:
	UFUNCTION()
	void QuestButtonClicked();
	UFUNCTION()
	void QuestButtonHovered();
	UFUNCTION()
	void QuestButtonUnhovered();
	UFUNCTION()
	void FollowButtonClicked();
};
