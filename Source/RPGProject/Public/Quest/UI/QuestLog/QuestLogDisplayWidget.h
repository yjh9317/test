// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Quest/Data/RPGQuestData.h"
#include "QuestLogDisplayWidget.generated.h"

class UHorizontalBox;
class UVerticalBox;
class UImage;
class UTextBlock;
class URPGQuest;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UQuestLogDisplayWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UQuestLogDisplayWidget(const FObjectInitializer& ObjectInitializer);
	
	void AddObjectiveTexts(const FQuestObjectives& InObjective);
	void UpdateObjectiveText(URPGQuest* InQuest);
	void SetObjectiveAmountText(const FQuestObjectives& CurrentObjective,UTextBlock* InTextBlock);
	void DisplayQuest(URPGQuest* InQuest);
	void AddImages(URPGQuest* InQuest);

	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_QuestDisplay;
protected:
	
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_QuestDescription;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_QuestSummary;
	
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_QuestTitle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Variable")
	FSlateFontInfo ObjectivesFont;
	
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UImage> Image_Background;

	
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Experience;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_Gold;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_ItemRewards;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_Objectives;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UHorizontalBox> HorizontalBox_Rewards;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Variable")
	float ItemSpriteSize = 75.f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Variable")
	float SpaceBetweenSprites = 10.f;
	
};
