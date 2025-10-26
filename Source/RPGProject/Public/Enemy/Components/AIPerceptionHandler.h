// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "AIPerceptionHandler.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UAIPerceptionHandler : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAIPerceptionHandler();

	void ProcessStimulus(AActor* Actor, const FAIStimulus& Stimulus);
	void ProcessSightStimulus(AActor* Actor, const FAIStimulus& Stimulus);
	void ProcessHearingStimulus(AActor* Actor, const FAIStimulus& Stimulus);
	void ProcessDamageStimulus(AActor* Actor, const FAIStimulus& Stimulus);
    
private:
	// 인지 관련 데이터만
	// TMap<AActor*, FPerceptionData> KnownActors;
	// float LastSightTime;
	// FVector LastHeardLocation;
		
};
