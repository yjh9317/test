// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/Storage/StorageInventoryWidget.h"

#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Interface/ComponentManager.h"
#include "Interface/ObjectInteraction.h"
#include "Interface/WidgetManager.h"
#include "Inventory/Components/InventoryComponent.h"
#include "Inventory/Components/InventoryCoreComponent.h"
#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"
#include "Inventory/UI/Storage/StorageSlotWidget.h"

UStorageInventoryWidget::UStorageInventoryWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UStorageInventoryWidget::NativePreConstruct()
{
	DisplaySampleSlots(SampleSlotsToDisplay);
}

void UStorageInventoryWidget::NativeConstruct()
{
	PlayConstructAnimation();
	
	APlayerController* LocalPC = GetOwningPlayer();
	IComponentManager* ComponentManager = Cast<IComponentManager>(LocalPC);
	if (LocalPC && ComponentManager)
	{
		PlayerInventory = ComponentManager->GetInventoryComponent();
	}
	SetOwnerInventory();
	if(OwnerInventory)
	{
		RefreshInventoryHandle = OwnerInventory->OnRefreshInventory.AddUObject(this,&UStorageInventoryWidget::RefreshInventory);
		HighlightInventorySlotHandle = OwnerInventory->OnHighlightInventorySlot.AddUObject(this,&UStorageInventoryWidget::HighlightSlot);
		SwitchedActivePanelHandle = OwnerInventory->OnSwitchedActivePanel.AddUObject(this,&UStorageInventoryWidget::SwitchInventoryPanel);
		CreateInventoryPanel(EInventoryPanel::P1);		
	}
}

void UStorageInventoryWidget::NativeDestruct()
{
	if(OwnerInventory)
	{
		OwnerInventory->OnRefreshInventory.Remove(RefreshInventoryHandle);
		OwnerInventory->OnHighlightInventorySlot.Remove(HighlightInventorySlotHandle);
		OwnerInventory->OnSwitchedActivePanel.Remove(SwitchedActivePanelHandle);

		if(OwnerInventory->RefreshOnClosingWeight)
		{
			PlayerInventory->Server_SortInventory(OwnerInventory,ESortMethod::Quicksort,EInventoryPanel::P1,false);
		}
	}
}

FReply UStorageInventoryWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	SetFocusToSlot(FocusedSlot);
	return FReply::Handled();
}

void UStorageInventoryWidget::RefreshInventory(EInventoryPanel InPanel)
{
	CreateInventoryPanel(InPanel);
}


void UStorageInventoryWidget::SetOwnerInventory()
{
	APlayerController* LocalPC = GetOwningPlayer();
	if(LocalPC)
	{
		IComponentManager* ComponentManager = Cast<IComponentManager>(LocalPC);
		if(ComponentManager)
		{
			OwnerInventory = ComponentManager->GetInventoryComponent();
		}
	}
}

void UStorageInventoryWidget::OnSlotFocused(UUserWidget* InSlotWidget)
{
	FocusItemSlot(InSlotWidget);
}

void UStorageInventoryWidget::OnSlotUnfocused(UUserWidget* InSlotWidget)
{
	UnfocusItemSlot(InSlotWidget);
}

void UStorageInventoryWidget::OnSlotHovered(UUserWidget* InSlotWidget)
{
	HoverItemSlot(InSlotWidget);
}

void UStorageInventoryWidget::OnSlotUnhovered(UUserWidget* InSlotWidget)
{
	UnhoverItemSlot(InSlotWidget);
}

void UStorageInventoryWidget::OnSlotClicked(UUserWidget* InSlotWidget)
{
	ClickedItemSlot(InSlotWidget);
}

void UStorageInventoryWidget::OnSlotPressed(UUserWidget* InSlotWidget, FKey PressedKey)
{
	PressedItemSlot(InSlotWidget,PressedKey);
}

void UStorageInventoryWidget::OnSlotReleased(UUserWidget* InSlotWidget, FKey ReleasedKey)
{
	ReleasedItemSlot(InSlotWidget,ReleasedKey);
}

