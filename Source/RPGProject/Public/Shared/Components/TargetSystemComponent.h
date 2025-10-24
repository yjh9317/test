// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "TargetSystemComponent.generated.h"

class AMainCharacter;

UENUM(BlueprintType)
enum class ERotationAxisMode : uint8
{
	ETA_YAW UMETA(Display = Yaw),
	ETA_ROLLANDYAW UMETA(Display = RollAndYaw)
};



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UTargetSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UTargetSystemComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//Get Angle between vectors
	UFUNCTION(BlueprintGetter, Category = "Get Angle Between Vectors")
	static float GetAngleBetweenVectors_UsingDeltaRotation(FVector InStart, FVector InTarget, FRotator InRotationToCheckAgainst);
	
	UFUNCTION(BlueprintGetter, Category = "Get Angle Between Vectors")
	static float GetAngleBetweenVectors_UsingArcTanDifference(FVector InStart, FVector InTarget, FVector InDirectionVector);

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "1ization")
	bool bInitialize = true;
	
	UPROPERTY()
	AMainCharacter* OwnerCharacter;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Actors|Target")
	AActor* ActorToTarget;

	UPROPERTY(BlueprintReadOnly, Category = "Lock On Variables")
	bool bIsLockOnEnabled = false;

	UPROPERTY(BlueprintReadOnly, Category = TickGates)
	bool bOpenGateForTick = false;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Perception Variables")
	bool AutoSwitchTargetsOnPerceptionFailure = true;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly,  Category = "Perception Variables")
	bool AutoSwitchTargetsOnActorToTargetInvalid = true;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly,  Category = "Lock on Variables")
	float InterpolationSpeed = 10;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Perception Variables", meta=(ToolTip = "This attempts to make sure that the current target is not part of the resulting sorted array"))
	float ErrorTolerance_Left = 5;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Perception Variables", meta=(ToolTip = "This attempts to make sure that the current target is not part of the resulting sorted array"))
	float ErrorTolerance_Right = 5;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = StorePerceivedActors)
	void StorePerceivedActorsInArray();

	// UFUNCTION(BlueprintCallable, Category = StorePerceivedActors)
	// void ResetAllArrays();

	UFUNCTION(BlueprintCallable, Category = FilterPerceivedActorsForTarget)
	void DetermineActorWithTheLeastActorRotationAngleRequiredToTarget();
	
	UFUNCTION(BlueprintCallable, Category = FilterPerceivedActorsForTarget)
	void DetermineActorToTargetFromLowestRotationAngleArray();


	UFUNCTION(BlueprintCallable, Category = SwitchTargets)
	void SwitchTargets(bool bSwitchTarget);
	
	//switch to target to the left of the current targeted actor
	UFUNCTION( BlueprintCallable, Category = SwitchTargets)
	void SwitchToTheTargetToTheLeftOfCurrentTarget();
	
	//switch to target to the right of the current targeted actor
	UFUNCTION(BlueprintCallable, Category = SwitchTargets)
	void SwitchToTheTargetToTheRightOfCurrentTarget();

	UFUNCTION(BlueprintCallable, Category = HideOrDisplayWidget)
	void DisplayWidgetForLockedOnTarget();

	UFUNCTION(BlueprintCallable, Category = HideOrDisplayWidget)
	bool ShouldShowWidget();
	
	//timers
	UFUNCTION(BlueprintCallable, Category = StorePerceivedActors)
	void Timer_StorePerceivedActorInArray();

	UFUNCTION(BlueprintCallable, Category = StorePerceivedActors)
	void ClearTimer_StorePerceivedActorsInArray();

	//Hide Widget For Target Not Being Locked On To, So Only The  Target Being Locked On To Displays The 3D Widget
	UFUNCTION(BlueprintCallable, Category = "Lock On Variables|UI")
	void ShowWidgetForTargetNotBeingLockOnTo() const;

	UFUNCTION(BlueprintCallable, Category = "Lock-On")
	void LockOnToTarget(float DeltaTime);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,  Category = "Actors|Perception", meta=(ToolTip = "Array to store all perceived actors"))
	TArray<AActor*> AllTargetActors;
