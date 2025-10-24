// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "QuestListWidget.generated.h"

class UTextBlock;
class UHorizontalBox;
class UButton;
class UVerticalBox;
class URPGQuest;
class UQuestLogButtonWidget;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UQuestListWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UQuestListWidget(const FObjectInitializer& ObjectInitializer);

	UQuestLogButtonWidget* CreateButton(URPGQuest* InQuest);
	void SetTextZones(const FText& InText);
	
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UHorizontalBox> HorizontalBox_ZoneFilter;
	
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UTextBlock> TextBlock_ZoneTitle;

	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UVerticalBox> VerticalBox_QuestList;
	
protected:
	virtual void NativeConstruct() override;
	UPROPERTY(BlueprintReadOnly, Category = "Quest | UI", meta=(BindWidget))
	TObjectPtr<UButton> Button_Zone;
	
private:
	UFUNCTION()
	void ZoneButtonClick();
	
};
