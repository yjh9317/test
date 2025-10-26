// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerStatusWidget.generated.h"

class UStatsViewModel;
class UProgressBar;
class UTextBlock;
class UImage;

/**
 * 플레이어 상태 표시 UI 위젯
 * - StatsViewModel과 연결되어 스탯 정보를 표시
 * - Health, Mana, Stamina, Experience, Level 정보 표시
 */
UCLASS()
class RPGPROJECT_API UPlayerStatusWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** 배경 이미지 */
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
	TObjectPtr<UImage> Image_Background;

	/** 캐릭터 아이콘 */
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget")
	TObjectPtr<UImage> Image_Character;

#pragma region Health_Widgets
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget|Health")
	TObjectPtr<UTextBlock> TextBlock_Health;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget|Health")
	TObjectPtr<UProgressBar> ProgressBar_Health;
#pragma endregion

#pragma region Mana_Widgets
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget|Mana")
	TObjectPtr<UTextBlock> TextBlock_Mana;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget|Mana")
	TObjectPtr<UProgressBar> ProgressBar_Mana;
#pragma endregion

#pragma region Stamina_Widgets
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget|Stamina")
	TObjectPtr<UTextBlock> TextBlock_Stamina;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget|Stamina")
	TObjectPtr<UProgressBar> ProgressBar_Stamina;
#pragma endregion
	
#pragma region Experience_Widgets
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget|EXP")
	TObjectPtr<UTextBlock> TextBlock_Experience;

	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget|EXP")
	TObjectPtr<UProgressBar> ProgressBar_Experience;
#pragma endregion

#pragma region Level_Widgets
	UPROPERTY(meta=(BindWidget), BlueprintReadOnly, Category="Widget|Level")
	TObjectPtr<UTextBlock> TextBlock_Level;
#pragma endregion

	/**
	 * ViewModel 설정
	 * @param InViewModel 연결할 StatsViewModel
	 */
	UFUNCTION(BlueprintCallable, Category="UI")
	void SetViewModel(UStatsViewModel* InViewModel);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	/** 연결된 ViewModel */
	UPROPERTY(Transient)
	TObjectPtr<UStatsViewModel> StatsViewModel;

private:
	/**
	 * ViewModel 초기화 및 바인딩
	 */
	void InitializeViewModel();
	void BindViewModel();
	void UnbindViewModel();
	void UpdateInitialDisplay();

#pragma region Health_Callbacks
	UFUNCTION()
	void OnHealthTextChanged(const FText& NewText);
	
	UFUNCTION()
	void OnHealthPercentChanged(float NewPercent);
	
	UFUNCTION()
	void OnHealthColorChanged(const FLinearColor& NewColor);
#pragma endregion

#pragma region Mana_Callbacks
	UFUNCTION()
	void OnManaTextChanged(const FText& NewText);
	
	UFUNCTION()
	void OnManaPercentChanged(float NewPercent);
	
	UFUNCTION()
	void OnManaColorChanged(const FLinearColor& NewColor);
#pragma endregion

#pragma region Stamina_Callbacks

	UFUNCTION()
	void OnStaminaTextChanged(const FText& NewText);
	
	UFUNCTION()
	void OnStaminaPercentChanged(float NewPercent);
	
	UFUNCTION()
	void OnStaminaColorChanged(const FLinearColor& NewColor);
#pragma endregion

#pragma region Experience_Callbacks

	UFUNCTION()
	void OnExperienceTextChanged(const FText& NewText);
	
	UFUNCTION()
	void OnExperiencePercentChanged(float NewPercent);
#pragma endregion

#pragma region Level_Callbacks

	UFUNCTION()
	void OnLevelChanged(const FText& NewText);
#pragma endregion
};