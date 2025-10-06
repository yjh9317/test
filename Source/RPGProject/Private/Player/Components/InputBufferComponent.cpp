// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/InputBufferComponent.h"
#include "Header/DebugHeader.h"
#include "Interface/ComponentManager.h"
#include "Shared/Action/BaseAction.h"


UInputBufferComponent::UInputBufferComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.TickInterval = 0.05f;
}

bool UInputBufferComponent::HasBufferedAction(const FGameplayTag& ActionTag) const
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    
    for (const FBufferedInput& Input : InputBuffer)
    {
        if (Input.ActionTag == ActionTag && !Input.IsExpired(CurrentTime, BufferWindow))
        {
            return true;
        }
    }
    return false;
}

float UInputBufferComponent::GetLastInputTime(const FGameplayTag& ActionTag) const
{
    for (int32 i = InputBuffer.Num() - 1; i >= 0; i--)
    {
        if (InputBuffer[i].ActionTag == ActionTag)
        {
            return InputBuffer[i].Timestamp;
        }
    }
    return -1.0f;
}

bool UInputBufferComponent::ConsumeBufferedInput(const FGameplayTag& ActionTag)
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    
    for (int32 i = InputBuffer.Num() - 1; i >= 0; i--)
    {
        if (InputBuffer[i].ActionTag == ActionTag && 
            !InputBuffer[i].IsExpired(CurrentTime, BufferWindow))
        {
            InputBuffer.RemoveAt(i);
            return true;
        }
    }
    return false;
}

FBufferedInput UInputBufferComponent::GetLastBufferedInput(const FGameplayTag& ActionTag) const
{
    for (int32 i = InputBuffer.Num() - 1; i >= 0; i--)
    {
        if (InputBuffer[i].ActionTag == ActionTag)
            return InputBuffer[i];
    }
    return FBufferedInput();
}

bool UInputBufferComponent::HasRecentInput(const FGameplayTag& ActionTag, float TimeWindow) const
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    
    for (const FBufferedInput& Input : InputBuffer)
    {
        if (Input.ActionTag == ActionTag)
        {
            float TimeSinceInput = CurrentTime - Input.Timestamp;
            if (TimeSinceInput <= TimeWindow)
                return true;
        }
    }
    return false;
}

void UInputBufferComponent::BeginPlay()
{
    Super::BeginPlay();
    
    if (DefaultMappingData)
    {
        InitializeFromDataAsset(DefaultMappingData);
    }

    APlayerController* MyPlayerController = Cast<APlayerController>(GetOwner());
    APawn* ControlledPawn = MyPlayerController->GetPawn();
    if(ControlledPawn)
    {
        IComponentManager* ComponentManager = Cast<IComponentManager>(ControlledPawn); 
        if(ComponentManager)
        {
            CachedActionComponent = ComponentManager->GetActionComponent();    
        }
    }
}

void UInputBufferComponent::TickComponent(float DeltaTime, ELevelTick TickType, 
                                         FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    
    CleanExpiredInputs();

    if (HasBufferedInput())
    {
        ProcessNextBufferedInput();
    }
}

void UInputBufferComponent::BufferInput(const FGameplayTag& InputTag, 
                                       const FVector& InputDir, float Magnitude)
{
    FGameplayTag ActionTag = InputTag;
    FGameplayTag* MappedAction = InputToActionMapping.Find(InputTag);
    if (!MappedAction)
    {
        return;
    }
    ActionTag = *MappedAction;
    
    if (ShouldExecuteImmediately(ActionTag))
    {
        CachedActionComponent->ExecuteAction(ActionTag);
        return;
    }
    
    FBufferedInput NewInput;
    NewInput.ActionTag = InputTag;  
    NewInput.MappedActionTag = ActionTag;
    NewInput.Timestamp = GetWorld()->GetTimeSeconds();
    NewInput.InputDirection = InputDir;
    NewInput.InputMagnitude = Magnitude;
    InputBuffer.Add(NewInput);

    while (InputBuffer.Num() > MaxBufferSize)
    {
       InputBuffer.RemoveAt(0);
    }
}

