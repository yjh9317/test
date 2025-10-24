// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Consumable/BaseConsumable.h"

#include "Components/PostProcessComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/Status/BuffComponent.h"
#include "Components/Status/StatsComponent.h"
#include "Interface/ComponentManager.h"
#include "Item/Consumable/AbstractConsumable.h"
#include "Kismet/KismetMathLibrary.h"


void ABaseConsumable::BeginPlay()
{
	IComponentManager* ComponentManager = Cast<IComponentManager>(GetOwner());
	if(ComponentManager)
	{
		StatsComponent = ComponentManager->GetStatsComponent();
		BuffComponent = ComponentManager->GetBuffComponent();
		UseBuff();
		PlayAdditionalEffect();
		LoadAndPlaySoundAsync();
	}
}

void ABaseConsumable::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if(BuffComponent)
	{
		// BuffComponent->RemoveConsumableBuffFromStack(this);
	}
}

void ABaseConsumable::UseBuff()
{
	switch (BuffEffect)
	{
	case EBuffEffectCategory::AddInstantly:
		{
			AddValueToStat();
			RemoveBuff();
			break;
		}
	case EBuffEffectCategory::AddForDuration:
		{
			AddValueForDuration();
			InitializeTimelineAnimation();
			break;
		}
	case EBuffEffectCategory::AddOverDuration:
		{
			AddValueOverDuration();
			InitializeTimelineAnimation();
			break;
		}
	case EBuffEffectCategory::None:
		default:
		break;
	}
}

void ABaseConsumable::RemoveBuff()
{
	switch (BuffEffect)
	{
	case EBuffEffectCategory::None:
	case EBuffEffectCategory::AddInstantly:
		{
			RemoveAdditionalEffect();
			Destroy();
		}
	case EBuffEffectCategory::AddForDuration:
		{
			RemoveValueFromStat();
			GetWorld()->GetTimerManager().ClearTimer(ForDurationTimer);
			RemoveAdditionalEffect();
			Destroy();
		}
	case EBuffEffectCategory::AddOverDuration:
		{
			GetWorld()->GetTimerManager().ClearTimer(OverDurationTimer);
			RemoveAdditionalEffect();
			Destroy();
		}
	}
			
}

void ABaseConsumable::InitializeTimelineAnimation()
{
	// BuffComponent->InitializeCircularAnimation(StatTag);

	float remainingDuration = Duration - UKismetMathLibrary::SafeDivide(Duration, NumberOfReps);
	float inverseRemainingDuration = UKismetMathLibrary::SafeDivide(1, remainingDuration);
	EffectTimeline->SetPlayRate(inverseRemainingDuration);
	if(EffectCurve)
	{
		FOnTimelineFloat TimelineUpdateDelegate;
		TimelineUpdateDelegate.BindUFunction(this, FName("UpdateEffectTimeline"));
		EffectTimeline->AddInterpFloat(EffectCurve, TimelineUpdateDelegate, FName("FloatValue"));
		EffectTimeline->PlayFromStart();
	}
	
}

void ABaseConsumable::AddValueOverDuration()
{
	if(!GetWorldTimerManager().TimerExists(OverDurationTimer))
	{
		float RepsCount = UKismetMathLibrary::SafeDivide(Duration, NumberOfReps);
		GetWorldTimerManager().SetTimer(
			OverDurationTimer,
			this,
			&ABaseConsumable::AddValueOverDuration,
			RepsCount,
			true);
	}
	const FGameplayTag HealthTag = FGameplayTag::RequestGameplayTag(FName("Character.Stat.Health"));
	const FGameplayTag StaminaTag = FGameplayTag::RequestGameplayTag(FName("Character.Stat.Stamina"));

	if (StatTag == HealthTag || StatTag == StaminaTag)
	{
		AddValueToStat();
		if(NumberOfReps <= ++CurrentRep)
		{
			RemoveBuff();
		}
	}
}

void ABaseConsumable::AddValueForDuration()
{
	if(!GetWorldTimerManager().TimerExists(OverDurationTimer))
	{
		GetWorldTimerManager().SetTimer(
		OverDurationTimer,
		this,
		&ABaseConsumable::AddValueForDuration,
		Duration,
		true);
	}
	else
	{
		RemoveBuff();
	}
	AddValueToStat();
}

void ABaseConsumable::ResetAddValueForDuration()
{
	if(BuffEffect == EBuffEffectCategory::AddForDuration && GetWorldTimerManager().TimerExists(OverDurationTimer))
	{
		GetWorldTimerManager().ClearTimer(ForDurationTimer);
		RemoveValueFromStat();
		AddValueForDuration();
	}
}

void ABaseConsumable::RemoveValueFromStat()
{
	if(StatsComponent)
	{
		// StatsComponent->ServerRemoveStatValue(StatTag,Value);
	}
}

void ABaseConsumable::AddValueToStat()
{
	if(StatsComponent)
	{
		// StatsComponent->ServerAddStatValue(StatTag,Value);
	}
}

void ABaseConsumable::PlayAdditionalEffect()
{
	switch (BuffEffect)
	{
		case EAdditionalBuffEffect::Drunk:
			// if(!PostProcess_Drunk)
			// {
			// 	PostProcess_Drunk = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcess_Drunk"));
			// 	PostProcess_Drunk->SetupAttachment(GetRootComponent());
			// }
			break;
	}
}

void ABaseConsumable::RemoveAdditionalEffect()
{
	switch (BuffEffect)
	{
	case EAdditionalBuffEffect::Drunk:
		// if(PostProcess_Drunk)
		// {
		// 	PostProcess_Drunk->DestroyComponent();
		// }
		break;
	}
}

void ABaseConsumable::LoadAndPlaySoundAsync()
{
}

void ABaseConsumable::UpdateEffectTimeline()
{
	float CurrentTime = EffectTimeline->GetPlaybackPosition();
	//BuffComponent->StatsDecimal.Add(StatTag,EffectCurve->GetFloatValue(CurrentTime));
}
