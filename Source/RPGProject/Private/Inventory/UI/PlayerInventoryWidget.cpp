// Fill out your copyright notice in the Description page of Project Settings.


#include "Inventory/UI/PlayerInventoryWidget.h"

#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Components/WidgetSwitcher.h"
#include "Header/DebugHeader.h"
#include "Interface/ComponentManager.h"
#include "Interface/FocusWidgetHandler.h"
#include "Interface/ObjectInteraction.h"
#include "Interface/WidgetManager.h"
#include "Inventory/Components/EquipmentComponent.h"
#include "Inventory/Components/InventoryComponent.h"
#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"
#include "Inventory/UI/PlayerInventorySlotWidget.h"
#include "Inventory/UI/Panel/InventoryPanelButtonWidget.h"

UPlayerInventoryWidget::UPlayerInventoryWidget(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
}

void UPlayerInventoryWidget::BindInventoryDelegates()
{
	if (!InventoryComponent->OnRefreshInventory.IsBoundToObject(this))
	{
		InventoryComponent->OnRefreshInventory.AddUObject(
			this, &UPlayerInventoryWidget::RefreshInventory);
	}
    
	if (!InventoryComponent->OnHighlightInventorySlot.IsBoundToObject(this))
	{
		InventoryComponent->OnHighlightInventorySlot.AddUObject(
			this, &UPlayerInventoryWidget::HighlightSlot);
	}
    
	if (!InventoryComponent->OnSwitchedActivePanel.IsBoundToObject(this))
	{
		InventoryComponent->OnSwitchedActivePanel.AddUObject(
			this, &UPlayerInventoryWidget::SwitchInventoryPanel);
	}
}

void UPlayerInventoryWidget::NativeConstruct()
{	
	Super::NativeConstruct();
	
	IComponentManager* ComponentManager = Cast<IComponentManager>(GetOwningPlayer());
	if (!ComponentManager)
	{
		return;
	}
    
	InventoryComponent = ComponentManager->GetInventoryComponent();
	EquipmentComponent = ComponentManager->GetEquipmentComponent();
    
	if (!InventoryComponent)
	{
		return;
	}

	BindInventoryDelegates();
	SetReceiverInventory();
	    
	// 패널 생성
	CreateAllInventoryPanels();
    
	// 활성 패널 설정
	SetActivePanel();
	
}

void UPlayerInventoryWidget::NativeDestruct()
{
	if (InventoryComponent)
	{
		InventoryComponent->OnRefreshInventory.RemoveAll(this);
		InventoryComponent->OnHighlightInventorySlot.RemoveAll(this);
		InventoryComponent->OnSwitchedActivePanel.RemoveAll(this);
	}
    
	Super::NativeDestruct();
}

void UPlayerInventoryWidget::SetFocusToSlot(int32 SlotIndex)
{
	if(IsAnyPopupActive())
	{
		EInventoryPanel Panel =GetActivePanel();
		UUniformGridPanel* GridPanel = GetUniformGridFromPanel(Panel);
		if(UWidget* ChildWidget = GridPanel->GetChildAt(SlotIndex))
		{
			FocusedSlot = SlotIndex;
			ChildWidget->SetKeyboardFocus();
		}
	}
}

void UPlayerInventoryWidget::HighlightSlot(int32 SlotIndex)
{
	UUniformGridPanel* GridPanel = GetUniformGridFromPanel(GetActivePanel());
	
	if(UPlayerInventorySlotWidget* SlotWidget = Cast<UPlayerInventorySlotWidget>(GridPanel->GetChildAt(SlotIndex)))
	{
		SlotWidget->HighlightSlot();
	}
}

