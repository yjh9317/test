// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/BaseWidgetController.h"

UBaseWidgetController::UBaseWidgetController()
{
}

void UBaseWidgetController::InitializeWidgetController(const FWidgetControllerInitParams& InInitParams)
{
	OwningPlayerController = InInitParams.PlayerController;
	OwningPlayerState = InInitParams.PlayerState;
	MainDataSourceComponent = InInitParams.DataSourceComponent;

	// 이 시점에서 MainDataSourceComponent가 유효한지 확인하고,
	// 유효하다면 필요한 초기 작업을 수행할 수 있습니다.
	// 예: if (GetMainDataSourceComponent<UPlayerStatsComponent>()) { ... }

	// BroadcastInitialValues(); // 여기서 호출하거나, 위젯이 준비된 후 외부에서 호출
	// BindCallbacksToDependencies(); // 여기서 호출하거나, 위젯이 준비된 후 외부에서 호출
}

void UBaseWidgetController::BroadcastInitialValues()
{
	// 파생 클래스에서 이 함수를 오버라이드하여
	// UI에 필요한 초기 데이터 값들을 델리게이트를 통해 브로드캐스트합니다.
	// 예: if (UPlayerStatsComponent* StatsComp = GetMainDataSourceComponent<UPlayerStatsComponent>())
	//     {
	//         OnHealthChanged.Broadcast(StatsComp->GetCurrentHealth(), StatsComp->GetMaxHealth());
	//     }
}

void UBaseWidgetController::BindCallbacksToDependencies()
{
	// 파생 클래스에서 이 함수를 오버라이드하여
	// MainDataSourceComponent나 PlayerState 등에서 제공하는 델리게이트에
	// 이 WidgetController 내부의 핸들러 함수들을 바인딩합니다.
	// 예: if (UPlayerStatsComponent* StatsComp = GetMainDataSourceComponent<UPlayerStatsComponent>())
	//     {
	//         StatsComp->OnHealthChangedDelegate.AddUObject(this, &UMyHealthWidgetController::HandleHealthUpdate);
	//     }
}

void UBaseWidgetController::CleanUp()
{
	OnLoadingStateChanged.Clear();
	OnShowUserMessage.Clear();
}

void UBaseWidgetController::BindCallBacksToDependencies()
{
}
