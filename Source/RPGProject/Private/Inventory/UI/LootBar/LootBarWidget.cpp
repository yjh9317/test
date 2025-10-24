// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/LootBar/LootBarWidget.h"
#include "Inventory/UI/LootBar/LootBarSlotWidget.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Components/Button.h"
#include "Components/ScrollBox.h"
#include "Components/VerticalBox.h"
#include "Header/DebugHeader.h"
#include "Interface/ComponentManager.h"
#include "Interface/ObjectInteraction.h"
#include "Inventory/Components/InventoryComponent.h"
#include "Inventory/Components/InventoryCoreComponent.h"
#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"
#include "Kismet/KismetInputLibrary.h"


void ULootBarWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	APlayerController* PlayerController = GetOwningPlayer();
	if (PlayerController)
	{
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			if (TakeItemAction)
			{
				EnhancedInputComponent->BindAction(TakeItemAction, ETriggerEvent::Triggered, this, &ULootBarWidget::OnTakeItemTriggered);
			}
		}
	}
	if(Button_TakeAll)
	{
		Button_TakeAll->OnClicked.AddDynamic(this,&ULootBarWidget::TakeAllItems);
	}
	
	if(Button_Take)
	{
		Button_Take->OnClicked.AddDynamic(this,&ULootBarWidget::TakeItem);
	}

	if(Button_Close)
	{
		Button_Close->OnClicked.AddDynamic(this,&ULootBarWidget::CloseButton);
	}
}

void ULootBarWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

FReply ULootBarWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FVector2D MouseCursorDelta = UKismetInputLibrary::PointerEvent_GetCursorDelta(InMouseEvent);
	if(MouseCursorDelta.X != 0 || MouseCursorDelta.Y !=0)
	{
		bKeyboardFocus = false;
		return FReply::Handled();
	}
	return FReply::Unhandled();
}

FReply ULootBarWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	SetFocusToSlot(FocusedIndex);
	return FReply::Handled();
}

FReply ULootBarWidget::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	FKey InputKey = UKismetInputLibrary::GetKey(InKeyEvent);
	if(InputKey == EKeys::W || InputKey == EKeys::Up)
	{
		bKeyboardFocus = true;
		SetFocusToSlot(FocusedIndex - 1);
		return FReply::Handled();
	}
	else if(InputKey == EKeys::S || InputKey == EKeys::Down)
	{
		bKeyboardFocus = true;
		SetFocusToSlot(FocusedIndex + 1);
		return FReply::Handled();
	}
	return FReply::Unhandled();
}


void ULootBarWidget::EventToggleInputListening()
{
	bCanListenForInput = false;

	GetWorld()->GetTimerManager().SetTimer(CanListenForInputHandle,
		[this]()
		{
			bCanListenForInput = true;
		},
		0.2f,
		false);
	
}

void ULootBarWidget::CreateLootBar()
{
	VerticalBox->ClearChildren();
	
	int LocalInventorySize = 0;
	TArray<FItemData> LocalInventory;
	OwnerInventory->GetInventoryAndSize(EInventoryPanel::P1,LocalInventory,LocalInventorySize);

	for(const auto& CurrentItem : LocalInventory)
	{
		if(UInventoryUtilityLibrary::IsItemClassValid(CurrentItem))
		{
			LootBarSlotWidget = CreateWidget<ULootBarSlotWidget>(GetOwningPlayer(),LootBarSlotWidgetClass);
			if(LootBarSlotWidget)
			{
				LootBarSlotWidget->ItemData = CurrentItem;
				VerticalBox->AddChild(LootBarSlotWidget);
			}
		}
	}
}

void ULootBarWidget::ScrollWidget(UUserWidget* FocusedWidget)
{
	ScrollBox->ScrollWidgetIntoView(FocusedWidget,true,EDescendantScrollDestination::Center,0.f);
}

void ULootBarWidget::RefreshLootBar(EInventoryPanel Panel)
{
	if(ValidateOwnerInventory())
	{
		CreateLootBar();
		SetFocusToSlot(FocusedIndex);
	}
}

void ULootBarWidget::SetOwnerInventory()
{
	if(nullptr == GetOwningPlayer())
	{
		return;
	}
	
	IObjectInteraction* ObjectInteraction = Cast<IObjectInteraction>(GetOwningPlayer());
	if(ObjectInteraction)
	{
		if(AActor* CurrentInteractedObject = ObjectInteraction->GetCurrentInteractableObject())
		{
			UInventoryCoreComponent* InventoryCoreComp = CurrentInteractedObject->FindComponentByClass<UInventoryCoreComponent>();
			if(InventoryCoreComp)
			{
				OwnerInventory = InventoryCoreComp;
			}
			else
			{
				OwnerInventory = nullptr;
			}
		}
		else
		{
			OwnerInventory = nullptr;
		}
	}
}

