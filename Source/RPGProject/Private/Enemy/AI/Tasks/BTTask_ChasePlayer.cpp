#include "Enemy/AI/Tasks/BTTask_ChasePlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Enemy/Controller/EnemyController.h"

UBTTask_ChasePlayer::UBTTask_ChasePlayer(FObjectInitializer const& ObjectInitializer)
	:UBTTask_BlackboardBase{ObjectInitializer}
{
	NodeName = TEXT("Chase Player");
}

EBTNodeResult::Type UBTTask_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(AEnemyController* const Cont = Cast<AEnemyController>(OwnerComp.GetAIOwner()))
	{
		FVector const PlayerLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector((GetSelectedBlackboardKey()));

		UAIBlueprintHelperLibrary::SimpleMoveToLocation(Cont, PlayerLocation);

		FinishLatentTask(OwnerComp,EBTNodeResult::Succeeded);
	}
	return EBTNodeResult::Failed;
}
