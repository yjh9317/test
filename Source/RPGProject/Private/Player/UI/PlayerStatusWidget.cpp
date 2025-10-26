// Fill out your copyright notice in the Description page of Project Settings.

#include "Player/UI/PlayerStatusWidget.h"
#include "Components/UI/StatsViewModel.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UPlayerStatusWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StatsViewModel)
	{
		InitializeViewModel();
	}
}

void UPlayerStatusWidget::NativeDestruct()
{
	UnbindViewModel();
	
	Super::NativeDestruct();
}

void UPlayerStatusWidget::SetViewModel(UStatsViewModel* InViewModel)
{
	// 기존 ViewModel 바인딩 해제
	if (StatsViewModel)
	{
		UnbindViewModel();
	}

	StatsViewModel = InViewModel;

	// 위젯이 이미 생성된 상태라면 즉시 초기화
	if (IsInViewport() && StatsViewModel)
	{
		InitializeViewModel();
	}
}

void UPlayerStatusWidget::InitializeViewModel()
{
	if (!StatsViewModel)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPlayerStatusWidget::InitializeViewModel - StatsViewModel is nullptr"));
		return;
	}
	
	BindViewModel();
	UpdateInitialDisplay();
}

void UPlayerStatusWidget::BindViewModel()
{
	if (!StatsViewModel)
	{
		return;
	}

	// Health 델리게이트 바인딩
	StatsViewModel->OnHealthTextChanged.AddUObject(this, &UPlayerStatusWidget::OnHealthTextChanged);
	StatsViewModel->OnHealthPercentChanged.AddUObject(this, &UPlayerStatusWidget::OnHealthPercentChanged);
	StatsViewModel->OnHealthColorChanged.AddUObject(this, &UPlayerStatusWidget::OnHealthColorChanged);

	// Mana 델리게이트 바인딩
	StatsViewModel->OnManaTextChanged.AddUObject(this, &UPlayerStatusWidget::OnManaTextChanged);
	StatsViewModel->OnManaPercentChanged.AddUObject(this, &UPlayerStatusWidget::OnManaPercentChanged);
	StatsViewModel->OnManaColorChanged.AddUObject(this, &UPlayerStatusWidget::OnManaColorChanged);

	// Stamina 델리게이트 바인딩
	StatsViewModel->OnStaminaTextChanged.AddUObject(this, &UPlayerStatusWidget::OnStaminaTextChanged);
	StatsViewModel->OnStaminaPercentChanged.AddUObject(this, &UPlayerStatusWidget::OnStaminaPercentChanged);
	StatsViewModel->OnStaminaColorChanged.AddUObject(this, &UPlayerStatusWidget::OnStaminaColorChanged);

	// Experience 델리게이트 바인딩
	StatsViewModel->OnExperienceTextChanged.AddUObject(this, &UPlayerStatusWidget::OnExperienceTextChanged);
	StatsViewModel->OnExperiencePercentChanged.AddUObject(this, &UPlayerStatusWidget::OnExperiencePercentChanged);

	// Level 델리게이트 바인딩
	StatsViewModel->OnLevelChanged.AddUObject(this, &UPlayerStatusWidget::OnLevelChanged);
}

void UPlayerStatusWidget::UnbindViewModel()
{
	if (!StatsViewModel)
	{
		return;
	}

	// 모든 델리게이트 바인딩 해제
	StatsViewModel->OnHealthTextChanged.RemoveAll(this);
	StatsViewModel->OnHealthPercentChanged.RemoveAll(this);
	StatsViewModel->OnHealthColorChanged.RemoveAll(this);

	StatsViewModel->OnManaTextChanged.RemoveAll(this);
	StatsViewModel->OnManaPercentChanged.RemoveAll(this);
	StatsViewModel->OnManaColorChanged.RemoveAll(this);

	StatsViewModel->OnStaminaTextChanged.RemoveAll(this);
	StatsViewModel->OnStaminaPercentChanged.RemoveAll(this);
	StatsViewModel->OnStaminaColorChanged.RemoveAll(this);

	StatsViewModel->OnExperienceTextChanged.RemoveAll(this);
	StatsViewModel->OnExperiencePercentChanged.RemoveAll(this);

	StatsViewModel->OnLevelChanged.RemoveAll(this);
}