void UStorageInventoryWidget::SetFocusToSlot(int32 InSlotIndex)
{
	UWidget* ChildWidget = GetUniformGridFromPanel(GetActivePanel())->GetChildAt(InSlotIndex);
	if(ChildWidget)
	{
		FocusedSlot = InSlotIndex;
		ChildWidget->SetKeyboardFocus();
	}
}

void UStorageInventoryWidget::HighlightSlot(int32 InSlotIndex)
{
	UWidget* ChildWidget = GetUniformGridFromPanel(GetActivePanel())->GetChildAt(InSlotIndex);
	if(ChildWidget)
	{
		UStorageSlotWidget* LocalStorageSlotWidget = Cast<UStorageSlotWidget>(ChildWidget);
		if(LocalStorageSlotWidget)
		{
			LocalStorageSlotWidget->HighlightSlot();
		}
	}
}

void UStorageInventoryWidget::BuildInventorySlots(const TArray<FItemData>& Inventory, int32 Size,UUniformGridPanel* UniformGrid)
{
	UniformGrid->ClearChildren();

	for (int i = 0; i < Size ; ++i)
	{
		UStorageSlotWidget* LocalStorageSlotWidget = CreateWidget<UStorageSlotWidget>(GetOwningPlayer(),StorageSlotWidgetClass);
		LocalStorageSlotWidget->ParentWidgetRef = this;
		LocalStorageSlotWidget->ActorInventory = OwnerInventory;
		LocalStorageSlotWidget->PlayerInventory = PlayerInventory;
		LocalStorageSlotWidget->SlotIndex = i;
		LocalStorageSlotWidget->DraggedImageSize = DraggedImageSize;

		// Get Current Row
		int32 Row = UniformGrid->GetChildrenCount() / RowLength;
		// Get Current Column
		int32 Col = UniformGrid->GetChildrenCount() % RowLength;

		UUniformGridSlot* GridSlot = UniformGrid->AddChildToUniformGrid(LocalStorageSlotWidget,Row,Col);
		GridSlot->SetHorizontalAlignment(HAlign_Fill);
		GridSlot->SetVerticalAlignment(VAlign_Fill);
	}
}

EInventoryPanel UStorageInventoryWidget::GetActivePanel()
{
	if(Switcher_Panel->GetActiveWidget() == UniformGridPanel_Inventory)
	{
		return EInventoryPanel::P1;
	}
	// if other panels exist, add code
	return EInventoryPanel::P1;
}

UUniformGridPanel* UStorageInventoryWidget::GetUniformGridFromPanel(EInventoryPanel Panel)
{
	// if other panels exist, add code
	return UniformGridPanel_Inventory;
}

void UStorageInventoryWidget::SwitchInventoryPanel(EInventoryPanel NewPanel)
{
	if(GetActivePanel() == NewPanel)
	{
		UInventoryUtilityLibrary::PlaySoundOnTabSwitched(GetWorld());
		Switcher_Panel->SetActiveWidget(GetUniformGridFromPanel(NewPanel));
		OwnerInventory->SwitchActivePanel(NewPanel);
	}
}

void UStorageInventoryWidget::AssignCurrentlyFocusedSlot(UUserWidget* InSlot)
{
	if(InSlot)
	{
		UUniformGridPanel* UniformGridPanel = GetUniformGridFromPanel(GetActivePanel());
		if(UniformGridPanel)
		{
			FocusedSlot = UniformGridPanel->GetChildIndex(InSlot);
		}
	}
}

void UStorageInventoryWidget::DisplaySampleSlots(int32 InSlots)
{
	UUniformGridPanel* LocalUniformGridPanel = GetUniformGridFromPanel(EInventoryPanel::P1);
	if(!LocalUniformGridPanel)
	{
		return;
	}
	
	LocalUniformGridPanel->ClearChildren();

	int Row = 0;
	int Col = 0;
	for (int i = 0; i < InSlots; ++i)
	{
		UStorageSlotWidget* LocalStorageSlotWidget = CreateWidget<UStorageSlotWidget>(GetOwningPlayer(),StorageSlotWidgetClass);
		LocalStorageSlotWidget->ParentWidgetRef = this;

		UUniformGridSlot* GridSlot = LocalUniformGridPanel->AddChildToUniformGrid(LocalStorageSlotWidget,Row,Col);
		GridSlot->SetHorizontalAlignment(HAlign_Fill);
		GridSlot->SetVerticalAlignment(VAlign_Fill);

		if(++Col >= RowLength)
		{
			Col = 0;
			++Row;
		}
	}
}