void UPlayerInventoryWidget::BuildInventorySlots(const TArray<FItemData>& Inventory, int32 Size,
	UUniformGridPanel* UniformGridPanel)
{
	if (!UniformGridPanel || !InventoryComponent)
	{ 
		return;
	}
	
	UniformGridPanel->ClearChildren();

	int32 ActualSize = InventoryComponent->UseInventorySize ? InventoryComponent->InventoryDefaultSize : FMath::Min(Size, Inventory.Num());
	for (int32 i = 0; i < ActualSize; ++i)
	{
		UPlayerInventorySlotWidget* SlotWidget = CreateWidget<UPlayerInventorySlotWidget>(
			GetOwningPlayer(), 
			InventorySlotWidgetClass);
        
		if (!SlotWidget)
		{
			continue;
		}
		
		SlotWidget->ReceiverInventory = ReceiverInventoryComponent;
		SlotWidget->PlayerEquipment = EquipmentComponent;
		SlotWidget->PlayerInventory = InventoryComponent;
		SlotWidget->ParentWidget = this;
		SlotWidget->DraggedImageSize = DraggedImageSize;
		SlotWidget->SlotIndex = i;
 
		if (Inventory.IsValidIndex(i))
		{
			SlotWidget->ItemData = Inventory[i];
		}
		else
		{
			SlotWidget->ItemData = FItemData();
			SlotWidget->ItemData.Index = i;
		}
        
		// 그리드에 추가
		int32 Row = i / RowLength;
		int32 Column = i % RowLength;
        
		UUniformGridSlot* GridSlot = UniformGridPanel->AddChildToUniformGrid(
			SlotWidget, Row, Column);
        
		if (GridSlot)
		{
			GridSlot->SetHorizontalAlignment(HAlign_Fill);
			GridSlot->SetVerticalAlignment(VAlign_Fill);
		}
	}
}

EInventoryPanel UPlayerInventoryWidget::GetActivePanel() const
{
	if(PanelSwitcher->GetActiveWidget() == UniformGridPanel_Slot1)
	{
		return EInventoryPanel::P1;
	}
	else if(PanelSwitcher->GetActiveWidget() == UniformGridPanel_Slot2)
	{
		return EInventoryPanel::P2;
	}
	else if(PanelSwitcher->GetActiveWidget() == UniformGridPanel_Slot3)
	{
		return EInventoryPanel::P3; 
	}
	else if(PanelSwitcher->GetActiveWidget() == UniformGridPanel_Slot4)
	{
		return EInventoryPanel::P4; 
	}
	return EInventoryPanel::P1;
}

EInventoryPanel UPlayerInventoryWidget::GetEnumByIndex(int32 Index) const
{
	const int32 EnumCount = StaticEnum<EInventoryPanel>()->NumEnums() - 1; 
	
	if (Index < 0 || Index >= EnumCount)
	{
		return EInventoryPanel::NONE; 
	}
    
	return static_cast<EInventoryPanel>(Index);
}

UUniformGridPanel* UPlayerInventoryWidget::GetUniformGridFromPanel(EInventoryPanel InPanel) const
{
	UUniformGridPanel* UniformGridPanel = nullptr;
	switch (InPanel)
	{
	case EInventoryPanel::P1:
		UniformGridPanel = UniformGridPanel_Slot1;
		break;
	case EInventoryPanel::P2:
		UniformGridPanel = UniformGridPanel_Slot2;
		break;
	case EInventoryPanel::P3:
		UniformGridPanel = UniformGridPanel_Slot3;
		break;
	case EInventoryPanel::P4:
		UniformGridPanel = UniformGridPanel_Slot4;
		break;
	default:
		break;
	}
	return UniformGridPanel;
}

void UPlayerInventoryWidget::SwitchInventoryPanel(EInventoryPanel NewPanel)
{
	if(GetActivePanel() == NewPanel)
	{
		return;
	}
	UInventoryUtilityLibrary::PlaySoundOnTabSwitched(GetWorld());
	PanelSwitcher->SetActiveWidget(GetUniformGridFromPanel(NewPanel));
	InventoryComponent->SwitchActivePanel(NewPanel);

	{
		Button_FirstPanel->RefreshPanelButton(NewPanel);
		Button_SecondPanel->RefreshPanelButton(NewPanel);
		Button_ThirdPanel->RefreshPanelButton(NewPanel);
		Button_FourthPanel->RefreshPanelButton(NewPanel);
	}
	SetPanelTitle(NewPanel);
	ScrollBox_InventoryPanel->ScrollToStart();
}