void UPlayerStatusWidget::UpdateInitialDisplay()
{
	if (!StatsViewModel)
	{
		return;
	}

	// ViewModel의 현재 값으로 UI 초기화
	OnHealthTextChanged(StatsViewModel->GetHealthText());
	OnHealthPercentChanged(StatsViewModel->GetHealthPercent());
	OnHealthColorChanged(StatsViewModel->GetHealthColor());

	OnManaTextChanged(StatsViewModel->GetManaText());
	OnManaPercentChanged(StatsViewModel->GetManaPercent());
	OnManaColorChanged(StatsViewModel->GetManaColor());

	OnStaminaTextChanged(StatsViewModel->GetStaminaText());
	OnStaminaPercentChanged(StatsViewModel->GetStaminaPercent());
	OnStaminaColorChanged(StatsViewModel->GetStaminaColor());

	OnExperienceTextChanged(StatsViewModel->GetExperienceText());
	OnExperiencePercentChanged(StatsViewModel->GetExperiencePercent());

	OnLevelChanged(StatsViewModel->GetLevelText());
}

#pragma region Health_Callbacks

void UPlayerStatusWidget::OnHealthTextChanged(const FText& NewText)
{
	if (TextBlock_Health)
	{
		TextBlock_Health->SetText(NewText);
	}
}

void UPlayerStatusWidget::OnHealthPercentChanged(float NewPercent)
{
	if (ProgressBar_Health)
	{
		ProgressBar_Health->SetPercent(NewPercent);
	}
}

void UPlayerStatusWidget::OnHealthColorChanged(const FLinearColor& NewColor)
{
	if (ProgressBar_Health)
	{
		ProgressBar_Health->SetFillColorAndOpacity(NewColor);
	}
}

#pragma endregion

#pragma region Mana_Callbacks

void UPlayerStatusWidget::OnManaTextChanged(const FText& NewText)
{
	if (TextBlock_Mana)
	{
		TextBlock_Mana->SetText(NewText);
	}
}

void UPlayerStatusWidget::OnManaPercentChanged(float NewPercent)
{
	if (ProgressBar_Mana)
	{
		ProgressBar_Mana->SetPercent(NewPercent);
	}
}

void UPlayerStatusWidget::OnManaColorChanged(const FLinearColor& NewColor)
{
	if (ProgressBar_Mana)
	{
		ProgressBar_Mana->SetFillColorAndOpacity(NewColor);
	}
}

#pragma endregion

#pragma region Stamina_Callbacks

void UPlayerStatusWidget::OnStaminaTextChanged(const FText& NewText)
{
	if (TextBlock_Stamina)
	{
		TextBlock_Stamina->SetText(NewText);
	}
}

void UPlayerStatusWidget::OnStaminaPercentChanged(float NewPercent)
{
	if (ProgressBar_Stamina)
	{
		ProgressBar_Stamina->SetPercent(NewPercent);
	}
}

void UPlayerStatusWidget::OnStaminaColorChanged(const FLinearColor& NewColor)
{
	if (ProgressBar_Stamina)
	{
		ProgressBar_Stamina->SetFillColorAndOpacity(NewColor);
	}
}

#pragma endregion

#pragma region Experience_Callbacks

void UPlayerStatusWidget::OnExperienceTextChanged(const FText& NewText)
{
	if (TextBlock_Experience)
	{
		TextBlock_Experience->SetText(NewText);
	}
}

void UPlayerStatusWidget::OnExperiencePercentChanged(float NewPercent)
{
	if (ProgressBar_Experience)
	{
		ProgressBar_Experience->SetPercent(NewPercent);
	}
}

#pragma endregion

#pragma region Level_Callbacks

void UPlayerStatusWidget::OnLevelChanged(const FText& NewText)
{
	if (TextBlock_Level)
	{
		TextBlock_Level->SetText(NewText);
	}
}

#pragma endregion