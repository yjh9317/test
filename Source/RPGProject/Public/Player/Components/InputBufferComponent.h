// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Shared/Action/Components/ActionComponent.h"
#include "Shared/Data/InputMappingData.h"
#include "InputBufferComponent.generated.h"

USTRUCT(BlueprintType)
struct FBufferedInput
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag ActionTag;  // 원본 입력 태그
    
	UPROPERTY()
	FGameplayTag MappedActionTag;  // 매핑된 액션 태그
    
	UPROPERTY()
	float Timestamp = 0.f;
    
	UPROPERTY()
	FVector InputDirection = FVector::ZeroVector;
    
	UPROPERTY()
	float InputMagnitude = 0.f;
    
	// 추가 컨텍스트 데이터
	UPROPERTY()
	TMap<FName, float> FloatParams;
    
	UPROPERTY()
	TMap<FName, FVector> VectorParams;
    
	bool IsExpired(float CurrentTime, float BufferWindow) const
	{
		return (CurrentTime - Timestamp) > BufferWindow;
	}
};

// 특수 커맨드 정의
USTRUCT(BlueprintType)
struct FCommandPattern
{
	GENERATED_BODY()
    
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag ResultActionTag;
    
	UPROPERTY(EditDefaultsOnly)
	TArray<FGameplayTag> InputSequence;
    
	UPROPERTY(EditDefaultsOnly)
	float MaxInputInterval = 0.5f; // 입력 간 최대 간격
    
	UPROPERTY(EditDefaultsOnly)
	bool bRequireExactOrder = true;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UInputBufferComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInputBufferComponent();

	UFUNCTION(BlueprintPure, Category = "Input Buffer")
	bool HasBufferedAction(const FGameplayTag& ActionTag) const;
    
	UFUNCTION(BlueprintPure, Category = "Input Buffer")
	float GetLastInputTime(const FGameplayTag& ActionTag) const;
    
	UFUNCTION(BlueprintCallable, Category = "Input Buffer")
	bool ConsumeBufferedInput(const FGameplayTag& ActionTag);
    
	UFUNCTION(BlueprintPure, Category = "Input Buffer")
	FBufferedInput GetLastBufferedInput(const FGameplayTag& ActionTag) const;
	
	UFUNCTION(BlueprintPure, Category = "Input Buffer")
	bool HasRecentInput(const FGameplayTag& ActionTag, float TimeWindow) const;

    UPROPERTY(EditDefaultsOnly, Category = "Input Buffer")
    float BufferWindow = 0.3f;
    
    UPROPERTY(EditDefaultsOnly, Category = "Input Buffer")
    int32 MaxBufferSize = 10;
private:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly,meta=(AllowPrivateAccess="true"))
    TArray<FBufferedInput> InputBuffer;
    
    UPROPERTY()
    class UActionComponent* CachedActionComponent;
	
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping")
	TMap<FGameplayTag, FGameplayTag> InputToActionMapping;
    
	UPROPERTY(EditDefaultsOnly, Category = "Input Mapping")
	UInputMappingData* DefaultMappingData;
public:
    virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;

    // === 입력 버퍼링 ===
    UFUNCTION(BlueprintCallable, Category = "Input Buffer")
    void BufferInput(const FGameplayTag& ActionTag, const FVector& InputDir = FVector::ZeroVector, 
                    float Magnitude = 1.0f);
    
    // UFUNCTION(BlueprintCallable, Category = "Input Buffer")
    // void BufferInputWithParams(const FGameplayTag& ActionTag, const TMap<FName, float>& FloatParams,
    //                           const TMap<FName, FVector>& VectorParams);
	
    UFUNCTION(BlueprintCallable, Category = "Input Buffer")
    bool ProcessNextBufferedInput();
	
    
    // === 쿼리 ===
    UFUNCTION(BlueprintPure, Category = "Input Buffer")
    bool HasBufferedInput() const { return InputBuffer.Num() > 0; }
    
    UFUNCTION(BlueprintPure, Category = "Input Buffer")
    int32 GetBufferSize() const { return InputBuffer.Num(); }
    
	UFUNCTION(BlueprintCallable, Category = "Input Buffer")
	bool ShouldExecuteImmediately(const FGameplayTag& ActionTag) const;
	
public:
	// 초기화
	UFUNCTION(BlueprintCallable, Category = "Input Mapping")
	void InitializeFromDataAsset(UInputMappingData* MappingData);
    
	// 런타임 수정
	UFUNCTION(BlueprintCallable, Category = "Input Mapping")
	void AddInputMapping(const FGameplayTag& InputTag, const FGameplayTag& ActionTag);
    
	UFUNCTION(BlueprintCallable, Category = "Input Mapping")
	void RemoveInputMapping(const FGameplayTag& InputTag);
    
	UFUNCTION(BlueprintCallable, Category = "Input Mapping")
	void ClearAllMappings();
    
	UFUNCTION(BlueprintCallable, Category = "Input Mapping")
	void SwapMappingData(UInputMappingData* NewMappingData);
    
	UFUNCTION(BlueprintPure, Category = "Input Mapping")
	FGameplayTag GetMappedAction(const FGameplayTag& InputTag) const;
	

private:
    void CleanExpiredInputs();
    bool TryExecuteAction(const FBufferedInput& Input);

    // FBaseActionContext CreateContextFromInput(const FBufferedInput& Input) const;
};