void UStorageInventoryWidget::ClickedItemSlot(UUserWidget* InSlotWidget)
{
	UStorageSlotWidget* LocalStorageSlotWidget = Cast<UStorageSlotWidget>(InSlotWidget);
	APlayerController* LocalPC = GetOwningPlayer();
	if(!LocalStorageSlotWidget || !LocalPC || !PlayerInventory || !OwnerInventory)
	{
		return;
	}
	FItemData LocalSlotItemData = LocalStorageSlotWidget->ItemData;
	if(UInventoryUtilityLibrary::IsStackableAndHaveStacks(LocalSlotItemData,1))
	{
		// Show Split Stack Widget [On Accept - 'On Splitting Accepted' will be called]
		IWidgetManager* WidgetManager = Cast<IWidgetManager>(LocalPC);
		if(WidgetManager)
		{
			WidgetManager->OpenSplitStackPopup(
				OwnerInventory,
				PlayerInventory,
				LocalSlotItemData,
				FItemData{},
				EInputMethod::RightClick,
				EItemDestination::StorageSlot,
				EItemDestination::InventorySlot,
				this);
		}
	}
	else
	{
		// SERVER: Transfer Item from Storage to Player Inventory
		PlayerInventory->Server_TransferItemFromInventory(
			PlayerInventory,
			OwnerInventory,
			LocalSlotItemData,
			FItemData{},
			EInputMethod::RightClick,
			LocalPC);
	}
}

void UStorageInventoryWidget::FocusItemSlot(UUserWidget* InSlotWidget)
{
	UStorageSlotWidget* LocalStorageSlotWidget = Cast<UStorageSlotWidget>(InSlotWidget);
	if(!LocalStorageSlotWidget)
	{
		return;
	}
	AssignCurrentlyFocusedSlot(LocalStorageSlotWidget);
	LocalStorageSlotWidget->CreateItemTooltip();
}

void UStorageInventoryWidget::UnfocusItemSlot(UUserWidget* InSlotWidget)
{
	// 
}

void UStorageInventoryWidget::HoverItemSlot(UUserWidget* InSlotWidget)
{
	UStorageSlotWidget* LocalStorageSlotWidget = Cast<UStorageSlotWidget>(InSlotWidget);
	APlayerController* LocalPC = GetOwningPlayer();
	if(!LocalStorageSlotWidget || !LocalPC)
	{
		return;
	}
	LocalStorageSlotWidget->SetUserFocus(LocalPC);
	LocalStorageSlotWidget->CreateItemTooltip();
}

void UStorageInventoryWidget::UnhoverItemSlot(UUserWidget* InSlotWidget)
{
	// 
}

void UStorageInventoryWidget::PressedItemSlot(UUserWidget* InSlotWidget, FKey PressedKey)
{
	UStorageSlotWidget* LocalStorageSlotWidget = Cast<UStorageSlotWidget>(InSlotWidget);
	if(LocalStorageSlotWidget)
	{
		
	}
}

void UStorageInventoryWidget::ReleasedItemSlot(UUserWidget* InSlotWidget, FKey PressedKey)
{
	UStorageSlotWidget* LocalStorageSlotWidget = Cast<UStorageSlotWidget>(InSlotWidget);
	if(LocalStorageSlotWidget)
	{
		
	}
}

void UStorageInventoryWidget::CreateInventoryPanel(EInventoryPanel InPanel)
{
	if(!OwnerInventory)
	{
		return;
	}
	
	int LocalInventorySize = 0;
	TArray<FItemData> LocalInventory{};
	OwnerInventory->GetInventoryAndSize(InPanel, LocalInventory, LocalInventorySize);
	BuildInventorySlots(LocalInventory,LocalInventorySize,UniformGridPanel_Inventory);
}


