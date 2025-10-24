
#include "UI/HUD/MainHUD.h"

#include "Blueprint/UserWidget.h"
#include "Interaction/UI/InteractionWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UI/HUD/RPGMainHUDWidget.h"
#include "UI/Menu/MainMenu.h"


AMainHUD::AMainHUD()
{

}

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();
	
	UWorld* World = GetWorld();
	checkf(RPGMainHUDWidgetClass,TEXT("AMainHUD::BeginPlay : RPGMainHUDWidgetClass is not valid"));
	checkf(World,TEXT("AMainHUD::BeginPlay : GetWorld is not valid"));
	
	RPGMainHUDWidget = CreateWidget<URPGMainHUDWidget>(GetWorld(), RPGMainHUDWidgetClass);
	RPGMainHUDWidget->SetVisibility(ESlateVisibility::Visible);
	if(UKismetSystemLibrary::HasMultipleLocalPlayers(World))
	{
		RPGMainHUDWidget->AddToPlayerScreen(1);
	}
	else
	{
		RPGMainHUDWidget->AddToViewport(1);
	}
	
	InitWidgets();
}


void AMainHUD::InitWidgets()
{
	if(CharacterOverlayClass)
	{
		CharacterOverlay = CreateWidget<UUserWidget>(GetWorld(),CharacterOverlayClass);
		CharacterOverlay->AddToViewport(0);
		CharacterOverlay->SetVisibility(ESlateVisibility::Visible);
	}
	if(MainMenuClass)
	{
		MainMenuWidget = CreateWidget<UMainMenu>(GetWorld(),MainMenuClass);
		MainMenuWidget->AddToViewport(5);
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	
}

void AMainHUD::SetMenuVisibility(bool IsVisible)
{
	if (!MainMenuWidget) return;

	if (IsVisible)
	{
		MainMenuWidget->SetVisibility(ESlateVisibility::Visible);

		const FInputModeUIOnly InputMode; 
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(true);
	}
	else
	{
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);

		const FInputModeGameOnly InputMode; 
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(false);
	}
}

void AMainHUD::SetInteractionWidgetVisibility(bool IsVisible) const
{
	if (!InteractionWidget) return;

	if (IsVisible)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AMainHUD::UpdateInteractionWidget(const FInteractableData* InteractableData) const
{
	if(InteractionWidget)
	{
		if(InteractionWidget->GetVisibility() == ESlateVisibility::Collapsed)
		{
			InteractionWidget->SetVisibility(ESlateVisibility::Visible);
		}

		InteractionWidget->UpdateWidget(InteractableData);
	}
}