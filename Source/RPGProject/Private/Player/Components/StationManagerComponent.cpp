// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Components/StationManagerComponent.h"

#include "Crafting/Component/BaseCraftingStation.h"
#include "GameFramework/PlayerController.h"
#include "Interface/ComponentManager.h"
#include "Interface/WidgetManager.h"
#include "Inventory/Components/EquipmentComponent.h"
#include "Inventory/Components/InventoryComponent.h"
#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"

void UStationManagerComponent::Client_InitializeStationsManager_Implementation(APlayerController* InPlayerController)
{
	PlayerController = InPlayerController;	
}

void UStationManagerComponent::EventStationDestroyed(AActor* InActor, EEndPlayReason::Type EndPlayReason)
{
	// Event Station End Play - when the Station would be (for some reason) destroyed (can no longer be referenced)
	CurrentStationActor = nullptr;
	OnStationActorChanged.Broadcast();
}

void UStationManagerComponent::Server_InitializeCraftingProcess_Implementation(UBaseCraftingStation* Sender,
	const FCraftingData& InCraftingData, int32 AmountToCraft, AActor* InOwningPlayer)
{
	FText LocalMessage;
	bool IsSuccess = TryToInitializeCraftingProcess(InCraftingData,AmountToCraft,Sender,InOwningPlayer,LocalMessage);

	FString LocalString = LocalMessage.ToString(); 
	Client_InitializeCraftingProcessReturnValue(Sender,InCraftingData,AmountToCraft,IsSuccess,LocalString);
}

void UStationManagerComponent::Server_TryToCraftItem_Implementation(UBaseCraftingStation* Sender,
	const FCraftingData& InCraftingData, AActor* InOwningPlayer)
{
	FText LocalMessage;
	bool IsSuccess = TryToCraftItem(InCraftingData,Sender,InOwningPlayer,LocalMessage);

	FString LocalString = LocalMessage.ToString();
	Client_TryToCraftReturnValue(Sender,IsSuccess,LocalString);
}

void UStationManagerComponent::Server_ReturnSecuredMaterials_Implementation(UBaseCraftingStation* Sender,
	const FCraftingData& InCraftingData, AActor* InOwningPlayer)
{
	ReturnSecuredMaterials(InCraftingData,InOwningPlayer);
	ReturnSecuredCraftingCost(InCraftingData,Sender,InOwningPlayer);
	Client_RefreshUIData(Sender);
}

void UStationManagerComponent::Client_UnlockCraftingRecipe_Implementation(const FString& InCraftingID)
{
	UnlockCraftingRecipe(InCraftingID);
	
}

void UStationManagerComponent::Client_TryToCraftReturnValue_Implementation(UBaseCraftingStation* Sender,
	bool ReturnValue,const FString& Message)
{
	if(!PlayerController) return;
	
	if(ReturnValue)
	{
		Sender->EventItemCrafted(PlayerController);
	}
	else
	{
		IWidgetManager* WidgetManager = Cast<IWidgetManager>(PlayerController);
		if(WidgetManager)
		{
			WidgetManager->DisplayMessageNotify(FText::FromString(Message));
			Sender->StopCraftingProcess(Sender->GetFirstItemFromQueue(),PlayerController);
		}
	}
}

void UStationManagerComponent::Client_InitializeCraftingProcessReturnValue_Implementation(UBaseCraftingStation* Sender,
	const FCraftingData& InCraftingData, int32 AmountToCraft, bool ReturnValue,const FString& Message)
{
	if(ReturnValue)
	{
		Sender->InitializeCraftingProcess(InCraftingData,AmountToCraft);
		Sender->OnRefreshed.Broadcast();
	}
	else
	{
		IWidgetManager* WidgetManager = Cast<IWidgetManager>(PlayerController);
		if(WidgetManager)
		{
			WidgetManager->DisplayMessageNotify(FText::FromString(Message));
			Sender->EventFailedToInitializeCraftingProcess(Sender->GetFirstItemFromQueue(),AmountToCraft);
		}
	}
}

void UStationManagerComponent::Client_RefreshUIData_Implementation(UBaseCraftingStation* Target)
{
	Target->OnRefreshed.Broadcast();
}

void UStationManagerComponent::SetCurrentCraftingStationActor(AActor* StationActor)
{
	if(StationActor != CurrentStationActor)
	{
		UnbindStationEndPlayEvent();
		CurrentStationActor = StationActor;
		BindStationEndPlayEvent();
	}
}

