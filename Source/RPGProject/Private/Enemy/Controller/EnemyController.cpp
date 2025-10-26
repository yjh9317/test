
#include "Enemy/Controller/EnemyController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
//#include "Header/DebugHeader.h"
#include "GameplayTagAssetInterface.h"
#include "Enemy/Character/EnemyCharacter.h"
#include "Header/DebugHeader.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Player/Character/MainCharacter.h"

// EnemyController.cpp
AEnemyController::AEnemyController(FObjectInitializer const& ObjectInitializer)
{
    SetPerceptionSystem();

    BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
    BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
    
    // GameplayTag 초기화
    PlayerTag = FGameplayTag::RequestGameplayTag(FName("Character.Player"));
    EnemyTag = FGameplayTag::RequestGameplayTag(FName("Character.Enemy"));
}

void AEnemyController::BeginPlay()
{
    Super::BeginPlay();
    
    if (AIPerceptionComponent)
    {
        AIPerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(
            this, 
            &AEnemyController::OnPerceptionUpdated
        );
    }
    
    if (UBlackboardComponent* BBComp = GetBlackboardComponent())
    {
        BBComp->SetValueAsEnum("AIState", StaticCast<uint8>(EEnemyAIState::EAS_Passive));
    }
}

void AEnemyController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);
}

void AEnemyController::SetPerceptionSystem()
{
    AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
    SetPerceptionComponent(*AIPerceptionComponent);
    
    // Sight Config
    SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
    SightConfig->SightRadius = SightRadius;
    SightConfig->LoseSightRadius = LoseSightRadius;
    SightConfig->PeripheralVisionAngleDegrees = PeripheralVisionAngle;
    SightConfig->SetMaxAge(5.f);
    SightConfig->AutoSuccessRangeFromLastSeenLocation = 520.f;
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = false;
    
    AIPerceptionComponent->ConfigureSense(*SightConfig);
    AIPerceptionComponent->SetDominantSense(*SightConfig->GetSenseImplementation());
    
    // Hearing Config
    HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("HearingConfig"));
    HearingConfig->HearingRange = HearingRange;
    HearingConfig->SetMaxAge(3.0f);
    HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
    HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
    HearingConfig->DetectionByAffiliation.bDetectFriendlies = false;
    
    AIPerceptionComponent->ConfigureSense(*HearingConfig);
    
    // Damage Config
    DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("DamageConfig"));
    DamageConfig->SetMaxAge(5.0f);
    
    AIPerceptionComponent->ConfigureSense(*DamageConfig);
}

void AEnemyController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
    if (!IsValid(Actor) || !PerceptionHandler) return;
    
    TSubclassOf<UAISense> SenseClass = UAIPerceptionSystem::GetSenseClassForStimulus(this, Stimulus);
    
    if (SenseClass == UAISense_Sight::StaticClass())
    {
        HandleSightStimulus(Actor, Stimulus);
    }
    else if (SenseClass == UAISense_Hearing::StaticClass())
    {
        HandleHearingStimulus(Actor, Stimulus);
    }
    else if (SenseClass == UAISense_Damage::StaticClass())
    {
        HandleDamageStimulus(Actor, Stimulus);
    }
}

// void AEnemyController::HandleSightStimulus(AActor* Actor, const FAIStimulus& Stimulus)
// {
//     if (!IsPlayerActor(Actor)) return;
//     
//     UBlackboardComponent* BBComp = GetBlackboardComponent();
//     if (!BBComp) return;
//     
//     if (Stimulus.WasSuccessfullySensed())
//     {
//         // 플레이어 목격
//         BBComp->SetValueAsObject("Target", Actor);
//         BBComp->SetValueAsVector("LastKnownPosition", Stimulus.StimulusLocation);
//         
//         // AI State 변경 (필요시)
//         // BBComp->SetValueAsEnum("AIState", StaticCast<uint8>(EEnemyAIState::EAS_Investigating));
//     }
//     else
//     {
//         // 시야에서 벗어남
//         BBComp->SetValueAsObject("Target", nullptr);
//         // LastKnownPosition은 유지 (마지막 위치 수색용)
//     }
// }
//
// void AEnemyController::HandleHearingStimulus(AActor* Actor, const FAIStimulus& Stimulus)
// {
//     if (!IsPlayerActor(Actor)) return;
//     
//     UBlackboardComponent* BBComp = GetBlackboardComponent();
//     if (!BBComp) return;
//     
//     // 소리만 들림 - 위치 정보만 업데이트
//     BBComp->SetValueAsVector("LastKnownPosition", Stimulus.StimulusLocation);
//     
//     // 조사 상태로 전환 (이미 전투 중이 아니라면)
//     // if (CurrentState != Combat)
//     //     BBComp->SetValueAsEnum("AIState", StaticCast<uint8>(EEnemyAIState::EAS_Investigating));
// }
//
// void AEnemyController::HandleDamageStimulus(AActor* Actor, const FAIStimulus& Stimulus)
// {
//     if (!IsPlayerActor(Actor)) return;
//     
//     UBlackboardComponent* BBComp = GetBlackboardComponent();
//     if (!BBComp) return;
//     
//     // 데미지를 받음 - 즉시 타겟 설정
//     BBComp->SetValueAsObject("Target", Actor);
//     BBComp->SetValueAsVector("LastKnownPosition", Stimulus.StimulusLocation);
//     
//     // 전투 상태로 즉시 전환
//     // BBComp->SetValueAsEnum("AIState", StaticCast<uint8>(EEnemyAIState::EAS_Attacking));
// }

bool AEnemyController::IsPlayerActor(AActor* Actor) const
{
    if (!IsValid(Actor)) return false;
    
    // GameplayTag 체크
    if (IGameplayTagAssetInterface* TagInterface = Cast<IGameplayTagAssetInterface>(Actor))
    {
        return TagInterface->HasMatchingGameplayTag(PlayerTag);
    }
    
    // Fallback: ActorHasTag (호환성)
    return Actor->ActorHasTag(FName("Player"));
}

void AEnemyController::UpdateTargetInBlackboard(AActor* NewTarget)
{
    
}

void AEnemyController::StartSightRangeTimer(float NewSightRange)
{
    FTimerDelegate TimerDel;
    TimerDel.BindUFunction(this, FName("SetSightRange"), this, NewSightRange);
    
    GetWorld()->GetTimerManager().SetTimer(
        SightRangeTimerHandle,
        TimerDel,
        5.0f,
        false
    );
}

bool AEnemyController::SetSightRange(AAIController* Controller, float NewSightRange)
{
    if (!IsValid(Controller)) return false;
    
    UAIPerceptionComponent* Perception = Controller->GetAIPerceptionComponent();
    if (!IsValid(Perception)) return false;
    
    FAISenseID SightID = UAISense::GetSenseID<UAISense_Sight>();
    if (!SightID.IsValid()) return false;
    
    UAISenseConfig* Config = Perception->GetSenseConfig(SightID);
    UAISenseConfig_Sight* ConfigSight = Cast<UAISenseConfig_Sight>(Config);
    if (!IsValid(ConfigSight)) return false;
    
    // 기존 LoseSight 범위 차이 유지
    float RangeDiff = ConfigSight->LoseSightRadius - ConfigSight->SightRadius;
    
    ConfigSight->SightRadius = NewSightRange;
    ConfigSight->LoseSightRadius = NewSightRange + RangeDiff;
    
    Perception->RequestStimuliListenerUpdate();
    return true;
}