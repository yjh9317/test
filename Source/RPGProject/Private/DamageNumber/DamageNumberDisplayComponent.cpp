// Fill out your copyright notice in the Description page of Project Settings.


#include "DamageNumber/DamageNumberDisplayComponent.h"
#include "Components/WidgetComponent.h"
#include "DamageNumber/DamageNumberWidget.h"
#include "Kismet/GameplayStatics.h"

// 데미지 넘버 풀 사이즈
static constexpr int32 DAMAGE_NUMBER_POOL_SIZE = 20;

UDamageNumberDisplayComponent::UDamageNumberDisplayComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UDamageNumberDisplayComponent::BeginPlay()
{
    Super::BeginPlay();
    
    // 위젯 풀 초기화
    InitializeWidgetPool();
}

void UDamageNumberDisplayComponent::ShowDamageNumber(const FDamageNumberInfo& DamageInfo)
{
    if (!DamageNumberWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("DamageNumberWidgetClass is not set!"));
        return;
    }

    // 풀에서 위젯 가져오기
    UDamageNumberWidget* DamageWidget = GetPooledDamageNumberWidget();
    if (!DamageWidget)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to get pooled damage widget!"));
        return;
    }

    // 위젯 설정
    DamageWidget->SetDamageInfo(DamageInfo.DamageAmount, DamageInfo.bIsCritical, DamageInfo.TextColor);
    
    // 위젯 컴포넌트 생성 및 설정
    UWidgetComponent* WidgetComp = NewObject<UWidgetComponent>(GetOwner());
    WidgetComp->RegisterComponent();
    WidgetComp->SetWidget(DamageWidget);
    WidgetComp->SetWorldLocation(DamageInfo.WorldLocation);
    WidgetComp->SetDrawSize(FVector2D(200, 100));
    WidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
    
    // 카메라를 향하도록 설정
    if (APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0))
    {
        if (APlayerCameraManager* CameraManager = PC->PlayerCameraManager)
        {
            FVector CameraLocation = CameraManager->GetCameraLocation();
            FRotator LookAtRotation = (CameraLocation - DamageInfo.WorldLocation).Rotation();
            WidgetComp->SetWorldRotation(LookAtRotation);
        }
    }

    // 애니메이션 재생
    DamageWidget->PlayDamageAnimation();
    
    // 활성 데미지 넘버 추가
    FActiveDamageNumber ActiveNumber;
    ActiveNumber.Widget = DamageWidget;
    ActiveNumber.WidgetComponent = WidgetComp;
    ActiveNumber.ElapsedTime = 0.0f;
    ActiveNumber.StartLocation = DamageInfo.WorldLocation;
    ActiveNumbers.Add(ActiveNumber);

    // 완료 콜백 설정
    DamageWidget->OnFinished.BindUObject(this, &UDamageNumberDisplayComponent::HandleWidgetFinished);

    // 틱 활성화
    if (!IsComponentTickEnabled())
    {
        SetComponentTickEnabled(true);
    }
}

void UDamageNumberDisplayComponent::ClearAllDamageNumbers()
{
}

void UDamageNumberDisplayComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    // 활성 데미지 넘버 업데이트
    for (int32 i = ActiveNumbers.Num() - 1; i >= 0; --i)
    {
        FActiveDamageNumber& ActiveNumber = ActiveNumbers[i];
        ActiveNumber.ElapsedTime += DeltaTime;

        // 위로 이동 애니메이션
        if (ActiveNumber.WidgetComponent)
        {
            FVector NewLocation = ActiveNumber.StartLocation + FVector(0, 0, ActiveNumber.ElapsedTime * 100.0f);
            ActiveNumber.WidgetComponent->SetWorldLocation(NewLocation);
        }

        // 시간 초과 체크 (안전장치)
        if (ActiveNumber.ElapsedTime > 3.0f)
        {
            ReturnDamageNumberWidgetToPool(ActiveNumber.Widget);
            if (ActiveNumber.WidgetComponent)
            {
                ActiveNumber.WidgetComponent->DestroyComponent();
            }
            ActiveNumbers.RemoveAt(i);
        }
    }

    // 모든 데미지 넘버가 완료되면 틱 비활성화
    if (ActiveNumbers.Num() == 0)
    {
        SetComponentTickEnabled(false);
    }
}

void UDamageNumberDisplayComponent::InitializeWidgetPool()
{
    if (!DamageNumberWidgetClass)
    {
        UE_LOG(LogTemp, Warning, TEXT("DamageNumberWidgetClass is not set!"));
        return;
    }

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC)
    {
        return;
    }

    // 풀 생성
    for (int32 i = 0; i < DAMAGE_NUMBER_POOL_SIZE; ++i)
    {
        UDamageNumberWidget* Widget = CreateWidget<UDamageNumberWidget>(PC, DamageNumberWidgetClass);
        if (Widget)
        {
            Widget->SetVisibility(ESlateVisibility::Collapsed);
            WidgetPool.Add(Widget);
        }
    }
}

UDamageNumberWidget* UDamageNumberDisplayComponent::GetPooledDamageNumberWidget()
{
    // 사용 가능한 위젯 찾기
    for (UDamageNumberWidget* Widget : WidgetPool)
    {
        if (Widget && Widget->GetVisibility() == ESlateVisibility::Collapsed)
        {
            Widget->SetVisibility(ESlateVisibility::Visible);
            return Widget;
        }
    }

    // 풀이 부족하면 새로 생성
    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC && DamageNumberWidgetClass)
    {
        UDamageNumberWidget* NewWidget = CreateWidget<UDamageNumberWidget>(PC, DamageNumberWidgetClass);
        if (NewWidget)
        {
            WidgetPool.Add(NewWidget);
            return NewWidget;
        }
    }

    return nullptr;
}

void UDamageNumberDisplayComponent::ReturnDamageNumberWidgetToPool(UDamageNumberWidget* WidgetToReturn)
{
    if (WidgetToReturn)
    {
        WidgetToReturn->ResetWidget();
        WidgetToReturn->SetVisibility(ESlateVisibility::Collapsed);
    }
}

void UDamageNumberDisplayComponent::HandleWidgetFinished(UDamageNumberWidget* FinishedWidget)
{
    // 활성 데미지 넘버에서 제거
    for (int32 i = ActiveNumbers.Num() - 1; i >= 0; --i)
    {
        if (ActiveNumbers[i].Widget == FinishedWidget)
        {
            // 위젯 컴포넌트 정리
            if (ActiveNumbers[i].WidgetComponent)
            {
                ActiveNumbers[i].WidgetComponent->DestroyComponent();
            }
            
            // 풀로 반환
            ReturnDamageNumberWidgetToPool(FinishedWidget);
            
            // 배열에서 제거
            ActiveNumbers.RemoveAt(i);
            break;
        }
    }
}

