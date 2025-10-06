
#include "UI/HUD/MainHUD.h"

#include "Blueprint/UserWidget.h"
#include "Interaction/UI/InteractionWidget.h"
#include "UI/HUD/RPGMainHUDWidget.h"
#include "UI/Menu/MainMenu.h"


AMainHUD::AMainHUD()
{

}

void AMainHUD::BeginPlay()
{
	Super::BeginPlay();

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

	// if(InteractionWidgetClass)
	// {
	// 	InteractionWidget = CreateWidget<UInteractionWidget>(GetWorld(),InteractionWidgetClass);
	// 	InteractionWidget->AddToViewport(-1);
	// 	InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	// }

	if(RPGMainHUDWidgetClass)
	{
		RPGMainHUDWidget = CreateWidget<URPGMainHUDWidget>(GetWorld(),RPGMainHUDWidgetClass);
		RPGMainHUDWidget->AddToViewport(1);
		RPGMainHUDWidget->SetVisibility(ESlateVisibility::Visible);
		RPGMainHUDWidget->ShowMinimap();
	}
	
}

void AMainHUD::DisplayMenu()
{
	if(MainMenuWidget)
	{
		bIsMenuVisible = true;
		MainMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainHUD::HideMenu()
{
	if(MainMenuWidget)
	{
		bIsMenuVisible = false;
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AMainHUD::ToggleMenu()
{
	if(bIsMenuVisible)
	{
		HideMenu();
		const FInputModeGameOnly InputMode;
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(false);
	}
	else
	{
		DisplayMenu();
		const FInputModeGameOnly InputMode;
		GetOwningPlayerController()->SetInputMode(InputMode);
		GetOwningPlayerController()->SetShowMouseCursor(true);
	}
}

void AMainHUD::ShowInteractionWidget() const
{
	if(InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainHUD::HideInteractionWidget() const
{
	if(InteractionWidget)
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