bool UInputBufferComponent::ProcessNextBufferedInput()
{
    if (!CachedActionComponent || InputBuffer.Num() == 0)
        return false;
    
    for (int32 i = InputBuffer.Num() - 1; i >= 0; i--)
    {
        if (TryExecuteAction(InputBuffer[i]))
        {
            InputBuffer.RemoveAt(i);
            return true;
        }
    }
    
    return false;
}


bool UInputBufferComponent::TryExecuteAction(const FBufferedInput& Input)
{
    if (!CachedActionComponent)
        return false;

    FGameplayTag ActionToExecute = Input.MappedActionTag.IsValid() ? Input.MappedActionTag : Input.ActionTag;
    
    // if (CachedActionComponent->IsActionActive(ActionToExecute))
    // {
    //     return false;
    // }

    if (UBaseAction* Action = CachedActionComponent->GetAction(ActionToExecute))
    {
        return CachedActionComponent->ExecuteAction(ActionToExecute);
    }
    
    return false;
}



bool UInputBufferComponent::ShouldExecuteImmediately(const FGameplayTag& ActionTag) const
{
    if (!CachedActionComponent)
        return false;
    
    // 동일한 액션이 이미 활성이면 버퍼링만
    UBaseAction* Action = CachedActionComponent->GetAction(ActionTag);
    if (Action && Action->IsActive())
        return false;
    
    return CachedActionComponent->GetActiveActionCount() == 0;
}

void UInputBufferComponent::InitializeFromDataAsset(UInputMappingData* MappingData)
{
    if (!MappingData)
        return;
    
    InputToActionMapping = MappingData->InputToActionMap;
}

void UInputBufferComponent::AddInputMapping(const FGameplayTag& InputTag, const FGameplayTag& ActionTag)
{
    if (!InputTag.IsValid() || !ActionTag.IsValid())
        return;
    
    InputToActionMapping.Add(InputTag, ActionTag);
}

void UInputBufferComponent::RemoveInputMapping(const FGameplayTag& InputTag)
{
    if (InputToActionMapping.Remove(InputTag) > 0)
    {
        UE_LOG(LogTemp, Log, TEXT("Removed mapping for: %s"), *InputTag.ToString());
    }
}

void UInputBufferComponent::ClearAllMappings()
{
    InputToActionMapping.Empty();
}

void UInputBufferComponent::SwapMappingData(UInputMappingData* NewMappingData)
{
    if (!NewMappingData)
        return;
    
    ClearAllMappings();
    
    InitializeFromDataAsset(NewMappingData);
}

FGameplayTag UInputBufferComponent::GetMappedAction(const FGameplayTag& InputTag) const
{
    if (const FGameplayTag* MappedTag = InputToActionMapping.Find(InputTag))
    {
        return *MappedTag;
    }
    return InputTag; // 매핑 없으면 원본 반환
}

void UInputBufferComponent::CleanExpiredInputs()
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    
    InputBuffer.RemoveAll([CurrentTime, this](const FBufferedInput& Input)
    {
        return Input.IsExpired(CurrentTime, BufferWindow);
    });
}


// void UInputBufferComponent::BufferInputWithParams(const FGameplayTag& ActionTag,
//                                                  const TMap<FName, float>& FloatParams,
//                                                  const TMap<FName, FVector>& VectorParams)
// {
//     FBufferedInput NewInput;
//     NewInput.ActionTag = ActionTag;
//     NewInput.Timestamp = GetWorld()->GetTimeSeconds();
//     NewInput.FloatParams = FloatParams;
//     NewInput.VectorParams = VectorParams;
//  
//     if (const FVector* DirPtr = VectorParams.Find("InputDirection"))
//     {
//         NewInput.InputDirection = *DirPtr;
//     }
//     
//     InputBuffer.Add(NewInput);
//     
//     while (InputBuffer.Num() > MaxBufferSize)
//     {
//         InputBuffer.RemoveAt(0);
//     }
// }
