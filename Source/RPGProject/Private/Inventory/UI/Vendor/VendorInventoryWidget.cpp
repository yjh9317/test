// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/Vendor/VendorInventoryWidget.h"

#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Interface/ComponentManager.h"
#include "Interface/WidgetManager.h"
#include "Inventory/Components/InventoryComponent.h"
#include "Inventory/Components/InventoryCoreComponent.h"
#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"
#include "Inventory/UI/Vendor/VendorSlotWidget.h"

void UVendorInventoryWidget::NativePreConstruct()
{
	DisplaySampleSlots(SampleSlotsToDisplay);
}

void UVendorInventoryWidget::NativeConstruct()
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
		RefreshInventoryHandle = OwnerInventory->OnRefreshInventory.AddUObject(this,&UVendorInventoryWidget::RefreshInventory);
		HighlightInventorySlotHandle = OwnerInventory->OnHighlightInventorySlot.AddUObject(this,&UVendorInventoryWidget::HighlightSlot);
		SwitchedActivePanelHandle = OwnerInventory->OnSwitchedActivePanel.AddUObject(this,&UVendorInventoryWidget::SwitchInventoryPanel);
		CreateInventoryPanel(EInventoryPanel::P1);		
	}
}

void UVendorInventoryWidget::NativeDestruct()
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

FReply UVendorInventoryWidget::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	SetFocusToSlot(FocusedSlot);
	return FReply::Handled();
}

void UVendorInventoryWidget::RefreshInventory(EInventoryPanel InPanel)
{
	CreateInventoryPanel(InPanel);
}

void UVendorInventoryWidget::SetOwnerInventory()
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

void UVendorInventoryWidget::OnSlotFocused(UUserWidget* InSlotWidget)
{
	FocusItemSlot(InSlotWidget);
}

void UVendorInventoryWidget::OnSlotUnfocused(UUserWidget* InSlotWidget)
{
	UnfocusItemSlot(InSlotWidget);
}

void UVendorInventoryWidget::OnSlotHovered(UUserWidget* InSlotWidget)
{
	HoverItemSlot(InSlotWidget);
}

void UVendorInventoryWidget::OnSlotUnhovered(UUserWidget* InSlotWidget)
{
	UnhoverItemSlot(InSlotWidget);
}

void UVendorInventoryWidget::OnSlotClicked(UUserWidget* InSlotWidget)
{
	ClickedItemSlot(InSlotWidget);
}

void UVendorInventoryWidget::OnSlotPressed(UUserWidget* InSlotWidget, FKey PressedKey)
{
	PressedItemSlot(InSlotWidget,PressedKey);
}

void UVendorInventoryWidget::OnSlotReleased(UUserWidget* InSlotWidget, FKey ReleasedKey)
{
	ReleasedItemSlot(InSlotWidget,ReleasedKey);
}

void UVendorInventoryWidget::SetFocusToSlot(int32 InSlotIndex)
{
	UWidget* ChildWidget = GetUniformGridFromPanel(GetActivePanel())->GetChildAt(InSlotIndex);
	if(ChildWidget)
	{
		FocusedSlot = InSlotIndex;
		ChildWidget->SetKeyboardFocus();
	}
}

void UVendorInventoryWidget::HighlightSlot(int32 InSlotIndex)
{
	UWidget* ChildWidget = GetUniformGridFromPanel(GetActivePanel())->GetChildAt(InSlotIndex);
	if(ChildWidget)
	{
		UVendorSlotWidget* LocalVendorSlotWidget = Cast<UVendorSlotWidget>(ChildWidget);
		if(LocalVendorSlotWidget)
		{
			LocalVendorSlotWidget->HighlightSlot();
		}
	}
}

void UVendorInventoryWidget::BuildInventorySlots(const TArray<FItemData>& Inventory, int32 Size,UUniformGridPanel* UniformGrid)
{
	UniformGrid->ClearChildren();

	for (int i = 0; i < Size ; ++i)
	{
		UVendorSlotWidget* LocalVendorSlotWidget = CreateWidget<UVendorSlotWidget>(GetOwningPlayer(),VendorSlotWidgetClass);
		LocalVendorSlotWidget->ParentWidgetRef = this;
		LocalVendorSlotWidget->ActorInventory = OwnerInventory;
		LocalVendorSlotWidget->PlayerInventory = PlayerInventory;
		LocalVendorSlotWidget->SlotIndex = i;
		LocalVendorSlotWidget->DraggedImageSize = DraggedImageSize;

		// Get Current Row
		int32 Row = UniformGrid->GetChildrenCount() / RowLength;
		// Get Current Column
		int32 Col = UniformGrid->GetChildrenCount() % RowLength;

		UUniformGridSlot* GridSlot = UniformGrid->AddChildToUniformGrid(LocalVendorSlotWidget,Row,Col);
		GridSlot->SetHorizontalAlignment(HAlign_Fill);
		GridSlot->SetVerticalAlignment(VAlign_Fill);
	}
}