AActor* UStationManagerComponent::GetCurrentCraftingStationActor() const
{
	return CurrentStationActor;
}

void UStationManagerComponent::BindStationEndPlayEvent()
{
	if(CurrentStationActor)
	{
		CurrentStationActor->OnEndPlay.AddDynamic(this,&UStationManagerComponent::EventStationDestroyed);
	}
}

void UStationManagerComponent::UnbindStationEndPlayEvent()
{
	if(CurrentStationActor)
	{
		CurrentStationActor->OnEndPlay.RemoveDynamic(this,&UStationManagerComponent::EventStationDestroyed);
	}
}

bool UStationManagerComponent::TryToInitializeCraftingProcess(const FCraftingData& InCraftingData, int32 AmountToCraft,
	UBaseCraftingStation* Sender, AActor* InOwningPlayer, FText& FailureMessage)
{
	if(IsCraftingRecipeLocked(InCraftingData))
	{
		FailureMessage = FText::FromString(TEXT("Recipe is Locked."));
		return false;
	}
	
	if(!Sender || !Sender->CanInitializeCraftingProcess(InCraftingData,AmountToCraft,InOwningPlayer,FailureMessage))
	{
		return false;
	}

	TArray<FItemData> LocalCraftableItemArray;
	TArray<FItemStack> LocalMaterialsArray;
	UInventoryUtilityLibrary::GetCraftableData(InCraftingData,LocalCraftableItemArray,LocalMaterialsArray);
	if(!CanBeAddedToInventory(LocalCraftableItemArray,InOwningPlayer,Sender))
	{
		FailureMessage = FText::FromString(TEXT("Your Inventory is full."));
		return false;
	}

	TArray<FItemData> LocalSecuredMaterials;
	if(!SecureMaterialItems(LocalMaterialsArray,AmountToCraft,InOwningPlayer,LocalSecuredMaterials))
	{
		FailureMessage = FText::FromString(TEXT("Not enough materials."));
		return false;
	}

	SecureCraftingCost(InCraftingData,AmountToCraft,Sender,InOwningPlayer);

	return true;
}

bool UStationManagerComponent::TryToCraftItem(const FCraftingData& InCraftingData, UBaseCraftingStation* Sender,
	AActor* InOwningPlayer, FText& FailureMessage)
{
	if(!UInventoryUtilityLibrary::IsCraftingDataValid(InCraftingData) || !Sender || !InOwningPlayer)
	{
		return false;
	}

	// Get Player Inventory Reference
	IComponentManager* ComponentManager = Cast<IComponentManager>(InOwningPlayer);
	if (!ComponentManager) return false;
	UInventoryComponent* LocalInventoryComponent = ComponentManager->GetInventoryComponent();
	if (!LocalInventoryComponent) return false;

	// Assign Craftable Item and Required Materials to local variables
	TArray<FItemData> LocalCraftableItemArray;
	TArray<FItemStack> LocalMaterialsArray;
	UInventoryUtilityLibrary::GetCraftableData(InCraftingData,LocalCraftableItemArray,LocalMaterialsArray);

	// Check #1 Should Item be Spawned?
	if(Sender->SpawnCraftedItem)
	{
		// Check #2 Can be added to Inventory? [If not Spawning]
		if(!CanBeAddedToInventory(LocalCraftableItemArray,InOwningPlayer,Sender))
		{
			FailureMessage = FText::FromString(TEXT("Inventory is full."));
			return false;
		}
		auto AddNewItem = [&](FItemData& ItemToAdd, int32 SlotIndex)
		{
			LocalInventoryComponent->AddItemToInventoryArray(ItemToAdd, SlotIndex);
			const float Weight = UInventoryUtilityLibrary::CalculateStackedItemWeight(ItemToAdd);
			LocalInventoryComponent->AddWeightToInventory(Weight);
		};
		// Add Crafted Items to Inventory
		for (auto CraftableItem : LocalCraftableItemArray)
		{
			if (UInventoryUtilityLibrary::IsStackableAndHaveStacks(CraftableItem, 1))
			{
				FItemData FoundItemData;
				if (LocalInventoryComponent->GetItemByData(CraftableItem, FoundItemData))
				{
					LocalInventoryComponent->AddToStackInInventory(CraftableItem, FoundItemData.Index);
				}
				else
				{
					int32 FoundIndex = -1;
					if (LocalInventoryComponent->GetEmptyInventorySlot(CraftableItem, FoundIndex))
					{
						AddNewItem(CraftableItem, FoundIndex);
					}
				}
			}
			else
			{
				for (int i = 0; i < CraftableItem.Quantity; ++i)
				{
					int32 FoundIndex = -1;
					if (LocalInventoryComponent->GetEmptyInventorySlot(CraftableItem, FoundIndex))
					{
						FItemData SingleItem = CraftableItem;
						SingleItem.Quantity = 1;

						AddNewItem(SingleItem, FoundIndex);
					}
				}
			}
		}
	}
	else
	{
		Sender->EventSpawnCraftedItem(InCraftingData,InOwningPlayer);
	}
	return true;
}


