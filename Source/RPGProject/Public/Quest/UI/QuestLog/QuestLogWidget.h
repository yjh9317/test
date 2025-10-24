// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestLogWidget.generated.h"

class UQuestLogButtonWidget;
class URPGQuest;
class UScrollBox;
class UImage;
class UQuestListWidget;
class UQuestTrackerWidget;
class UQuestLogDisplayWidget;
class UButton;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UQuestLogWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UQuestLogWidget(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void NativeConstruct() override;
	
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UButton> Button_Abandon;
	
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UButton> Button_Exit;
	
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UQuestLogDisplayWidget> QuestLogDisplayWidget;
	
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UQuestListWidget> QuestListWidget;
	
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UImage> Image_Background;
	
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UImage> Image_BookLogo;
	
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox;
	
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UImage> Image_Title;
		
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Variable")
	bool FilterPerZone;

	UPROPERTY(BlueprintReadOnly,Category = "Quest | UI")
	TObjectPtr<UQuestTrackerWidget> QuestTrackerWidget;

	UPROPERTY(VisibleAnywhere)
	TArray<UQuestLogButtonWidget*> QuestButtons;

	UPROPERTY(VisibleAnywhere)
	UQuestLogButtonWidget* QuestButtonSelected;

	UPROPERTY(VisibleAnywhere)
	TArray<UQuestListWidget*> ZoneFilters;


	void AddQuestButton(URPGQuest* InQuest);
	void RemoveQuestButton(URPGQuest* InQuest);
	void OnQuestUpdateEvent(URPGQuest* InQuest);
	
	UQuestListWidget* CheckZoneFilters(URPGQuest* InQuest);
	void UpdateFilters();
	void CreateButton(UQuestListWidget* InQuestList,URPGQuest* InQuest);
	void RemoveButton(URPGQuest* InQuest);
	void RefreshQuestButtons(URPGQuest* InQuest);
private:
	UFUNCTION()
	void AbandonButtonClicked();
	UFUNCTION()
	void ExitButtonClicked();
	UFUNCTION()
	void DisplayQuest(UQuestLogButtonWidget* InQuestLogButtonWidget);
	UFUNCTION()
	void QuickDisplay(UQuestLogButtonWidget* InQuestLogButtonWidget);
};
