// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/UI/QuestTracker/QuestTrackerObjectivesWidget.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"

UQuestTrackerObjectivesWidget::UQuestTrackerObjectivesWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	
}

void UQuestTrackerObjectivesWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if(ProgressBar)
	{
		ProgressBar->SetPercent(0.f);
	}
	if(Image_GlowyLine)
	{
		Image_GlowyLine->SetColorAndOpacity({1.f,1.f,1.f,0.f});
	}
	
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindLambda([this]()
	{
		if(PlaybackSpeed > 1.f)
		{
			PlaybackSpeed = FMath::Clamp(PlaybackSpeed/ AccelerationRate,1.f,5.f);
		}
	});
	
	GetWorld()->GetTimerManager().SetTimer(
		TickTimer,   
		TimerDelegate,  
		TickIntervals,  
		true                  
	);
}

void UQuestTrackerObjectivesWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
}

void UQuestTrackerObjectivesWidget::OnObjectiveUpdate()
{
	if(OnUpdateAnimation && IsAnimationPlaying(OnUpdateAnimation))
	{
		SetNumLoopsToPlay(OnUpdateAnimation,++NumLoopsPlay);
		PlaybackSpeed = FMath::Clamp(PlaybackSpeed * AccelerationRate,1.f,5.f);
		SetPlaybackSpeed(OnUpdateAnimation,PlaybackSpeed);
	}
	else
	{
		NumLoopsPlay = 1;
		PlayAnimation(OnUpdateAnimation,0.f,1.f,EUMGSequencePlayMode::Type::Forward,PlaybackSpeed,false);
	}
}
