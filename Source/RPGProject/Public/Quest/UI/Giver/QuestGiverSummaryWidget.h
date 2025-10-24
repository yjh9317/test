// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestGiverSummaryWidget.generated.h"

class URPGQuest;
class UQuestSummaryButtonWidget;
class UQuestLogDisplayWidget;
class UVerticalBox;
class UButton;
class UImage;
/**
 * 
 */
DECLARE_DELEGATE_OneParam(FOnQuestEnable,URPGQuest* Quest);
DECLARE_DELEGATE_OneParam(FOnQuestDisable,URPGQuest* Quest);

UCLASS()
class RPGPROJECT_API UQuestGiverSummary : public UUserWidget
{
	GENERATED_BODY()
public:
	UQuestGiverSummary(const FObjectInitializer& ObjectInitializer);
	
	FOnQuestEnable OnQuestEnable;
	FOnQuestDisable OnQuestDisable;
	
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UImage> Image_Background;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UButton> Button_Exit;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UButton> Button_Left;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UButton> Button_Right;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_QuestList;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	UQuestLogDisplayWidget* QuestLogDisplayWidget;

	UPROPERTY(VisibleAnywhere)
	TArray<UQuestSummaryButtonWidget*> QuestButtons;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UQuestSummaryButtonWidget> QuestButtonSelected;

	UPROPERTY(VisibleAnywhere)
	TArray<URPGQuest*> HiddenQuests;

protected:

	void DisplayQuest();
	void DisplayQuestList();
	void CreateQuestButton(URPGQuest* InQuest);
	void RemoveQuestButton();
	void SetButtonsVisibility(bool IsVisible);
	void HideQuestDisplay();
	void UpdateQuestList();
	void CloseWidget();
private:
};
