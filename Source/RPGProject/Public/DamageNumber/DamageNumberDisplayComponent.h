// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageNumberDisplayComponent.generated.h"

class UObjectPoolComponent;
class UDamageNumberWidget;
class UWidgetComponent;

USTRUCT(BlueprintType)
struct FDamageNumberInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageNumber")
	float DamageAmount = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageNumber")
	FVector WorldLocation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageNumber")
	bool bIsCritical = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageNumber")
	FLinearColor TextColor = FLinearColor::White;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageNumber")
	float TextScale = 1.0f;
};

USTRUCT()
struct FActiveDamageNumber
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<UDamageNumberWidget> Widget = nullptr;

	UPROPERTY()
	TObjectPtr<UWidgetComponent> WidgetComponent = nullptr;

	float ElapsedTime = 0.0f;
	FVector StartLocation = FVector::ZeroVector;
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UDamageNumberDisplayComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UDamageNumberDisplayComponent();

	// 데미지 숫자 표시
	UFUNCTION(BlueprintCallable, Category = "DamageNumber")
	void ShowDamageNumber(const FDamageNumberInfo& DamageInfo);

	// 모든 활성 데미지 넘버 제거
	UFUNCTION(BlueprintCallable, Category = "DamageNumber")
	void ClearAllDamageNumbers();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 위젯 클래스
	UPROPERTY(EditDefaultsOnly, Category = "DamageNumber")
	TSubclassOf<UDamageNumberWidget> DamageNumberWidgetClass;

	// 동시에 표시할 수 있는 최대 데미지 넘버 수
	UPROPERTY(EditDefaultsOnly, Category = "DamageNumber", meta = (ClampMin = "1", ClampMax = "50"))
	int32 MaxActiveDamageNumbers = 10;

	// 데미지 넘버 표시 시간
	UPROPERTY(EditDefaultsOnly, Category = "DamageNumber", meta = (ClampMin = "0.1"))
	float DamageNumberLifetime = 2.0f;

	// 데미지 넘버 이동 속도
	UPROPERTY(EditDefaultsOnly, Category = "DamageNumber")
	float FloatSpeed = 100.0f;

	// 크리티컬 히트 스케일 배율
	UPROPERTY(EditDefaultsOnly, Category = "DamageNumber", meta = (ClampMin = "1.0"))
	float CriticalScaleMultiplier = 1.5f;

private:
	// 위젯 풀
	UPROPERTY()
	TArray<TObjectPtr<UDamageNumberWidget>> WidgetPool;

	// 활성 데미지 넘버들
	UPROPERTY()
	TArray<FActiveDamageNumber> ActiveNumbers;

	// 위젯 풀 초기화
	void InitializeWidgetPool();

	// 풀에서 위젯 가져오기
	UDamageNumberWidget* GetPooledDamageNumberWidget();

	// 위젯을 풀로 반환
	void ReturnDamageNumberWidgetToPool(UDamageNumberWidget* WidgetToReturn);

	// 위젯 완료 핸들러
	void HandleWidgetFinished(UDamageNumberWidget* FinishedWidget);
};
