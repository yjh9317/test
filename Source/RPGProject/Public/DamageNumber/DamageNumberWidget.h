// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DamageNumberWidget.generated.h"

class UDamageNumberWidget;

DECLARE_DELEGATE_OneParam(FOnDamageNumberFinished, UDamageNumberWidget*);
/**
 * 
 */
UCLASS()
class RPGPROJECT_API UDamageNumberWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	// 데미지 정보 설정
	UFUNCTION(BlueprintCallable, Category = "DamageNumber")
	void SetDamageInfo(float Damage, bool bIsCritical, const FLinearColor& Color);

	// 애니메이션 재생
	UFUNCTION(BlueprintImplementableEvent, Category = "DamageNumber")
	void PlayDamageAnimation();

	// 애니메이션 완료 콜백
	UFUNCTION(BlueprintCallable, Category = "DamageNumber")
	void OnDamageAnimationFinished();
	
	// 위젯 리셋
	UFUNCTION(BlueprintCallable, Category = "DamageNumber")
	void ResetWidget();

	// 완료 델리게이트
	FOnDamageNumberFinished OnFinished;

protected:
	// 데미지 텍스트 (블루프린트에서 바인딩)
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DamageText;

	UPROPERTY(BlueprintReadOnly, Category = "DamageNumber")
	float DamageAmount;
	
	UPROPERTY(BlueprintReadOnly, Category = "DamageNumber")
	bool bIsCriticalHit;
	
	UPROPERTY(BlueprintReadOnly, Category = "DamageNumber")
	FLinearColor TextColor;
};
