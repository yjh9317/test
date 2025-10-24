// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/PlayerWidgetComponent.h"

#include "Blueprint/UserWidget.h"
#include "Header/DebugHeader.h"

UPlayerWidgetComponent::UPlayerWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UPlayerWidgetComponent::BeginPlay()
{

}


void UPlayerWidgetComponent::InitializeComponentData(AMainPlayerController* PC, AMainCharacter* Character)
{
	MainPlayerController = PC;
	MainCharacter = Character;
}

void UPlayerWidgetComponent::SetNewActiveWidget(EWidgetType NewType)
{
	ActiveWidget = NewType;
	OnSwitchedWidget.Broadcast(ActiveWidget);
}

void UPlayerWidgetComponent::SetNewActiveTab(EWidgetType NewType)
{
	ActiveTab = NewType;
	OnSwitchedTab.Broadcast(ActiveTab);
}

void UPlayerWidgetComponent::SetNewActivePopup(UUserWidget* NewWidget, EWidgetPopup NewType)
{
	CurrentPopupWidget = NewWidget;
	ActivePopup = NewType;
}

void UPlayerWidgetComponent::CloseActivePopup()
{
	ActivePopup = EWidgetPopup::None;
	if(CurrentPopupWidget)
	{
		CurrentPopupWidget->RemoveFromParent();
	}
}

void UPlayerWidgetComponent::ClearCurrentMessage()
{
	if(CurrentMessageWidget)
	{
		CurrentMessageWidget->RemoveFromParent();
		CurrentMessageWidget = nullptr;
	}
}

void UPlayerWidgetComponent::DisplayMessagePopup(const FText& Message)
{
	if(Message.IsEmpty() || CurrentMessageWidget)
	{
		DebugHeader::Log("UPlayerWidgetComponent::DisplayMessagePopup : Message is empty or CurrentMessageWidget is already active");
		return;
	}

	// Message 클래스 바꿔야 Message 매개변수 전달가능
	// CurrentMessageWidget = CreateWidget(MainPlayerController,MessageClass);
	
	FTimerHandle LocalTimerHandle{};
	GetWorld()->GetTimerManager().SetTimer(
		LocalTimerHandle,
		FTimerDelegate::CreateLambda([this](){ ClearCurrentMessage();}),
		2.f,
		false
	);
}

