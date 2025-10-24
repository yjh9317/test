// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Quest/Data/QuestStruct.h"
#include "QuestMarkerWidget.generated.h"

class UImage;
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UQuestMarkerWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "Advanced Quest | UI")
	void SetImageQuest(bool isEnded, EQuestType questType = EQuestType::EQuest_MainQuest);

	UPROPERTY(EditDefaultsOnly, Category="Quest Marker")
	TObjectPtr<UTexture2D> MainQuestAvailableIcon;

	UPROPERTY(EditDefaultsOnly, Category="Quest Marker")
	TObjectPtr<UTexture2D> MainQuestCompleteIcon;

	UPROPERTY(EditDefaultsOnly, Category="Quest Marker")
	TObjectPtr<UTexture2D> SubQuestAvailableIcon;

	UPROPERTY(EditDefaultsOnly, Category="Quest Marker")
	TObjectPtr<UTexture2D> SubQuestCompleteIcon;

	UPROPERTY(VisibleInstanceOnly,Category="Image",meta=(bindWidget))
	TObjectPtr<UImage> Image_Marker;
	
	UPROPERTY(EditDefaultsOnly, Category="Variables")
	float ImageSize = 512.f;
};