void UPlayerInventoryWidget::AssignCurrentlyFocusedSlot(UUserWidget* SlotWidget)
{
	if(!SlotWidget) return;

	UUniformGridPanel* GridPanel = GetUniformGridFromPanel(GetActivePanel());
	FocusedSlot = GridPanel->GetChildIndex(SlotWidget);
}

void UPlayerInventoryWidget::SwitchToNextInventoryPanel(bool ToTheRight)
{
	EInventoryPanel NewPanel = GetActivePanel();
	if(ToTheRight)
	{
		switch (NewPanel)
		{
		case EInventoryPanel::P1:
			SwitchInventoryPanel(EInventoryPanel::P2);
			break;
		case EInventoryPanel::P2:
			SwitchInventoryPanel(EInventoryPanel::P3);
			break;
		case EInventoryPanel::P3:
			SwitchInventoryPanel(EInventoryPanel::P4);
			break;
		case EInventoryPanel::P4:
			SwitchInventoryPanel(EInventoryPanel::P1);
			break;
		}	
	}
	else
	{
		switch (NewPanel)
		{
		case EInventoryPanel::P1:
			SwitchInventoryPanel(EInventoryPanel::P4);
			break;
		case EInventoryPanel::P2:
			SwitchInventoryPanel(EInventoryPanel::P1);
			break;
		case EInventoryPanel::P3:
			SwitchInventoryPanel(EInventoryPanel::P2);
			break;
		case EInventoryPanel::P4:
			SwitchInventoryPanel(EInventoryPanel::P3);
			break;
		}	
	}	
}


void UPlayerInventoryWidget::SetPanelTitle(EInventoryPanel InPanel)
{
	switch (InPanel)
	{
	case EInventoryPanel::P1:
		TextBlock_PanelName->SetText(FirstPanelTitle);
		break;
	case EInventoryPanel::P2:
		TextBlock_PanelName->SetText(SecondPanelTitle);
		break;
	case EInventoryPanel::P3:
		TextBlock_PanelName->SetText(ThirdPanelTitle);
		break;
	case EInventoryPanel::P4:
		TextBlock_PanelName->SetText(FourthPanelTitle);
		break;
	}
}

void UPlayerInventoryWidget::DisplaySampleSlots(int32 Slots)
{
	UUniformGridPanel* UniformGridPanel = GetUniformGridFromPanel(EInventoryPanel::P1);
	UniformGridPanel->ClearChildren();

	int LocalRow = 0;
	int LocalColumn = 0;
	
	for(int index = 1; index <Slots; ++index)
	{
		UUserWidget* SlotWidget = CreateWidget(GetWorld(),InventorySlotWidgetClass);

		UUniformGridSlot* GridSlot = UniformGridPanel->AddChildToUniformGrid(SlotWidget,LocalRow,LocalColumn);
		GridSlot->SetHorizontalAlignment(HAlign_Fill);
		GridSlot->SetVerticalAlignment(VAlign_Fill);

		LocalColumn++;

		if(LocalColumn >= RowLength)
		{
			LocalColumn = 0;
			LocalRow++;
		}
	}
}

bool UPlayerInventoryWidget::IsAnyPopupActive()
{
	if(!GetOwningPlayer())
	{
		return false;
	}
	if(IWidgetManager* WidgetManager = Cast<IWidgetManager>(GetOwningPlayer()))
	{
		if(WidgetManager->GetActivePopup() != EWidgetPopup::None)
		{
			return true;
		}
	}
	return false;
}

