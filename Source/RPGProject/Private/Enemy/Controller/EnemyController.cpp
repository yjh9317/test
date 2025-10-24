
#include "Enemy/Controller/EnemyController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
//#include "Header/DebugHeader.h"
#include "Enemy/Character/EnemyCharacter.h"
#include "Header/DebugHeader.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Damage.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Player/Character/MainCharacter.h"

AEnemyController::AEnemyController(FObjectInitializer const& ObjectInitializer)
{
	SetPerceptionSystem();

	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	check(BlackboardComponent);

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorTreeComponent"));
	check(BehaviorTreeComponent);

	AIPerceptionComponent->bEditableWhenInherited = true;
}

void AEnemyController::BeginPlay()
{
	Super::BeginPlay();

	AMainCharacter* MainCharacter = Cast<AMainCharacter>(UGameplayStatics::GetPlayerCharacter(this,0));
	GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this,&AEnemyController::OnPerceptionUpdated);
	GetBlackboardComponent()->SetValueAsEnum("AIState",StaticCast<uint8>(EEnemyAIState::EAS_Passive));

	
}

void AEnemyController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AEnemyController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if(InPawn == nullptr) return;

	AEnemyCharacter* Enemy = Cast<AEnemyCharacter>(InPawn);

	if(Enemy)
	{
		if(Enemy->GetBehaviorTree())
		{
			Blackboard = BlackboardComponent;
			BlackboardComponent->InitializeBlackboard(*(Enemy->GetBehaviorTree()->BlackboardAsset));
			UseBlackboard(Enemy->GetBehaviorTree()->BlackboardAsset,BlackboardComponent);
			RunBehaviorTree(Enemy->GetBehaviorTree());

			// BehaviorTreeComponent->SetDynamicSubtree();
		}
	}
}

void AEnemyController::SetPerceptionSystem()
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception Component"));
	SetPerceptionComponent(*AIPerceptionComponent);
	
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	if(SightConfig)
	{
		SightConfig->SightRadius = 5000.f;
		SightConfig->LoseSightRadius = SightConfig->SightRadius + 250.f;
		SightConfig->PeripheralVisionAngleDegrees = 1000.f;
		SightConfig->SetMaxAge(5.f);
		SightConfig->AutoSuccessRangeFromLastSeenLocation = 520.f;				
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;		
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;	
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;		
		
		GetPerceptionComponent()->ConfigureSense((*SightConfig));
		GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
	}

	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
	if (HearingConfig)
	{
		HearingConfig->HearingRange = 1200.0f;
		HearingConfig->SetMaxAge(3.0f);
		HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
		HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
		HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;
		GetPerceptionComponent()->SetDominantSense(*HearingConfig->GetSenseImplementation());
		GetPerceptionComponent()->ConfigureSense((*HearingConfig));
	}
		
	DamageConfig = CreateDefaultSubobject<UAISenseConfig_Damage>(TEXT("Damage Config"));
	if(DamageConfig)
	{
		DamageConfig->SetMaxAge(5.0f);
		GetPerceptionComponent()->SetDominantSense(*DamageConfig->GetSenseImplementation());
		GetPerceptionComponent()->ConfigureSense((*DamageConfig));
	}
}

void AEnemyController::OnPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed())
	{
		if(Actor->ActorHasTag("Player"))
		{
			GetBlackboardComponent()->SetValueAsObject("Target",Actor);
		}
	}
	else
	{
		if(Actor->ActorHasTag("Player"))
		{
			GetBlackboardComponent()->SetValueAsObject("Target",nullptr);
			GetBlackboardComponent()->SetValueAsVector("PlayerLocation",FVector::ZeroVector);
		}
	}
}

void AEnemyController::StartSightRangeTimer(float NewSightRange)
{
	FTimerHandle TimerHandle;
	FTimerDelegate TimerDel;

	TimerDel.BindUFunction(this, FName("SetSightRange"), this, NewSightRange);
    
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDel, 5.0f, false);
}

bool AEnemyController::SetSightRange(AAIController* Controller, float NewSightRange)
{
	if (Controller == nullptr)
	{
		DebugHeader::LogWarning("AEnemyController::SetSightRange : Controller is nullptr");
		return false;
	}

	FAISenseID Id = UAISense::GetSenseID(UAISense_Sight::StaticClass());
	if (!Id.IsValid())
	{
		DebugHeader::LogWarning("AEnemyController::SetSightRange : Wrong Sense ID");
		return false;
	}

	auto Perception = Controller->GetAIPerceptionComponent();
	if (Perception == nullptr)
	{
		DebugHeader::LogWarning("AEnemyController::SetSightRange : Perception is nullptr");
		return false;
	}

	auto Config = Perception->GetSenseConfig(Id);
	if (Config == nullptr)
	{
		DebugHeader::LogError("AEnemyController::SetSightRange : Config is nullptr");
		return false;
	}

	auto ConfigSight = Cast<UAISenseConfig_Sight>(Config);
	if (ConfigSight == nullptr)
	{
		DebugHeader::LogWarning("AEnemyController::SetSightRange : Failed to cast Config to UAISenseConfig_Sight");
		return false;
	}

	float LoseRange = ConfigSight->LoseSightRadius - ConfigSight->SightRadius;

	ConfigSight->SightRadius = NewSightRange;
	ConfigSight->LoseSightRadius = ConfigSight->SightRadius + LoseRange;

	Perception->RequestStimuliListenerUpdate();
	return true;
}

