#pragma once

#include "CoreMinimal.h"
#include "Engine/Engine.h"
#include "Engine/World.h"

// =============================================================================
// 1. 안전성 체크 매크로
// =============================================================================

// Null 체크 후 리턴
#define RETURN_IF_NULL(Pointer) \
    do { \
        if (!IsValid(Pointer)) \
        { \
            UE_LOG(LogTemp, Warning, TEXT("%s: %s is null"), TEXT(__FUNCTION__), TEXT(#Pointer)); \
            return; \
        } \
    } while(0)

// Null 체크 후 값과 함께 리턴
#define RETURN_VALUE_IF_NULL(Pointer, ReturnValue) \
    do { \
        if (!IsValid(Pointer)) \
        { \
            UE_LOG(LogTemp, Warning, TEXT("%s: %s is null"), TEXT(__FUNCTION__), TEXT(#Pointer)); \
            return ReturnValue; \
        } \
    } while(0)

// 조건 체크 후 리턴 
#define RETURN_IF_FALSE(Condition) \
    do { \
        if (!(Condition)) \
        { \
            UE_LOG(LogTemp, Warning, TEXT("%s: Condition failed: %s"), TEXT(__FUNCTION__), TEXT(#Condition)); \
            return; \
        } \
    } while(0)

// 배열 인덱스 유효성 체크
#define RETURN_IF_INVALID_INDEX(Array, Index) \
    do { \
        if (!Array.IsValidIndex(Index)) \
        { \
            UE_LOG(LogTemp, Warning, TEXT("%s: Invalid index %d for array (Size: %d)"), \
                   TEXT(__FUNCTION__), Index, Array.Num()); \
            return; \
        } \
    } while(0)

// =============================================================================
// 2. 네트워킹 매크로
// =============================================================================

#define IS_AUTHORITY() (GetOwnerRole() == ROLE_Authority)
#define IS_CLIENT() (GetOwnerRole() == ROLE_AutonomousProxy || GetOwnerRole() == ROLE_SimulatedProxy)
#define IS_LOCALLY_CONTROLLED() (GetOwnerRole() <= ROLE_AutonomousProxy)
#define IS_SIMULATED_PROXY() (GetOwnerRole() == ROLE_SimulatedProxy)

// 안전한 HasAuthority 체크 (Owner null 체크 포함)
#define HAS_AUTHORITY_SAFE() \
    (IsValid(GetOwner()) && GetOwner()->HasAuthority())

#define IS_REMOTE_SAFE() \
    (IsValid(GetOwner()) && !GetOwner()->HasAuthority())


// =============================================================================
// 3. 디버깅 매크로
// =============================================================================

// 개발 빌드에서만 로그 출력
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
    #define DEBUG_LOG(Format, ...) \
        UE_LOG(LogTemp, Log, TEXT("[%s:%d] ") Format, TEXT(__FUNCTION__), __LINE__, ##__VA_ARGS__)
    
    #define DEBUG_WARNING(Format, ...) \
        UE_LOG(LogTemp, Warning, TEXT("[%s:%d] ") Format, TEXT(__FUNCTION__), __LINE__, ##__VA_ARGS__)
    
    #define DEBUG_ERROR(Format, ...) \
        UE_LOG(LogTemp, Error, TEXT("[%s:%d] ") Format, TEXT(__FUNCTION__), __LINE__, ##__VA_ARGS__)
#else
    #define DEBUG_LOG(Format, ...)
    #define DEBUG_WARNING(Format, ...)  
    #define DEBUG_ERROR(Format, ...)
#endif

// 화면에 디버그 메시지 출력 (개발 빌드만)
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
    #define SCREEN_DEBUG(Format, ...) \
        do { \
            if (GEngine) \
            { \
                GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Yellow, \
                    FString::Printf(TEXT("[%s] ") Format, TEXT(__FUNCTION__), ##__VA_ARGS__)); \
            } \
        } while(0)
    
    #define SCREEN_DEBUG_COLOR(Color, Format, ...) \
        do { \
            if (GEngine) \
            { \
                GEngine->AddOnScreenDebugMessage(-1, 3.0f, Color, \
                    FString::Printf(TEXT("[%s] ") Format, TEXT(__FUNCTION__), ##__VA_ARGS__)); \
            } \
        } while(0)
#else
    #define SCREEN_DEBUG(Format, ...)
    #define SCREEN_DEBUG_COLOR(Color, Format, ...)
#endif

// =============================================================================
// 4. 성능 측정 매크로  
// =============================================================================

// 함수 실행 시간 측정 (개발 빌드만)
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
    #define SCOPED_TIMER(TimerName) \
        FScopedDurationTimer Timer_##TimerName([](double Duration) { \
            UE_LOG(LogTemp, Log, TEXT("%s: %s took %.4f ms"), \
                   TEXT(__FUNCTION__), TEXT(#TimerName), Duration * 1000.0); \
        })
    
    #define MEASURE_FUNCTION_TIME() \
        FScopedDurationTimer FunctionTimer([](double Duration) { \
            UE_LOG(LogTemp, Log, TEXT("%s took %.4f ms"), \
                   TEXT(__FUNCTION__), Duration * 1000.0); \
        })
#else
    #define SCOPED_TIMER(TimerName)
    #define MEASURE_FUNCTION_TIME()
#endif

// =============================================================================
// 5. 편의성 매크로
// =============================================================================

// 안전한 델리게이트 실행
#define SAFE_EXECUTE_DELEGATE(Delegate, ...) \
    do { \
        if (Delegate.IsBound()) \
        { \
            Delegate.ExecuteIfBound(__VA_ARGS__); \
        } \
    } while(0)

// 안전한 멀티캐스트 델리게이트 실행
#define SAFE_BROADCAST_DELEGATE(Delegate, ...) \
    do { \
        if (Delegate.IsBound()) \
        { \
            Delegate.Broadcast(__VA_ARGS__); \
        } \
    } while(0)

// 컴포넌트 안전 획득
#define GET_COMPONENT_SAFE(ComponentClass, VariableName) \
    ComponentClass* VariableName = GetOwner() ? GetOwner()->FindComponentByClass<ComponentClass>() : nullptr; \
    if (!IsValid(VariableName)) \
    { \
        UE_LOG(LogTemp, Warning, TEXT("%s: Failed to find component %s"), \
               TEXT(__FUNCTION__), TEXT(#ComponentClass)); \
        return; \
    }

// 컴포넌트 안전 획득 (값 리턴 버전)
#define GET_COMPONENT_SAFE_RET(ComponentClass, VariableName, ReturnValue) \
    ComponentClass* VariableName = GetOwner() ? GetOwner()->FindComponentByClass<ComponentClass>() : nullptr; \
    if (!IsValid(VariableName)) \
    { \
        UE_LOG(LogTemp, Warning, TEXT("%s: Failed to find component %s"), \
               TEXT(__FUNCTION__), TEXT(#ComponentClass)); \
        return ReturnValue; \
    }

// 월드 안전 획득
#define GET_WORLD_SAFE(VariableName) \
    UWorld* VariableName = GetWorld(); \
    if (!IsValid(VariableName)) \
    { \
        UE_LOG(LogTemp, Warning, TEXT("%s: World is null"), TEXT(__FUNCTION__)); \
        return; \
    }

// 게임 인스턴스 안전 획득
#define GET_GAME_INSTANCE_SAFE(GameInstanceClass, VariableName) \
    GameInstanceClass* VariableName = GetWorld() ? Cast<GameInstanceClass>(GetWorld()->GetGameInstance()) : nullptr; \
    if (!IsValid(VariableName)) \
    { \
        UE_LOG(LogTemp, Warning, TEXT("%s: Failed to get GameInstance"), TEXT(__FUNCTION__)); \
        return; \
    }

// =============================================================================
// 6. 타이머 관련 매크로
// =============================================================================

// 안전한 타이머 설정
#define SET_TIMER_SAFE(TimerHandle, Object, Function, Rate, bLoop) \
    do { \
        UWorld* World = Object ? Object->GetWorld() : nullptr; \
        if (IsValid(World)) \
        { \
            World->GetTimerManager().SetTimer(TimerHandle, Object, Function, Rate, bLoop); \
        } \
    } while(0)

// 안전한 타이머 해제
#define CLEAR_TIMER_SAFE(TimerHandle, Object) \
    do { \
        UWorld* World = Object ? Object->GetWorld() : nullptr; \
        if (IsValid(World) && TimerHandle.IsValid()) \
        { \
            World->GetTimerManager().ClearTimer(TimerHandle); \
        } \
    } while(0)

// =============================================================================
// 7. 블루프린트 연동 매크로
// =============================================================================

// 블루프린트 함수 안전 호출
#define CALL_BP_FUNCTION_SAFE(Object, FunctionName, ...) \
    do { \
        if (IsValid(Object)) \
        { \
            Object->FunctionName(__VA_ARGS__); \
        } \
        else \
        { \
            UE_LOG(LogTemp, Warning, TEXT("%s: Cannot call %s - Object is null"), \
                   TEXT(__FUNCTION__), TEXT(#FunctionName)); \
        } \
    } while(0)

// =============================================================================
// 8. 수학 관련 매크로
// =============================================================================

// 안전한 0으로 나누기 방지
#define SAFE_DIVIDE(Numerator, Denominator, DefaultValue) \
    (FMath::IsNearlyZero(Denominator) ? DefaultValue : (Numerator / Denominator))

// 벡터 거리 체크 (제곱근 연산 없이)
#define IS_WITHIN_DISTANCE_SQUARED(Location1, Location2, DistanceSquared) \
    (FVector::DistSquared(Location1, Location2) <= (DistanceSquared * DistanceSquared))

// =============================================================================
// 9. 메모리 관리 매크로
// =============================================================================

// 안전한 객체 소멸
#define SAFE_DESTROY(Object) \
    do { \
        if (IsValid(Object)) \
        { \
            Object->Destroy(); \
            Object = nullptr; \
        } \
    } while(0)

// TArray 안전한 요소 제거
#define SAFE_REMOVE_FROM_ARRAY(Array, Element) \
    do { \
        int32 Index = Array.Find(Element); \
        if (Index != INDEX_NONE) \
        { \
            Array.RemoveAt(Index); \
        } \
    } while(0)

// =============================================================================
// 사용 예시 주석
// =============================================================================

/*
// 예시 1: 기본 안전성 체크
void UMyComponent::DoSomething()
{
    RETURN_IF_NULL(GetOwner());
    GET_WORLD_SAFE(World);
    
    DEBUG_LOG(TEXT("Doing something..."));
    MEASURE_FUNCTION_TIME();
    
    // 로직 구현...
}

// 예시 2: 네트워킹
void UMyComponent::ServerFunction()
{
    SERVER_ONLY();
    
    SCREEN_DEBUG(TEXT("Server function called"));
    // 서버 로직...
}

// 예시 3: 컴포넌트 안전 획득
void UMyComponent::ProcessHealth()
{
    GET_COMPONENT_SAFE(UHealthComponent, HealthComp);
    
    float CurrentHealth = HealthComp->GetCurrentHealth();
    DEBUG_LOG(TEXT("Current Health: %.2f"), CurrentHealth);
}

// 예시 4: 타이머 사용
void UMyComponent::StartRegeneration()
{
    SET_TIMER_SAFE(RegenTimerHandle, this, &UMyComponent::RegenerateHealth, 1.0f, true);
}

// 예시 5: 델리게이트 안전 실행
void UMyComponent::NotifyHealthChanged(float NewHealth)
{
    SAFE_BROADCAST_DELEGATE(OnHealthChangedDelegate, NewHealth);
}
*/