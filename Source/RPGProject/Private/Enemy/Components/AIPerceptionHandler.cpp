// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy/Components/AIPerceptionHandler.h"

#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Sight.h"

UAIPerceptionHandler::UAIPerceptionHandler()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UAIPerceptionHandler::ProcessStimulus(AActor* Actor, const FAIStimulus& Stimulus)
{
	if (!IsValid(Actor)) return;
    
	// 타입 확인
	TSubclassOf<UAISense> SenseClass = UAIPerceptionSystem::GetSenseClassForStimulus(
		GetOwner(), 
		Stimulus
	);
    
	// 적절한 핸들러로 분기
	if (SenseClass == UAISense_Sight::StaticClass())
		ProcessSightStimulus(Actor, Stimulus);
	else if (SenseClass == UAISense_Hearing::StaticClass())
		ProcessHearingStimulus(Actor, Stimulus);
	else if (SenseClass == UAISense_Damage::StaticClass())
		ProcessDamageStimulus(Actor, Stimulus);
}

void UAIPerceptionHandler::ProcessSightStimulus(AActor* Actor, const FAIStimulus& Stimulus)
{
	// if (!IsPlayerActor(Actor)) return;
 //    
	// if (Stimulus.WasSuccessfullySensed())
	// {
	// 	// 시야에 들어옴
	// 	OnPlayerSighted.Broadcast(Actor);  // 이벤트 발생
 //        
	// 	UpdateKnownActorData(Actor, Stimulus);
 //        
	// 	// Blackboard 업데이트
	// 	if (CachedBlackboard)
	// 	{
	// 		CachedBlackboard->SetValueAsObject("Target", Actor);
	// 		CachedBlackboard->SetValueAsVector("LastKnownPosition", Stimulus.StimulusLocation);
	// 	}
	// }
	// else
	// {
	// 	// 시야에서 벗어남
	// 	OnPlayerLost.Broadcast(Actor);
 //        
	// 	if (CachedBlackboard)
	// 	{
	// 		CachedBlackboard->SetValueAsObject("Target", nullptr);
	// 		// LastKnownPosition은 유지
	// 	}
	// }
}

void UAIPerceptionHandler::ProcessHearingStimulus(AActor* Actor, const FAIStimulus& Stimulus)
{
	// if (!IsPlayerActor(Actor)) return;
 //    
	// // 소리 정보 저장
	// LastHeardLocation = Stimulus.StimulusLocation;
	// LastHeardTime = GetWorld()->GetTimeSeconds();
 //    
	// OnSoundHeard.Broadcast(Stimulus.StimulusLocation);
 //    
	// // 시각 정보가 없을 때만 소리 위치로 이동
	// if (!CachedBlackboard->GetValueAsObject("Target"))
	// {
	// 	CachedBlackboard->SetValueAsVector("InvestigateLocation", Stimulus.StimulusLocation);
	// }
}

void UAIPerceptionHandler::ProcessDamageStimulus(AActor* Actor, const FAIStimulus& Stimulus)
{
	// OnDamageTaken.Broadcast(Actor, Stimulus.StimulusLocation);
 //    
	// if (CachedBlackboard)
	// {
	// 	CachedBlackboard->SetValueAsObject("Target", Actor);
	// 	CachedBlackboard->SetValueAsVector("LastKnownPosition", Stimulus.StimulusLocation);
	// 	CachedBlackboard->SetValueAsEnum("AIState", StaticCast<uint8>(EEnemyAIState::EAS_Attacking));
	// }
}