EInventoryPanel UVendorInventoryWidget::GetActivePanel()
{
	if(Switcher_Panel->GetActiveWidget() == UniformGridPanel_Inventory)
	{
		return EInventoryPanel::P1;
	}
	// if other panels exist, add code
	return EInventoryPanel::P1;
}

UUniformGridPanel* UVendorInventoryWidget::GetUniformGridFromPanel(EInventoryPanel Panel)
{
	// if other panels exist, add code
	return UniformGridPanel_Inventory;
}

void UVendorInventoryWidget::SwitchInventoryPanel(EInventoryPanel NewPanel)
{
	if(GetActivePanel() == NewPanel)
	{
		UInventoryUtilityLibrary::PlaySoundOnTabSwitched(GetWorld());
		Switcher_Panel->SetActiveWidget(GetUniformGridFromPanel(NewPanel));
		OwnerInventory->SwitchActivePanel(NewPanel);
	}
}

void UVendorInventoryWidget::AssignCurrentlyFocusedSlot(UUserWidget* InSlot)
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

void UVendorInventoryWidget::DisplaySampleSlots(int32 InSlots)
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
		UVendorSlotWidget* LocalVendorSlotWidget = CreateWidget<UVendorSlotWidget>(GetOwningPlayer(),VendorSlotWidgetClass);
		LocalVendorSlotWidget->ParentWidgetRef = this;

		UUniformGridSlot* GridSlot = LocalUniformGridPanel->AddChildToUniformGrid(LocalVendorSlotWidget,Row,Col);
		GridSlot->SetHorizontalAlignment(HAlign_Fill);
		GridSlot->SetVerticalAlignment(VAlign_Fill);

		if(++Col >= RowLength)
		{
			Col = 0;
			++Row;
		}
	}
}

void UVendorInventoryWidget::ClickedItemSlot(UUserWidget* InSlotWidget)
{
	UVendorSlotWidget* LocalVendorSlotWidget = Cast<UVendorSlotWidget>(InSlotWidget);
	APlayerController* LocalPC = GetOwningPlayer();
	if(!LocalVendorSlotWidget || !LocalPC || !PlayerInventory || !OwnerInventory)
	{
		return;
	}
	FItemData LocalSlotItemData = LocalVendorSlotWidget->ItemData;
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
				EItemDestination::VendorSlot,
				EItemDestination::InventorySlot,
				this);
		}
	}
	else
	{
		// SERVER: Transfer Item from Vendor to Player Inventory
		PlayerInventory->Server_TransferItemFromInventory(
			PlayerInventory,
			OwnerInventory,
			LocalSlotItemData,
			FItemData{},
			EInputMethod::RightClick,
			LocalPC);
	}
}

void UVendorInventoryWidget::FocusItemSlot(UUserWidget* InSlotWidget)
{
	UVendorSlotWidget* LocalVendorSlotWidget = Cast<UVendorSlotWidget>(InSlotWidget);
	if(!LocalVendorSlotWidget)
	{
		return;
	}
	AssignCurrentlyFocusedSlot(LocalVendorSlotWidget);
	LocalVendorSlotWidget->CreateItemTooltip();
}

void UVendorInventoryWidget::UnfocusItemSlot(UUserWidget* InSlotWidget)
{
	// 
}

void UVendorInventoryWidget::HoverItemSlot(UUserWidget* InSlotWidget)
{
	UVendorSlotWidget* LocalVendorSlotWidget = Cast<UVendorSlotWidget>(InSlotWidget);
	APlayerController* LocalPC = GetOwningPlayer();
	if(!LocalVendorSlotWidget || !LocalPC)
	{
		return;
	}
	LocalVendorSlotWidget->SetUserFocus(LocalPC);
	LocalVendorSlotWidget->CreateItemTooltip();
}

void UVendorInventoryWidget::UnhoverItemSlot(UUserWidget* InSlotWidget)
{
	// 
}

void UVendorInventoryWidget::PressedItemSlot(UUserWidget* InSlotWidget, FKey PressedKey)
{
	UVendorSlotWidget* LocalVendorSlotWidget = Cast<UVendorSlotWidget>(InSlotWidget);
	if(LocalVendorSlotWidget)
	{
		
	}
}

void UVendorInventoryWidget::ReleasedItemSlot(UUserWidget* InSlotWidget, FKey PressedKey)
{
	UVendorSlotWidget* LocalVendorSlotWidget = Cast<UVendorSlotWidget>(InSlotWidget);
	if(LocalVendorSlotWidget)
	{
		
	}
}

void UVendorInventoryWidget::CreateInventoryPanel(EInventoryPanel InPanel)
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
