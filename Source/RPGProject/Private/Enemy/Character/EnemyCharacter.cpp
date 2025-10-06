
#include "Enemy/Character/EnemyCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Components/Status/HealthBarComponent.h"
#include "Enemy/Controller/EnemyController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
// #include "Perception/AIPerceptionStimuliSourceComponent.h"
// #include "Perception/AISense_Sight.h"

AEnemyCharacter::AEnemyCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	,HealthBarDisplayTime(4.f)
	,HitNumberDestroyTime(1.5f)

{
	HealthBarComponent = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBarComponent"));
	HealthBarComponent->SetupAttachment(GetRootComponent());

}

void AEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	EnemyInitialize();

}

void AEnemyCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateHitNumbers();
}

void AEnemyCharacter::EnemyInitialize()
{
	EnemyController = Cast<AEnemyController>(GetController());
}

void AEnemyCharacter::ShowHealthBar()
{
	GetWorldTimerManager().ClearTimer(HealthBarTimer);
	HealthBarComponent->SetVisibility(true);
	GetWorldTimerManager().SetTimer(HealthBarTimer,this,&AEnemyCharacter::HideHealthBar,HealthBarDisplayTime);
}

void AEnemyCharacter::HideHealthBar()
{
	HealthBarComponent->SetVisibility(false);
}

void AEnemyCharacter::Hit(FHitResult& HitResult)
{
	if(HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this,HitSound,GetActorLocation());
	}
	if(HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),HitParticles,HitResult.Location);
	}
	ShowHealthBar();

	// const float Stunned = FMath::FRandRange(0.f,1.f);
	// if(Stunned <= StunChance)
	// {
	// 	// PlayHitMontage("HitReactFront");
	// 	SetStunned(true);
	// }
}

void AEnemyCharacter::Die()
{
	Super::Die();
	HideHealthBar();

	if(EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("Dead"),true);
		EnemyController->StopMovement();
	}
}

float AEnemyCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                  AActor* DamageCauser)
{
	if(EnemyController)
	{
		EnemyController->GetBlackboardComponent()->SetValueAsObject(FName("Target"),DamageCauser);
	}

			
	return DamageAmount;
}

void AEnemyCharacter::ShowHitNumber(int32 Damage, const FVector& HitLocation)
{
	// UUserWidget Hit전용 Widget으로 바꾼 다음 Text 바꿔야함 데미지값으로.
	// WidgetAnimation도 추가해서 PlayAnimation 함수 호출
	FVector2D ScreenLocation;
	UGameplayStatics::GetPlayerController(this, 0)->ProjectWorldLocationToScreen(HitLocation, ScreenLocation);
	UUserWidget* HitNumberWidget = CreateWidget<UUserWidget>(GetWorld(), HitNumberWidgetClass);
	if(HitNumberWidget != nullptr)
	{
		HitNumberWidget->AddToViewport();
		HitNumberWidget->SetPositionInViewport(ScreenLocation);
		StoreHitNumber(HitNumberWidget, HitLocation);
	}
}

void AEnemyCharacter::SetupStimulusScore()
{

}

void AEnemyCharacter::StoreHitNumber(UUserWidget* HitNumber,const FVector& Location)
{
	HitNumbers.Add(HitNumber,Location);

	FTimerHandle HitNumberTimer;
	FTimerDelegate HitNumberDelegate;
	HitNumberDelegate.BindUFunction(this,FName("DestroyHitNumber"),HitNumber);
	GetWorld()->GetTimerManager().SetTimer(HitNumberTimer,HitNumberDelegate,HitNumberDestroyTime,false);
}

void AEnemyCharacter::DestroyHitNumber(UUserWidget* HitNumber)
{
	HitNumbers.Remove(HitNumber);
	HitNumber->RemoveFromParent();
}

void AEnemyCharacter::UpdateHitNumbers()
{
	for(auto& HitPair : HitNumbers)
	{
		UUserWidget* HitNumber{HitPair.Key};
		const FVector Location{HitPair.Value};
		FVector2D ScreenPosition;
		UGameplayStatics::ProjectWorldToScreen(
			GetWorld()->GetFirstPlayerController(),
			Location,
			ScreenPosition);
		HitNumber->SetPositionInViewport(ScreenPosition);
	}
}



void AEnemyCharacter::DoDamage(AMainCharacter* Victim)
{
	if(Victim == nullptr) return;
	// UGameplayStatics::ApplyDamage(
	// 	Victim,
	// 	20,
	// 	EnemyController,
	// 	this,
	// 	UDamageType::StaticClass()
	// );

	//if(Victim->GetMeleeImpactSound())
	{
		//UGameplayStatics::PlaySoundAtLocation()
	}
	
}

void AEnemyCharacter::SpawnBlood(AMainCharacter* Victim, const FName& SocketName)
{
	const USkeletalMeshSocket* TipSocket{GetMesh()->GetSocketByName(SocketName)};
	if(TipSocket)
	{
		const FTransform SocketTransform{TipSocket->GetSocketTransform(GetMesh())};
		//if(Victim->GetParticle)
		{
			//UGameplayStatics::SpawnEmitterAtLocation(GetWorld(),Victim->GetParticle(),SocketTransform);
		}
	}
}

void AEnemyCharacter::ResetCanAttack()
{
	if(EnemyController)
	{
		GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("CanAttack"),true);
	}
}



void AEnemyCharacter::StunCharacter(AMainCharacter* Victim)
{
	if(Victim)
	{
		const float Stun{FMath::FRandRange(0.f,1.f)};
		// if(Stun <= Victim->GetStunChance())
		// {
		// 	Victim->Stun();
		// }
	}
}

void AEnemyCharacter::SetMovementSpeed(EEnemyMovementSpeed MovementSpeed)
{
	switch (MovementSpeed)
	{
	case EEnemyMovementSpeed::EMS_Idle:
		GetCharacterMovement()->MaxWalkSpeed = 0.f;
		break;
	case EEnemyMovementSpeed::EMS_Walking:
		GetCharacterMovement()->MaxWalkSpeed = 200.f;
		break;
	case EEnemyMovementSpeed::EMS_Sprinting:
		GetCharacterMovement()->MaxWalkSpeed = 600.f;
		break;
	};
}
