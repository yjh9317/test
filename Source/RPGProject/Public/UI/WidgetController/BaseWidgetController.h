// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseWidgetController.generated.h"

USTRUCT(BlueprintType)
struct FWidgetControllerInitParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WidgetController")
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WidgetController")
	TObjectPtr<APlayerState> PlayerState = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "WidgetController")
	TObjectPtr<UActorComponent> DataSourceComponent = nullptr;

	FWidgetControllerInitParams() {}
	FWidgetControllerInitParams(APlayerController* PC, APlayerState* PS, UActorComponent* DSC)
		: PlayerController(PC), PlayerState(PS), DataSourceComponent(DSC) {}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLoadingStateChangedSignature, bool, bIsLoading);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShowMessageSignature, const FText&, MessageText);

UCLASS()
class RPGPROJECT_API UBaseWidgetController : public UObject
{
	GENERATED_BODY()

public:
	UBaseWidgetController();
	
	UFUNCTION(BlueprintCallable, Category = "WidgetController")
	virtual void InitializeWidgetController(const FWidgetControllerInitParams& InInitParams);

	// UI에 초기 데이터 값을 브로드캐스트하는 함수. 파생 클래스에서 구체적인 내용을 구현.
	UFUNCTION(BlueprintCallable, Category = "WidgetController")
	virtual void BroadcastInitialValues();

	// 데이터 소스의 변경 사항에 콜백 함수들을 바인딩하는 함수. 파생 클래스에서 구현.
	UFUNCTION(BlueprintCallable, Category = "WidgetController")
	virtual void BindCallbacksToDependencies();

	// 위젯 컨트롤러가 더 이상 필요 없을 때 호출되어 바인딩된 콜백 등을 정리
	UFUNCTION(BlueprintCallable, Category = "WidgetController")
	virtual void CleanUp();

	// --- 공통 델리게이트 예시 ---
	UPROPERTY(BlueprintAssignable, Category = "WidgetController|CommonEvents")
	FOnLoadingStateChangedSignature OnLoadingStateChanged;

	UPROPERTY(BlueprintAssignable, Category = "WidgetController|CommonEvents")
	FOnShowMessageSignature OnShowUserMessage;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController|CoreReferences")
	TObjectPtr<APlayerController> OwningPlayerController;

	UPROPERTY(BlueprintReadOnly, Category = "WidgetController|CoreReferences")
	TObjectPtr<APlayerState> OwningPlayerState;

	// 게임 데이터의 주요 출처가 되는 컴포넌트. (예: 스탯, 인벤토리 등을 관리)
	UPROPERTY(BlueprintReadOnly, Category = "WidgetController|CoreReferences")
	TObjectPtr<UActorComponent> MainDataSourceComponent;


	template<typename T>
	T* GetOwningPlayerController() const { return Cast<T>(OwningPlayerController); }

	template<typename T>
	T* GetOwningPlayerState() const { return Cast<T>(OwningPlayerState); }

	template<typename T>
	T* GetMainDataSourceComponent() const { return Cast<T>(MainDataSourceComponent); }


	// virtual void HandleSpecificUIAction1(); // 예: 인벤토리 열기 버튼 처리
	// virtual void OnPlayerDataChanged_SpecificStat(); // 예: 특정 스탯 변경 시 처리

protected:
	virtual void BindCallBacksToDependencies();
	// UPROPERTY(BlueprintReadOnly, Category = "WidgetController")
	// TObjectPtr<UUserWidget> BoundWidget;

	// void SetBoundWidget(UUserWidget* InWidget) { BoundWidget = InWidget; }
};