void ULootBarWidget::ActivateLootBar()
{
	if(nullptr == GetOwningPlayer())
	{
		return;
	}
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(GetOwningPlayer(),this);
	IComponentManager* ComponentManager = Cast<IComponentManager>(GetOwningPlayer());
	if(ComponentManager)
	{
		PlayerInventory = ComponentManager->GetInventoryComponent();
		SetOwnerInventory();
		if(OwnerInventory && PlayerInventory)
		{
			TakeItemHandle = PlayerInventory->OnTakeItem.AddUObject(this,&ULootBarWidget::TakeItem);
			TakeAllItemHandle = PlayerInventory->OnTakeAllItem.AddUObject(this,&ULootBarWidget::TakeAllItems);
			RefreshLootBarHandle = OwnerInventory->OnRefreshInventory.AddUObject(this,&ULootBarWidget::RefreshLootBar);
		}
		PlayConstructAnim();
		CreateLootBar();
		EventToggleInputListening();
	}
}

void ULootBarWidget::DeactivateLootBar()
{
	if(PlayerInventory)
	{
		PlayerInventory->OnTakeItem.Remove(TakeItemHandle);
		PlayerInventory->OnTakeAllItem.Remove(TakeAllItemHandle);
	}
	if(OwnerInventory)
	{
		OwnerInventory->OnRefreshInventory.Remove(RefreshLootBarHandle);
	}
	FocusedIndex = 0;
	ScrollBox->ScrollToStart();
}

void ULootBarWidget::SetFocusToSlot(int32 SlotIndex)
{
	if(VerticalBox)
	{
		if(VerticalBox->GetChildAt(SlotIndex))
		{
			SetKeyboardFocus();
		}
		else if(VerticalBox->GetChildAt(--SlotIndex))
		{
			SetKeyboardFocus();
		}
	}
}

void ULootBarWidget::TakeItem()
{
	APlayerController* LocalPC = GetOwningPlayer();
	if(nullptr == OwnerInventory || nullptr == LocalPC)
	{
		DebugHeader::LogWarning(TEXT("ULootBarWidget::TakeItem : OwnerInventory is nullptr"));
		return;
	}
	
	TArray<FItemData> LocalInventory{};
	int32 InventorySize = 0;
	OwnerInventory->GetInventoryAndSize(EInventoryPanel::P1, LocalInventory, InventorySize);

	if(!LocalInventory.IsValidIndex(FocusedIndex))
	{
		DebugHeader::LogWarning(TEXT("ULootBarWidget::TakeItem : FocusedIndex is not valid"));
		return;	
	}
	
	FItemData FocusedItemData = LocalInventory[FocusedIndex];
	if(UInventoryUtilityLibrary::IsItemClassValid(FocusedItemData))
	{
		PlayerInventory->Server_TakeItem(OwnerInventory,FocusedItemData,LocalPC);
		
		OwnerInventory->GetInventoryAndSize(EInventoryPanel::P1, LocalInventory, InventorySize);
		if(LocalInventory.IsValidIndex(0))
		{
			SetFocusToSlot(FocusedIndex);
		}
	}
}

void ULootBarWidget::SetCurrentlyFocusedSlot(ULootBarSlotWidget* InSlotWidget)
{
	if(InSlotWidget)
	{
		FocusedIndex = VerticalBox->GetChildIndex(InSlotWidget);
	}
}

void ULootBarWidget::TakeAllItems()
{
	APlayerController* LocalPC = GetOwningPlayer();
	if(PlayerInventory && OwnerInventory && LocalPC)
	{
		PlayerInventory->Server_TakeAllItems(OwnerInventory,LocalPC);
	}
}

void ULootBarWidget::CloseButton()
{
	if(PlayerInventory)
	{
		// PlayerInventory->inputcloseWidget;
	}
}

bool ULootBarWidget::ValidateOwnerInventory()
{
	if(OwnerInventory)
	{
		TArray<FItemData> LocalInventory;
		int32 LocalSize = 0;
		OwnerInventory->GetInventoryAndSize(EInventoryPanel::P1, LocalInventory, LocalSize);

		return LocalInventory.IsValidIndex(0);
	}
	return false;
}

TSoftObjectPtr<UTexture2D> ULootBarWidget::GetInputIconTakeItem() const
{
	return TakeItemTexture;
}

TSoftObjectPtr<UTexture2D> ULootBarWidget::GetInputIconTakeAllItems() const
{
	return TakeAllItemTexture;
}

TSoftObjectPtr<UTexture2D> ULootBarWidget::GetInputIconCloseBar() const
{
	return CloseBarTexture;
}

void ULootBarWidget::OnTakeItemTriggered(const FInputActionValue& Value)
{
	if(TakeItemAction && IsVisible() && bCanListenForInput) 
	{
		TakeItem();
	}	
}