void UStationManagerComponent::GetCurrentCraftableData(FItemData& OutItemData, TArray<FItemStack>& OutRequiredMaterials)
{
	if(UInventoryUtilityLibrary::IsCraftingDataValid(SelectedCraftingData))
	{
		TArray<FItemData> LocalCraftableItemArray;
		TArray<FItemStack> LocalMaterialsArray;
		UInventoryUtilityLibrary::GetCraftableData(SelectedCraftingData,LocalCraftableItemArray,LocalMaterialsArray);

		OutItemData = LocalCraftableItemArray[0];
		OutRequiredMaterials = LocalMaterialsArray;
	}
}

void UStationManagerComponent::AssignCraftableData(const FCraftingData& InCraftingData)
{
	SelectedCraftingData = InCraftingData;
	OnNewItemSelected.Broadcast(SelectedCraftingData);
}

bool UStationManagerComponent::CanBeAddedToInventory(const TArray<FItemData>& CraftableItems, AActor* InOwningPlayer, UBaseCraftingStation* CraftingStation)
{
	if (CraftingStation && !CraftingStation->SpawnCraftedItem)
	{
		return true;
	}

	if (!InOwningPlayer) return false;
	IComponentManager* ComponentManager = Cast<IComponentManager>(InOwningPlayer);
	if (!ComponentManager) return false;
	UInventoryComponent* LocalInventoryComponent = ComponentManager->GetInventoryComponent();
	if (!LocalInventoryComponent) return false;

	for (const FItemData& Item : CraftableItems)
	{
		if (UInventoryUtilityLibrary::IsStackableAndHaveStacks(Item, 0))
		{
			FItemData LocalItemData{};
			int32 LocalSlotIndex = -1;

			if (!LocalInventoryComponent->GetItemByData(Item, LocalItemData) && 
				!LocalInventoryComponent->GetEmptyInventorySlot(Item, LocalSlotIndex))
			{
				return false;
			}
		}
		else
		{
			const EInventoryPanel LocalPanel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(Item);
            
			if (LocalInventoryComponent->GetAmountOfEmptySlots(LocalPanel) < Item.Quantity)
			{
				return false;
			}
		}
	}

	return true;
}


void UStationManagerComponent::FocusSelectedItem(const FCraftingData& InCraftingData, UBaseCraftingStation* Station)
{
	if(!UInventoryUtilityLibrary::AreCraftingDataTheSame(InCraftingData,SelectedCraftingData) && Station)
	{
		AssignCraftableData(InCraftingData);
		AttachNewItemToPlayerPreview(SelectedCraftingData);
		Station->OnRefreshed.Broadcast();
	}
}

void UStationManagerComponent::AttachNewItemToPlayerPreview(const FCraftingData& InCraftingData)
{
	if(!PlayerController) return;
	IComponentManager* ComponentManager = Cast<IComponentManager>(PlayerController);
	if (!ComponentManager) return;
	UEquipmentComponent* LocalEquipmentComponent = ComponentManager->GetEquipmentComponent();
	if (!LocalEquipmentComponent) return;

	if(UInventoryUtilityLibrary::IsItemClassValid(PreviewItemData))
	{
		LocalEquipmentComponent->OnItemDetach.Broadcast(PreviewItemData);
		LocalEquipmentComponent->OnItemAttach.Broadcast(StoredPreviewData);
	}
	
	TArray<FItemData> LocalCraftableItemArray;
	TArray<FItemStack> LocalMaterialsArray;
	UInventoryUtilityLibrary::GetCraftableData(InCraftingData,LocalCraftableItemArray,LocalMaterialsArray);
	
	PreviewItemData = LocalCraftableItemArray[0];
	LocalEquipmentComponent->GetItemByEquipmentSlot(PreviewItemData.EquipmentSlot,StoredPreviewData);

	LocalEquipmentComponent->OnItemAttach.Broadcast(PreviewItemData);
}

