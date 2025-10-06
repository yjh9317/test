
#include "Enemy/AI/Tasks/BTTask_FindRandomLocation.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Enemy/Character/EnemyCharacter.h"
#include "Enemy/Controller/EnemyController.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation(FObjectInitializer const& ObjectInitializer)
	:UBTTask_BlackboardBase{ObjectInitializer}
{
	NodeName =  TEXT("Find Random Location In NavMesh");
	
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// get controller and character
	if(AEnemyController* const Cont = Cast<AEnemyController>(OwnerComp.GetAIOwner()))
	{
		if(AEnemyCharacter* const EnemyCharacter = Cast<AEnemyCharacter>(Cont->GetPawn()))
		{
			FVector const Origin = EnemyCharacter->GetActorLocation();

			// get the nav system and generate
			if(UNavigationSystemV1* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
			{
				FNavLocation Loc;
				if(NavSys->GetRandomPointInNavigableRadius(Origin, SearchRadius, Loc))
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), Loc.Location);
				}

				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;
}
