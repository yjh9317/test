
#include "MountSystem/Components/MountComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "MountSystem/Components/RiderComponent.h"

UMountComponent::UMountComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}

void UMountComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	SpringArmComponent = OwnerCharacter->FindComponentByClass<USpringArmComponent>();
	CameraComponent = OwnerCharacter->FindComponentByClass<UCameraComponent>();

	ControlRotation = OwnerCharacter->GetControlRotation();
	
	SpringArmRelativeLocation = SpringArmComponent->GetRelativeLocation();
	CameraRelativeLocation = CameraComponent->GetRelativeLocation();
	// Character->FindComponentByClass<AnimalMovementComponent>()
}


void UMountComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

FName UMountComponent::GetSocketName(FName NewSocketName)
{
	if(NewSocketName.IsNone())
	{
		return MountPointSocket;
	}
	else
	{
		return NewSocketName;
	}
}

void UMountComponent::ChangeSocketName(FName NewName)
{
	FAttachmentTransformRules Rule(EAttachmentRule::SnapToTarget,false);
	Rule.LocationRule = EAttachmentRule::SnapToTarget;
	Rule.RotationRule = EAttachmentRule::SnapToTarget;
	Rule.ScaleRule = EAttachmentRule::KeepWorld;
	RiderComponent->Character->AttachToComponent(OwnerCharacter->GetMesh(),Rule,NewName);
}

void UMountComponent::SetCharacterControlRotation(const FRotator& RiderControlRotation)
{
	FRotator FinalRotation = UKismetMathLibrary::MakeRotator(ControlRotation.Roll,RiderControlRotation.Pitch,ControlRotation.Yaw);
	ACharacter* LocalCharacter = Cast<ACharacter>(GetOwner());
	AController* Controller = OwnerCharacter->GetController();
	if(LocalCharacter && Controller)
	{
		Controller->SetControlRotation(FinalRotation);
	}
}

void UMountComponent::SetCameraPosition()
{
}

void UMountComponent::ResetControlRotation()
{
}