bool UStationManagerComponent::SecureMaterialItems(const TArray<FItemStack>& Materials, int32 AmountToCraft,
	AActor* InOwningPlayer, TArray<FItemData>& OutSecuredMaterials)
{
	if(AmountToCraft <= 0)
	{
		return false;
	}
	
	if (!InOwningPlayer) return false;
	IComponentManager* ComponentManager = Cast<IComponentManager>(InOwningPlayer);
	if (!ComponentManager) return false;
	UInventoryComponent* LocalInventoryComponent = ComponentManager->GetInventoryComponent();
	if (!LocalInventoryComponent) return false;
	UEquipmentComponent* LocalEquipmentComponent = ComponentManager->GetEquipmentComponent();
	if (!LocalEquipmentComponent) return false;

	
	TArray<FItemData> ResultSecuredMaterials{};
	
	for(const auto& LocalMaterial : Materials)
	{
		if(const UDataTable* DT = LocalMaterial.TableAndRow.DataTable)
		{
			FItemData* FoundItemDataFromDataTable =  DT->FindRow<FItemData>(LocalMaterial.TableAndRow.RowName, "");
			if(!FoundItemDataFromDataTable) break;
			FItemData FoundItemData;
				
			if (LocalInventoryComponent->GetItemByData(*FoundItemDataFromDataTable, FoundItemData) ||
				LocalEquipmentComponent->GetItemByID(FoundItemDataFromDataTable->ID, FoundItemData))
			{
				if(FoundItemData.Quantity >= LocalMaterial.Quantity)
				{
					FoundItemData.Quantity = LocalMaterial.Quantity;
					ResultSecuredMaterials.Add(FoundItemData);
				}
			}
		}
	}

	if(ResultSecuredMaterials.Num() <= 0)
	{
		return false;
	}

	for(const auto& Item : ResultSecuredMaterials)
	{
		if(Item.IsEquipped)
		{
			FItemData EquipItemData{};
			LocalEquipmentComponent->GetItemByID(Item.ID,EquipItemData);
			LocalEquipmentComponent->RemoveItemQuantity(EquipItemData,Item.Quantity);
			AttachNewItemToPlayerPreview(SelectedCraftingData);
		}
		else
		{
			FItemData InventoryItemData{};
			LocalInventoryComponent->GetItemByData(Item,InventoryItemData);
			LocalInventoryComponent->RemoveItemQuantity(InventoryItemData,Item.Quantity);
		}
	}
	OutSecuredMaterials = ResultSecuredMaterials;
	return true;
}


void UStationManagerComponent::ReturnSecuredMaterials(const FCraftingData& InCraftingData, AActor* InOwningPlayer)
{
	TArray<FItemData> LocalCraftableItemArray;
	TArray<FItemStack> LocalMaterialsArray;
	UInventoryUtilityLibrary::GetCraftableData(InCraftingData,LocalCraftableItemArray,LocalMaterialsArray);

	if (!InOwningPlayer) return; 
	IComponentManager* ComponentManager = Cast<IComponentManager>(InOwningPlayer);
	if (!ComponentManager) return;
	UInventoryComponent* LocalInventoryComponent = ComponentManager->GetInventoryComponent();
	if (!LocalInventoryComponent) return;
	
	for(const auto& LocalMaterial : LocalMaterialsArray)
	{
		if(const UDataTable* DT = LocalMaterial.TableAndRow.DataTable)
		{
			FItemData* FoundItemDataFromDataTable = DT->FindRow<FItemData>(LocalMaterial.TableAndRow.RowName, "");
			if(!FoundItemDataFromDataTable) break;

			FoundItemDataFromDataTable->Quantity = LocalMaterial.Quantity * InCraftingData.CraftingQuantity;

			TArray<FItemData> LocalInventory;
			int32 LocalSize = 0;
			EInventoryPanel LocalPanel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(*FoundItemDataFromDataTable);
			LocalInventoryComponent->GetInventoryAndSize(LocalPanel,LocalInventory,LocalSize);

			int32 LocalSlotIndex = 0;
			if(UInventoryUtilityLibrary::HasPartialStack(LocalInventory,*FoundItemDataFromDataTable,LocalSlotIndex))
			{
				LocalInventoryComponent->AddToStackInInventory(*FoundItemDataFromDataTable,LocalSlotIndex);
			}
			else
			{
				LocalInventoryComponent->GetEmptyInventorySlot(*FoundItemDataFromDataTable,LocalSlotIndex);
				LocalInventoryComponent->AddItemToInventoryArray(*FoundItemDataFromDataTable,LocalSlotIndex);
			}
		}
	}
}