;

	//should 3D widget in character be shown to indicate which character is locked-on to?
	UPROPERTY(BlueprintReadOnly,  EditDefaultsOnly, Category = "Lock On Variables|UI", meta=(ToolTip = "Should 3D widget in character be shown to indicate which character is locked-on to?"))
	bool ShouldShowWidgetForLockOn = true;
	
	//store each perceived actor's angle difference between the owning actor and the perceived actor
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,  Category = "PerceptionVariables|AngleArrays", meta=(ToolTip = "Array to store actor's Angle between Enemy and Player"))
	TArray<float> LowestRotationAngle_Array;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly,  Category = "PerceptionVariables|AngleArrays", meta=(ToolTip = "Array to store actor's Distance between Enemy and Player"))
	TArray<float> LowestDistance_Array;

	//non-replicated variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PerceptionVariables|AngleArrays|Left")
	TArray<float> LowestRotationAngle_Left_Array;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PerceptionVariables|AngleArrays|Right")
	TArray<float> LowestRotationAngle_Right_Array;

	//timers
	//delay between input and actual lock-on
	UPROPERTY(BlueprintReadOnly,  EditDefaultsOnly, Category = "Lock On Variables", meta=(ToolTip = "Delay between input and actual lock-on"))
	float DelayBeforeLockOn = 0.2;
	
	//How quickly should actors perceived be stored?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = PerceptionVariables, meta=(ToolTip = "How quickly should actors perceived be stored?"))
	float TimerIntervalForStoringPerceivedActorsInArray = 0.1;

	//Loop the timer or just store the perceived actors once?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = PerceptionVariables, meta=(ToolTip = "Loop the timer or just store the perceived actors once?"))
	bool bLoopTimerForStoringPerceivedActorsInArray = true;

	//timer handle for storing perceived actors in array - used to clear timer if timer should be stopped
	FTimerHandle TimerHandle_StorePerceivedActors;

	//timer handle for delay before lock-on
	FTimerHandle TimerHandle_DelayBeforeLockOn;


	
	//Sense To Use For Perception - Should be set to the dominant sense being used, or any preferred one
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = PerceptionVariables, meta=(ToolTip ="Sense To Use For Perception - Should be set to the dominant sense being used, or any preferred one"))
	TSubclassOf<UAISense> SenseToUseForPerception;

	//Should the Currently Perceived Actors Be Filtered Into A Particular Class?
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = PerceptionVariables, meta=(ToolTip = "Should the Currently Perceived Actors Be Filtered Into A Particular Class?"))
	bool bFilterPerceivedActorsArray = false;

	//Class to filter perceived actors into if bFilterPerceivedActorsArray is true
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = PerceptionVariables, meta=(ToolTip = "Class to filter perceived actors into if bFilterPerceivedActorsArray is true"))
	TSubclassOf<AActor> ClassToFilterPerceivedActorsTo = nullptr;

	

	//method to use to get angle difference between owning actor and perceived actor
	
	//If False, Control Rotation Is Used
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Perception Variables|Algorithm to Use for Getting Angle Between Vectors")
	bool bUseActorRotation = false;

	//Important For [Use Control Rotation]. If False, Arc Tan Difference Is Used
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Perception Variables|Algorithm to Use for Getting Angle Between Vectors")
	bool bUseDeltaRotation = true;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category= "AutoChange")
	bool bUseAutoChange = false;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category= "AutoChange")
	bool bUseAutoChangeWithAngle = false;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category= "AutoChange")
	float AngleThreshold = 20.f;
	
private:
	bool bCanSwitchTargets = false;
	bool bCanLockOn = true;
	

	//Find Ideal Actor To Target in given Array
	UFUNCTION(Category = HelperFunctions)
	void FindClosestActorWithinAngle(TArray<float> &InArray);

};
