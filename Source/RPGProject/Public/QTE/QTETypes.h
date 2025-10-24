#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Widgets/Layout/Anchors.h"
#include "InputCoreTypes.h"
#include "Engine/Texture2D.h"
#include "QTETypes.generated.h"


// QTE 타입
UENUM(BlueprintType)
enum class EQTEType : uint8
{
    SingleKey       UMETA(DisplayName = "Single Key"),        // 단일 키 입력
    Sequence        UMETA(DisplayName = "Sequence"),          // 순서대로 입력
    Rapid           UMETA(DisplayName = "Rapid"),             // 연타
    Hold            UMETA(DisplayName = "Hold"),              // 길게 누르기
    Direction       UMETA(DisplayName = "Direction"),         // 방향키
    Combo           UMETA(DisplayName = "Combo"),             // 조합키
    Timing          UMETA(DisplayName = "Timing")             // 타이밍 맞추기
};

// QTE 상태
UENUM(BlueprintType)
enum class EQTEStatus : uint8
{
    Inactive        UMETA(DisplayName = "Inactive"),
    Active          UMETA(DisplayName = "Active"),
    Success         UMETA(DisplayName = "Success"),
    Failed          UMETA(DisplayName = "Failed"),
    Timeout         UMETA(DisplayName = "Timeout"),
    Perfect         UMETA(DisplayName = "Perfect")
};

// QTE 난이도
UENUM(BlueprintType)
enum class EQTEDifficulty : uint8
{
    Easy            UMETA(DisplayName = "Easy"),
    Normal          UMETA(DisplayName = "Normal"),
    Hard            UMETA(DisplayName = "Hard"),
    Expert          UMETA(DisplayName = "Expert")
};

// QTE 결과
USTRUCT(BlueprintType)
struct FQTEResult
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    EQTEStatus Status = EQTEStatus::Inactive;

    UPROPERTY(BlueprintReadOnly)
    float Accuracy = 0.0f; // 0.0 ~ 1.0

    UPROPERTY(BlueprintReadOnly)
    float TimingScore = 0.0f; // 타이밍 점수

    UPROPERTY(BlueprintReadOnly)
    int32 CorrectInputs = 0;

    UPROPERTY(BlueprintReadOnly)
    int32 TotalInputs = 0;

    UPROPERTY(BlueprintReadOnly)
    float CompletionTime = 0.0f;

    UPROPERTY(BlueprintReadOnly)
    bool bPerfectExecution = false;

    UPROPERTY(BlueprintReadOnly)
    float ScoreMultiplier = 1.0f;

    FQTEResult()
    {
        Status = EQTEStatus::Inactive;
        Accuracy = 0.0f;
        TimingScore = 0.0f;
        CorrectInputs = 0;
        TotalInputs = 0;
        CompletionTime = 0.0f;
        bPerfectExecution = false;
        ScoreMultiplier = 1.0f;
    }
};

// QTE 정보 구조체
USTRUCT(BlueprintType)
struct FQTEInfo : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag QTETag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EQTEType QTEType = EQTEType::SingleKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EQTEDifficulty Difficulty = EQTEDifficulty::Normal;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FKey> RequiredKeys;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float Duration = 3.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float PerfectWindow = 0.2f; // Perfect 타이밍 윈도우

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float GoodWindow = 0.5f; // Good 타이밍 윈도우

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RequiredInputCount = 1; // Rapid 타입용

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float HoldDuration = 2.0f; // Hold 타입용

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bAllowEarlyInput = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bShowVisualCues = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FVector2D ScreenPosition = FVector2D(0.5f, 0.5f);

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DisplayText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TObjectPtr<UTexture2D> BackgroundImage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor PrimaryColor = FLinearColor::White;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FLinearColor SecondaryColor = FLinearColor::Gray;

    FQTEInfo()
    {
        QTETag = FGameplayTag();
        QTEType = EQTEType::SingleKey;
        Difficulty = EQTEDifficulty::Normal;
        Duration = 3.0f;
        PerfectWindow = 0.2f;
        GoodWindow = 0.5f;
        RequiredInputCount = 1;
        HoldDuration = 2.0f;
        bAllowEarlyInput = false;
        bShowVisualCues = true;
        ScreenPosition = FVector2D(0.5f, 0.5f);
        DisplayText = FText::GetEmpty();
        BackgroundImage = nullptr;
        PrimaryColor = FLinearColor::White;
        SecondaryColor = FLinearColor::Gray;
    }
};

// QTE 설정
USTRUCT(BlueprintType)
struct FQTESettings
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bAutoStartOnTrigger = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bPauseGameDuringQTE = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bAllowRetry = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxRetries = 3;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RetryDelay = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bShowProgressBar = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bShowAccuracy = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bVibrateOnInput = true;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float VibrationIntensity = 0.5f;

    FQTESettings()
    {
        bAutoStartOnTrigger = true;
        bPauseGameDuringQTE = true;
        bAllowRetry = false;
        MaxRetries = 3;
        RetryDelay = 1.0f;
        bShowProgressBar = true;
        bShowAccuracy = true;
        bVibrateOnInput = true;
        VibrationIntensity = 0.5f;
    }
};
