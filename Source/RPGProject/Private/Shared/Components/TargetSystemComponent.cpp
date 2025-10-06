
#include "Shared/Components/TargetSystemComponent.h"

#include "Enemy/Character/EnemyCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/Character/MainCharacter.h"

UTargetSystemComponent::UTargetSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UTargetSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	LockOnToTarget(DeltaTime);
}


// Called when the game starts
void UTargetSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	if(OwnerCharacter)
	{
		// OwnerCharacter->TargetSystemOn.BindUFunction(this,"StorePerceivedActorsInArray");
	}
}

void UTargetSystemComponent::StorePerceivedActorsInArray()
{
	ClearTimer_StorePerceivedActorsInArray();
	
	if(!bCanLockOn)
	{
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle_StorePerceivedActors,
			this,
			&UTargetSystemComponent::Timer_StorePerceivedActorInArray,
			TimerIntervalForStoringPerceivedActorsInArray,
			bLoopTimerForStoringPerceivedActorsInArray);
		
		bCanLockOn = true;
	}
	else
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_StorePerceivedActors);
		bCanLockOn = false;
	}
}


void UTargetSystemComponent::Timer_StorePerceivedActorInArray()
{
	AllTargetActors.Empty();
	LowestDistance_Array.Empty();
	LowestRotationAngle_Array.Empty();
	
	TArray<AActor*> EnemyActors;
	UGameplayStatics::GetAllActorsOfClass(this,AEnemyCharacter::StaticClass(),EnemyActors);

	for(AActor* EnemyActor : EnemyActors)
	{
		AllTargetActors.Add(EnemyActor);
	}

	FVector PlayerLoc = OwnerCharacter->GetActorLocation();
	FRotator PlayerRot = OwnerCharacter->GetActorRotation();
	
	for(auto Actor : AllTargetActors)
	{
		FVector Loc = Actor->GetActorLocation();

		// Get Distance Between Player And Enemy
		float Dist = FVector::Dist2D(PlayerLoc,Loc);
		LowestDistance_Array.Add(Dist);

		// Get Angle Between Player And Enemy
		float Rot = GetAngleBetweenVectors_UsingDeltaRotation(PlayerLoc,Loc,PlayerRot);
		LowestRotationAngle_Array.Add(FMath::Abs(Rot));
	}

	// auto Lambda = [](const float l, const float r){ return l < r; };
	//
	// // Sort
	// LowestDistance_Array.Sort(Lambda);
	// LowestRotationAngle_Array.Sort(Lambda);
	if(bUseAutoChange)
	{
		if(bUseAutoChangeWithAngle)
		{
			DetermineActorWithTheLeastActorRotationAngleRequiredToTarget();
		}
		else
		{
			DetermineActorToTargetFromLowestRotationAngleArray();
		}
	}
	
}

void UTargetSystemComponent::DetermineActorWithTheLeastActorRotationAngleRequiredToTarget()
{
	FindClosestActorWithinAngle(LowestRotationAngle_Array);
}

void UTargetSystemComponent::DetermineActorToTargetFromLowestRotationAngleArray()
{
	int Index = 0;
	float Value = 0;
	UKismetMathLibrary::MinOfFloatArray(LowestRotationAngle_Array,Index,Value);
	if(AllTargetActors[Index])
	{
		ActorToTarget = AllTargetActors[Index];
	}
}

void UTargetSystemComponent::SwitchTargets(bool bSwitchTarget)
{
	if(bSwitchTarget)
	{
		int Index = 0;
		float Value = 0;
		UKismetMathLibrary::MinOfFloatArray(LowestRotationAngle_Array,Index,Value);
		
		if(AllTargetActors[Index])
		{
			ActorToTarget = AllTargetActors[Index];	
		}
		else
		{
			// ClientSwitchTargets(false);
		}
	}
}

void UTargetSystemComponent::SwitchToTheTargetToTheLeftOfCurrentTarget()
{
}

void UTargetSystemComponent::SwitchToTheTargetToTheRightOfCurrentTarget()
{
}


void UTargetSystemComponent::DisplayWidgetForLockedOnTarget()
{
	FLatentActionInfo ActionInfo;
	ActionInfo.ExecutionFunction = FName("ShouldShowWidget");
	UKismetSystemLibrary::RetriggerableDelay(this,DelayBeforeLockOn,FLatentActionInfo{});
}

bool UTargetSystemComponent::ShouldShowWidget()
{
	if(ShouldShowWidgetForLockOn)
	{
		if(ActorToTarget)
		{

		}
	}
	return false;
}


void UTargetSystemComponent::ClearTimer_StorePerceivedActorsInArray()
{
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_StorePerceivedActors);
	AllTargetActors.Empty();
	LowestRotationAngle_Array.Empty();
	LowestDistance_Array.Empty();
	
	LowestRotationAngle_Left_Array.Empty();
	LowestRotationAngle_Right_Array.Empty();
	
}

void UTargetSystemComponent::ShowWidgetForTargetNotBeingLockOnTo() const
{
}

void UTargetSystemComponent::LockOnToTarget(float DeltaTime)
{
	if(!bCanLockOn || !ActorToTarget) return;

	AController* Controller = OwnerCharacter->GetController();

	FRotator ControllerRot = Controller->GetControlRotation();
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(OwnerCharacter->GetActorLocation(),ActorToTarget->GetActorLocation());
	
	FRotator InterpRot =FMath::RInterpTo(ControllerRot,LookAtRot,DeltaTime,InterpolationSpeed);
	ControllerRot.Yaw = InterpRot.Yaw;
	
	Controller->SetControlRotation(InterpRot);
}

void UTargetSystemComponent::FindClosestActorWithinAngle(TArray<float>& InArray)
{
	
}


float UTargetSystemComponent::GetAngleBetweenVectors_UsingDeltaRotation(FVector InStart, FVector InTarget,
                                                                        FRotator InRotationToCheckAgainst)
{
	const FRotator LookAtRotator = UKismetMathLibrary::FindLookAtRotation(InStart, InTarget);
	const FRotator DeltaRotatorResult =  UKismetMathLibrary::NormalizedDeltaRotator(LookAtRotator, InRotationToCheckAgainst);
	return DeltaRotatorResult.Yaw;
}

float UTargetSystemComponent::GetAngleBetweenVectors_UsingArcTanDifference(FVector InStart, FVector InTarget,
	FVector InDirectionVector)
{
	const FRotator LookAtRotator = UKismetMathLibrary::FindLookAtRotation(InStart, InTarget);
	const FVector ForwardVectorFromLookAtRotator = UKismetMathLibrary::GetForwardVector(LookAtRotator);

	const float InverseTanA = UKismetMathLibrary::Atan2(InDirectionVector.X, InDirectionVector.Y);
	const float InverseTanB = UKismetMathLibrary::Atan2(ForwardVectorFromLookAtRotator.X, ForwardVectorFromLookAtRotator.Y);

	return  InverseTanA - InverseTanB;
}