bool UStationManagerComponent::CanCraft(UBaseCraftingStation* CraftingStation)
{
	if(CraftingStation &&
		CraftingStation->CraftOnlyWhenWindowIsOpen ? bIsCraftingWidgetOpen : true)
	{
		return true;
	}
	return false;
}

void UStationManagerComponent::SecureCraftingCost(const FCraftingData& InCraftingData, int32 AmountToCraft,
	UBaseCraftingStation* CraftingStation, AActor* InOwningPlayer)
{
	if (!InOwningPlayer) return; 
	IComponentManager* ComponentManager = Cast<IComponentManager>(InOwningPlayer);
	if (!ComponentManager) return;
	UInventoryComponent* LocalInventoryComponent = ComponentManager->GetInventoryComponent();
	
	if(CraftingStation && CraftingStation->CraftingCostMultiplier > 0.f && LocalInventoryComponent)
	{
		LocalInventoryComponent->RemoveGoldFromOwner(InCraftingData.CraftingCost * CraftingStation->CraftingCostMultiplier * AmountToCraft);
	}
}

void UStationManagerComponent::ReturnSecuredCraftingCost(const FCraftingData& InCraftingData,
	UBaseCraftingStation* CraftingStation, AActor* InOwningPlayer)
{
	if (!InOwningPlayer) return; 
	IComponentManager* ComponentManager = Cast<IComponentManager>(InOwningPlayer);
	if (!ComponentManager) return;
	UInventoryComponent* LocalInventoryComponent = ComponentManager->GetInventoryComponent();
	
	if(CraftingStation && CraftingStation->CraftingCostMultiplier > 0.f && LocalInventoryComponent)
	{
		LocalInventoryComponent->AddGoldToOwner(InCraftingData.CraftingCost * CraftingStation->CraftingCostMultiplier * InCraftingData.CraftingQuantity);
	}
}

bool UStationManagerComponent::IsCraftingRecipeLocked(const FCraftingData& InCraftingData)
{
	if(InCraftingData.StartLocked)
	{
		return UnlockedRecipes.Contains(InCraftingData.CraftingID);
	}
	return false;
}

bool UStationManagerComponent::UnlockCraftingRecipe(const FString& InCraftingID)
{
	if(InCraftingID.IsEmpty() && !UnlockedRecipes.Contains(InCraftingID))
	{
		UnlockedRecipes.Add(InCraftingID);
		return true;
	}
	return false;
}

void UStationManagerComponent::FindItemQuantity(const FItemData& InItemData, AActor* InOwningPlayer, int32& QuantityFound)
{
	// Attempt to Find Item Quantity in Inventory
	if (!InOwningPlayer) return; 
	IComponentManager* ComponentManager = Cast<IComponentManager>(InOwningPlayer);
	if (!ComponentManager) return;
	UInventoryComponent* LocalInventoryComponent = ComponentManager->GetInventoryComponent();

	int32 LocalItemQuantity = 0;
	if(LocalInventoryComponent && LocalInventoryComponent->GetItemsQuantityInInventory(InItemData,LocalItemQuantity))
	{
		QuantityFound = LocalItemQuantity;
	}
	else
	{
		// Attempt to Find Item Quantity in Equipment
		UEquipmentComponent* LocalEquipmentComponent = ComponentManager->GetEquipmentComponent();
		FItemData EquipItemData;
		if(LocalEquipmentComponent && LocalEquipmentComponent->GetItemByID(InItemData.ID,EquipItemData))
		{
			QuantityFound = EquipItemData.Quantity;
			return;
		}
		QuantityFound = 0;
	}
}

void UStationManagerComponent::OpenCraftingWidget(AActor* StationActor, UClass* CraftingWidget)
{
}

void UStationManagerComponent::CloseCraftingWidget()
{
}