void UPlayerInventoryWidget::OnSlotClicked(UUserWidget* SlotWidget)
{
	UPlayerInventorySlotWidget* LocalSlotWidget = Cast<UPlayerInventorySlotWidget>(SlotWidget);
	IWidgetManager* WidgetManager = Cast<IWidgetManager>(GetOwningPlayer());
	if(!LocalSlotWidget || !WidgetManager) return;

	EWidgetType WidgetType = WidgetManager->GetActiveWidget();
	const FItemData& SlotItemData = LocalSlotWidget->ItemData;
	
	switch (WidgetType)
	{
	case EWidgetType::INVENTORY:
	case EWidgetType::EQUIPMENT:
		{
			if(SlotItemData.EquipmentSlot != EItemSlot::NONE)
			{
				EquipmentComponent->ServerTransferItemFromInventory(InventoryComponent,SlotItemData,{},EInputMethod::RightClick);
			}
			else
			{
				TryToUseAnItem(SlotItemData);
			}
		}
		break;
	case EWidgetType::VENDOR:
	case EWidgetType::STORAGE:
		{
			if(EWidgetType::EQUIPMENT == WidgetManager->GetActiveTab())
			{
				if(SlotItemData.EquipmentSlot != EItemSlot::NONE)
				{
					EquipmentComponent->ServerTransferItemFromInventory(InventoryComponent,SlotItemData,{},EInputMethod::RightClick);
				}
				else
				{
					TryToUseAnItem(SlotItemData);
				}
			}
			else
			{
				EItemRemoveType RemoveType = InventoryComponent->GetItemRemoveType(SlotItemData);
				switch (RemoveType)
				{
				case EItemRemoveType::Default:
					if(UInventoryUtilityLibrary::IsStackableAndHaveStacks(SlotItemData,1))
					{
						WidgetManager->OpenSplitStackPopup(InventoryComponent,ReceiverInventoryComponent,
							SlotItemData,{},EInputMethod::RightClick,
							EItemDestination::InventorySlot,EItemDestination::VendorSlot,this);	
					}
					else
					{
						InventoryComponent->Server_TransferItemFromInventory(ReceiverInventoryComponent,InventoryComponent,
							SlotItemData,{},
							EInputMethod::RightClick,GetOwningPlayer());
					}
					break;
				case EItemRemoveType::OnConfirmation:
					WidgetManager->OpenConfirmationPopup(FText::FromString("Are you sure you want to remove?"),
						InventoryComponent,ReceiverInventoryComponent,SlotItemData,{},
						EInputMethod::RightClick,EItemDestination::InventorySlot,EItemDestination::VendorSlot,this);
					break;
				case EItemRemoveType::CannotBeRemoved:
					WidgetManager->DisplayMessageNotify(FText::FromString("Item cannot be Removed."));
					break;
				}
			}
		}
		break;
	}
}

void UPlayerInventoryWidget::OnSlotFocused(UUserWidget* SlotWidget)
{
	UPlayerInventorySlotWidget* LocalSlotWidget = Cast<UPlayerInventorySlotWidget>(SlotWidget);
	if(LocalSlotWidget)
	{
		AssignCurrentlyFocusedSlot(LocalSlotWidget);
		LocalSlotWidget->CreateItemTooltip();
	}
}

void UPlayerInventoryWidget::OnSlotHovered(UUserWidget* SlotWidget)
{
	UPlayerInventorySlotWidget* LocalSlotWidget = Cast<UPlayerInventorySlotWidget>(SlotWidget);
	if(LocalSlotWidget && GetOwningPlayer())
	{
		LocalSlotWidget->SetUserFocus(GetOwningPlayer());
		LocalSlotWidget->CreateItemTooltip();
	}
}


void UPlayerInventoryWidget::SetReceiverInventory()
{
	IObjectInteraction* ObjectInteraction = Cast<IObjectInteraction>(GetOwningPlayer());
	AActor* CurObject = ObjectInteraction->GetCurrentInteractableObject();
	if(CurObject)
	{
		if(UInventoryCoreComponent* CoreComponent = CurObject->FindComponentByClass<UInventoryCoreComponent>())
		{
			ReceiverInventoryComponent = CoreComponent;
		}
	}
}

void UPlayerInventoryWidget::TryToUseAnItem(FItemData InItemData)
{
	if(!InItemData.ItemClass)
	{
		return;
	}
	
	UItemBase* ItemBase = Cast<UItemBase>(InItemData.ItemClass->GetDefaultObject());
	IWidgetManager* WidgetManager = Cast<IWidgetManager>(GetOwningPlayer());
	if (!ItemBase || !WidgetManager) 
	{
		DebugHeader::LogWarning(TEXT("TryToUseAnItem : ItemBase or WidgetManageris null"));
		return;
	}
	if(ItemBase->UseType == EItemUseType::TextDocument)
	{
		InItemData.AlreadyUsed = true;
		InventoryComponent->Server_AddItemToInventory(InventoryComponent,InItemData,InItemData.Index);
		WidgetManager->OpenTextDocumentPopup(InItemData,this);
	}
}

