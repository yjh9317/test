#include "MountSystem/Components/AnimalMovementComponent.h"
#include "GameFramework/Character.h"

UAnimalMovementComponent::UAnimalMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UAnimalMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	if(OwnerCharacter)
	{
		OwnerCharacterMovementComponent = OwnerCharacter->GetCharacterMovement();
	}
}


void UAnimalMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

