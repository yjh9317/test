

#include "Shared/Notify/ANS_SpeedModifier.h"
#include "Player/Character/MainCharacter.h"


void UANS_SpeedModifier::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                     float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	AMainCharacter* PlayerCharacter = Cast<AMainCharacter>(MeshComp->GetOwner());
	if(PlayerCharacter)
	{
		PlayerCharacter->SetCurrentMontageRate(StartSpeed);
	}
}

void UANS_SpeedModifier::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                   const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	AMainCharacter* PlayerCharacter = Cast<AMainCharacter>(MeshComp->GetOwner());
	if(PlayerCharacter)
	{
		PlayerCharacter->SetCurrentMontageRate(EndSpeed);
	}
}