void UPlayerInventoryWidget::CreateAllInventoryPanels()
{
	if (!InventoryComponent) return;
    
	for (const auto& Panel : InventoryComponent->PanelsToUse)
	{
		CreateInventoryPanel(Panel);
	}
}

void UPlayerInventoryWidget::CreateInventoryPanel(EInventoryPanel Panel)
{
	UUniformGridPanel* GridPanel = GetUniformGridFromPanel(Panel);
	if (!GridPanel) return;

	TArray<FItemData> LocalInventory;
	int32 LocalSize = 0;
	InventoryComponent->GetInventoryAndSize(Panel, LocalInventory, LocalSize);

	if (GridPanel->GetChildrenCount() == LocalSize)
	{
		for (int32 i = 0; i < LocalSize; ++i)
		{
			if (UPlayerInventorySlotWidget* SlotWidget = Cast<UPlayerInventorySlotWidget>(GridPanel->GetChildAt(i)))
			{
				if (LocalInventory.IsValidIndex(i))
				{
					SlotWidget->ItemData = LocalInventory[i];
					SlotWidget->RefreshSlotDisplay();
				}
			}
		}
	}
	else
	{
		BuildInventorySlots(LocalInventory, LocalSize, GridPanel);
	}
}

void UPlayerInventoryWidget::RefreshInventory(EInventoryPanel Panel)
{
	if (!InventoryComponent)
	{
		return;
	}

	if (!InventoryComponent->PanelsToUse.Contains(Panel))
	{
		return;
	}
    
	CreateInventoryPanel(Panel);
	ResetSlotFocus();
	// OnInventoryRefreshed.Broadcast(Panel);
}

void UPlayerInventoryWidget::OnSortingFinished()
{
}

void UPlayerInventoryWidget::ResetSlotFocus()
{
	
	IFocusWidgetHandler* FocusWidgetHandler = Cast<IFocusWidgetHandler>( GetOwningPlayer());
	if(FocusWidgetHandler)
	{
		EWidgetType WidgetType = FocusWidgetHandler->GetCurrentlyFocusedWidget();
		if(WidgetType == EWidgetType::INVENTORY)
		{
			SetFocusToSlot(FocusedSlot);
		}
	}
}

void UPlayerInventoryWidget::SetActivePanel()
{
	{
		// Bind
		if(!Button_FirstPanel->OnClickedButtonPanel.IsBound())
		{
			Button_FirstPanel->OnClickedButtonPanel.AddDynamic(this,&UPlayerInventoryWidget::SwitchInventoryPanel);
		}
		if(!Button_SecondPanel->OnClickedButtonPanel.IsBound())
		{
			Button_SecondPanel->OnClickedButtonPanel.AddDynamic(this,&UPlayerInventoryWidget::SwitchInventoryPanel);
		}
		if(!Button_ThirdPanel->OnClickedButtonPanel.IsBound())
		{
			Button_ThirdPanel->OnClickedButtonPanel.AddDynamic(this,&UPlayerInventoryWidget::SwitchInventoryPanel);
		}
		if(!Button_FourthPanel->OnClickedButtonPanel.IsBound())
		{
			Button_FourthPanel->OnClickedButtonPanel.AddDynamic(this,&UPlayerInventoryWidget::SwitchInventoryPanel);
		}
	}
	{
		// Refresh
		Button_FirstPanel->RefreshPanelButton(DefaultPanel);
		Button_SecondPanel->RefreshPanelButton(DefaultPanel);
		Button_ThirdPanel->RefreshPanelButton(DefaultPanel);
		Button_FourthPanel->RefreshPanelButton(DefaultPanel);
	}
	{
		// Set
		PanelSwitcher->SetActiveWidget(GetUniformGridFromPanel(DefaultPanel));
		InventoryComponent->SwitchActivePanel(DefaultPanel);
		SetPanelTitle(DefaultPanel);
	}
}




