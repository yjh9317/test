// Fill out your copyright notice in the Description page of Project Settings.

#include "Shared/Components/StationsManager.h"

#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Header/DebugHeader.h"
#include "Interface/WidgetManager.h"
#include "Inventory/Components/EquipmentComponent.h"
#include "Inventory/Components/InventoryComponent.h"
#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


UStationsManager::UStationsManager()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UStationsManager::Client_InitializeStationsManager_Implementation(APlayerController* PC)
{
	PlayerController = PC;
}

void UStationsManager::EventStationDestroyed(AActor* Actor, EEndPlayReason::Type EndPlayReason)
{
	CurrentStationActor = nullptr;
	OnStationActorChanged.Broadcast();
}

void UStationsManager::SetCurrentCraftingStationActor(AActor* StationActor)
{
	if(StationActor != CurrentStationActor)
	{
		UnbindStationEndPlayEvent();
		CurrentStationActor = StationActor;
		OnStationActorChanged.Broadcast();
		BindStationEndPlayEvent();
	}
}

AActor* UStationsManager::GetCurrentCraftingStationActor() const
{
	return CurrentStationActor;
}

void UStationsManager::BindStationEndPlayEvent()
{
	if(CurrentStationActor)
	{
		CurrentStationActor->OnEndPlay.AddDynamic(this,&UStationsManager::EventStationDestroyed);
	}
}

void UStationsManager::UnbindStationEndPlayEvent()
{
	if(CurrentStationActor)
	{
		CurrentStationActor->OnEndPlay.RemoveDynamic(this,&UStationsManager::EventStationDestroyed);
	}
}

bool UStationsManager::TryToInitializeCraftingProcess(UBaseCraftingStation* Sender, const FCraftingData& InCraftingData,
	int32 AmountToCraft, AActor* OwningActor, FText& FailureMessage)
{
	bool IsLocked = IsCraftingRecipeLocked(InCraftingData);
	
	if(IsLocked)
	{
		FailureMessage = FText::FromString(TEXT("Recipe is Locked."));
		return false;
	}
	bool IsInitialized = Sender->CanInitializeCraftingProcess(InCraftingData,AmountToCraft,OwningActor,FailureMessage);
	if(!IsInitialized)
	{
		return false;
	}
	TArray<FItemData> LocalCraftableItemArray;
	TArray<FItemStack> LocalMaterialsArray;
	UInventoryUtilityLibrary::GetCraftableData(InCraftingData,LocalCraftableItemArray,LocalMaterialsArray);

	if(!CanBeAddedToInventory(LocalCraftableItemArray,Sender,OwningActor))
	{
		FailureMessage = FText::FromString(TEXT("Your Inventory is full."));
		return false;
	}

	TArray<FItemData> LocalSecuredMaterial;
	if(!SecureMaterialItems(LocalMaterialsArray,AmountToCraft,OwningActor,LocalSecuredMaterial))
	{
		FailureMessage = FText::FromString(TEXT("Not enough materials."));
		return false;
	}

	SecureCraftingCost(InCraftingData,AmountToCraft,Sender,OwningActor);
	return true;
}

bool UStationsManager::TryToCraftItem(UBaseCraftingStation* Sender, const FCraftingData& InCraftingData,
	AActor* OwningActor, const FText& FailureMessage)
{
	if(!UInventoryUtilityLibrary::IsCraftingDataValid(InCraftingData))
	{
		DebugHeader::LogWarning(TEXT("TryToCraftItem : CraftingData is not valid"));
		return false;
	}

	// Get Player Inventory Reference
	if(nullptr == OwningActor)
	{
		DebugHeader::LogWarning(TEXT("TryToCraftItem : OwningActor is nullptr"));
		return false;		
	}
	UInventoryComponent* LocalInventoryComp = OwningActor->FindComponentByClass<UInventoryComponent>();

	if(nullptr == LocalInventoryComp)
	{
		DebugHeader::LogWarning(TEXT("TryToCraftItem : InventoryComponent is nullptr"));
		return false;	
	}
	// Assign Craftable Item and Required Materials to local variables
	TArray<FItemData> LocalCraftableItemArray;
	TArray<FItemStack> LocalMaterialsArray;
	UInventoryUtilityLibrary::GetCraftableData(InCraftingData,LocalCraftableItemArray,LocalMaterialsArray);

	if(nullptr == Sender)
	{
		DebugHeader::LogWarning(TEXT("TryToCraftItem : Sender is nullptr"));
		return false; 
	}
	// Check #1 Should Item be Spawned?
	if(!Sender->SpawnCraftedItem)
	{
		// Check #2 Can be added to Inventory? [If not Spawning]
		if(!CanBeAddedToInventory(LocalCraftableItemArray,Sender,OwningActor))
		{
			DebugHeader::LogWarning(TEXT("TryToCraftItem : Inventory is full."));
			return false;
		}
	}
	else
	{
		// Spawn Crafted Item [skip adding to the Inventory, spawning happens in Crafting Station owner BP]
		Sender->EventSpawnCraftedItem(InCraftingData,OwningActor);
		return true;
	}

	// Add Crafted Items to Inventory
	for(auto CurrentItemData : LocalCraftableItemArray)
	{
		if(UInventoryUtilityLibrary::IsStackableAndHaveStacks(CurrentItemData,1))
		{
			FItemData FoundItemData;
			if(LocalInventoryComp->GetItemByData(CurrentItemData,FoundItemData))
			{
				LocalInventoryComp->AddToStackInInventory(CurrentItemData,FoundItemData.Index);
			}
			else
			{
				
			}

			LocalInventoryComp->AddWeightToInventory(UInventoryUtilityLibrary::CalculateStackedItemWeight(CurrentItemData));
		}
		else
		{
			float ClampedValue = FMath::Clamp(CurrentItemData.Quantity,1,CurrentItemData.Quantity);
			for(int i = 1; i<=ClampedValue ; ++i)
			{
				int32 FoundIndex = -1;
				if(LocalInventoryComp->GetEmptyInventorySlot(CurrentItemData,FoundIndex) && FoundIndex >= 0)
				{
					LocalInventoryComp->AddItemToInventoryArray(CurrentItemData,FoundIndex);
					LocalInventoryComp->AddWeightToInventory(UInventoryUtilityLibrary::CalculateStackedItemWeight(CurrentItemData));
				}
			}
		}
	}
	return true;
}

void UStationsManager::GetCurrentCraftableData(FItemData& OutItemData, TArray<FItemStack>& RequiredMaterials)
{
	if(UInventoryUtilityLibrary::IsCraftingDataValid(SelectedCraftingData))
	{
		TArray<FItemData> LocalCraftableItemArray;
		TArray<FItemStack> LocalMaterialsArray;
		UInventoryUtilityLibrary::GetCraftableData(SelectedCraftingData,LocalCraftableItemArray,LocalMaterialsArray);
		if(LocalCraftableItemArray.IsValidIndex(0))
		{
			OutItemData = LocalCraftableItemArray[0];
			RequiredMaterials = LocalMaterialsArray;
		}
	}
}

void UStationsManager::AssignCraftableData(const FCraftingData& InCraftingData)
{
	SelectedCraftingData = InCraftingData;
	OnNewItemSelected.Broadcast(SelectedCraftingData);
}

bool UStationsManager::CanBeAddedToInventory(const TArray<FItemData>& CraftableItems,
	UBaseCraftingStation* CraftingStation, AActor* OwningPlayer)
{
	// 1. Guard Clauses: 함수를 실행할 수 없는 조건들을 맨 앞에서 먼저 체크합니다.
	if (CraftingStation == nullptr || CraftingStation->SpawnCraftedItem || OwningPlayer == nullptr)
	{
		return false;
	}

	UInventoryComponent* InventoryComp = OwningPlayer->FindComponentByClass<UInventoryComponent>();
	if (InventoryComp == nullptr)
	{
		return false;
	}

	// 2. 모든 아이템을 순회하며 '추가할 수 없는' 경우를 찾습니다.
	for (const FItemData& CurrentItemData : CraftableItems)
	{
		bool bCanAddItem = false; // 현재 아이템을 추가할 수 있는지 여부

		if (UInventoryUtilityLibrary::IsStackableAndHaveStacks(CurrentItemData, 0))
		{
			FItemData FoundItemData;
			// 이미 아이템이 있거나, 새로 넣을 빈 슬롯이 있으면 추가 가능
			int32 TempInt = -1;
			bCanAddItem = InventoryComp->GetItemByData(CurrentItemData, FoundItemData) ||
						  InventoryComp->GetEmptyInventorySlot(CurrentItemData,TempInt);
		}
		else
		{
			// 스택 불가능한 아이템은 수량만큼의 빈 슬롯이 필요
			const EInventoryPanel Panel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(CurrentItemData);
			bCanAddItem = InventoryComp->GetAmountOfEmptySlots(Panel) >= CurrentItemData.Quantity;
		}
		
		if (!bCanAddItem)
		{
			return false;
		}
	}
	
	return true;
}

void UStationsManager::FocusSelectedItem(const FCraftingData& InCraftingData, UBaseCraftingStation* Station)
{
	if(UInventoryUtilityLibrary::AreCraftingDataTheSame(InCraftingData,SelectedCraftingData))
	{
		return;
	}
	AssignCraftableData(InCraftingData);
	AttachNewItemToPlayerPreview(SelectedCraftingData);
	Station->OnRefreshed.Broadcast();
}

void UStationsManager::AttachNewItemToPlayerPreview(const FCraftingData& InCraftingData)
{
	if(nullptr == PlayerController)
	{
		DebugHeader::LogWarning(TEXT("AttachNewItemToPlayerPreview : PlayerController is nullptr"));
		return;
	}
	UEquipmentComponent* LocalEquipmentComp = PlayerController->FindComponentByClass<UEquipmentComponent>();

	if(nullptr == LocalEquipmentComp)
	{
		DebugHeader::LogWarning(TEXT("AttachNewItemToPlayerPreview : UEquipmentComponent is nullptr"));
		return;	
	}

	if(UInventoryUtilityLibrary::IsItemClassValid(PreviewItemData))
	{
		LocalEquipmentComp->OnItemDetach.Broadcast(PreviewItemData);
		LocalEquipmentComp->OnItemAttach.Broadcast(StoredPreviewData);
	}
	
	TArray<FItemData> LocalCraftableItemArray;
	TArray<FItemStack> LocalMaterialsArray;
	UInventoryUtilityLibrary::GetCraftableData(InCraftingData,LocalCraftableItemArray,LocalMaterialsArray);

	if(LocalCraftableItemArray.IsValidIndex(0))
	{
		PreviewItemData = LocalCraftableItemArray[0];
	}

	LocalEquipmentComp->GetItemByEquipmentSlot(PreviewItemData.EquipmentSlot,StoredPreviewData);
	LocalEquipmentComp->OnItemAttach.Broadcast(PreviewItemData);
}

bool UStationsManager::SecureMaterialItems(const TArray<FItemStack>& Materials, int32 AmountToCraft,
	AActor* OwningPlayer, TArray<FItemData>& SecuredMaterials)
{
	if(AmountToCraft <= 0)
	{
		return false;
	}

	// Store Player Inventory as local ref
	if(nullptr == OwningPlayer)
	{
		return false;
	}
	UInventoryComponent* LocalInventoryComp = OwningPlayer->FindComponentByClass<UInventoryComponent>();
	if(nullptr == LocalInventoryComp)
	{
		return false;
	}

	// Store Player Equipment as local ref
	UEquipmentComponent* LocalEquipmentComp = OwningPlayer->FindComponentByClass<UEquipmentComponent>();
	if(nullptr == LocalEquipmentComp)
	{
		return false;
	}

	bool LocalCanBeRemoved = false;
	TArray<FItemData> LocalSecuredItems;
	FItemData FoundItemData;
	for(const auto& Material : Materials)
	{
		if(const UDataTable* DT = Material.TableAndRow.DataTable)
		{
			FItemData FoundItemDataFromDataTable = *DT->FindRow<FItemData>(Material.TableAndRow.RowName, "");
			if(LocalInventoryComp->GetItemByData(FoundItemDataFromDataTable,FoundItemData) ||
				LocalEquipmentComp->GetItemByID(FoundItemDataFromDataTable.ID,FoundItemData))
			{
				if(FoundItemData.Quantity >= FoundItemDataFromDataTable.Quantity * AmountToCraft)
				{
					FoundItemData.Quantity = FoundItemDataFromDataTable.Quantity * AmountToCraft;
					LocalSecuredItems.Add(FoundItemData);
					LocalCanBeRemoved = true;
					break;
				}
			}
		}
	}

	if(LocalCanBeRemoved)
	{
		for(const auto& SecuredItem : LocalSecuredItems)
		{
			FoundItemData = {};
			if(SecuredItem.IsEquipped)
			{
				LocalEquipmentComp->GetItemByID(SecuredItem.ID,FoundItemData);
				LocalEquipmentComp->RemoveItemQuantity(FoundItemData,SecuredItem.Quantity);
				AttachNewItemToPlayerPreview(SelectedCraftingData);
			}
			else
			{
				LocalInventoryComp->GetItemByData(SecuredItem,FoundItemData);
				LocalInventoryComp->RemoveItemQuantity(FoundItemData,SecuredItem.Quantity);
			}
		}
		SecuredMaterials = LocalSecuredItems;
	}
	return true;
}

void UStationsManager::ReturnSecuredMaterials(const FCraftingData& InCraftingData, AActor* OwningPlayer)
{
	if(nullptr == OwningPlayer)
	{
		return;
	}
	UInventoryComponent* LocalInventoryComp = OwningPlayer->FindComponentByClass<UInventoryComponent>();
	if(nullptr == LocalInventoryComp)
	{
		return;
	}
	
	TArray<FItemData> LocalCraftableItemArray;
	TArray<FItemStack> LocalMaterialsArray;
	UInventoryUtilityLibrary::GetCraftableData(InCraftingData,LocalCraftableItemArray,LocalMaterialsArray);

	for(const auto& Material: LocalMaterialsArray)
	{
		if(const UDataTable* DT = Material.TableAndRow.DataTable)
		{
			FItemData& FoundItemDataFromDataTable = *DT->FindRow<FItemData>(Material.TableAndRow.RowName, "");
			FoundItemDataFromDataTable.Quantity = InCraftingData.CraftingQuantity;
			
			const EInventoryPanel Panel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(FoundItemDataFromDataTable);
			TArray<FItemData> LocalInventoryArray;
			int32 TempSize = 0;
			LocalInventoryComp->GetInventoryAndSize(Panel,LocalInventoryArray,TempSize);
			
			int32 LocalEmptySlotIndex = 0 ;
			if(UInventoryUtilityLibrary::HasPartialStack(LocalInventoryArray,FoundItemDataFromDataTable,LocalEmptySlotIndex))
			{
				LocalInventoryComp->AddToStackInInventory(FoundItemDataFromDataTable,LocalEmptySlotIndex);
			}
			else
			{
				LocalInventoryComp->GetEmptyInventorySlot(FoundItemDataFromDataTable,LocalEmptySlotIndex);
				LocalInventoryComp->AddItemToInventoryArray(FoundItemDataFromDataTable,LocalEmptySlotIndex);
			}
		}
	}
}

bool UStationsManager::CanCraft(UBaseCraftingStation* CraftingStation)
{
	if(nullptr == CraftingStation)
	{
		return false;
	}

	bool TempBool = false;
	if(CraftingStation->CraftOnlyWhenWindowIsOpen)
	{
		TempBool = bIsCraftingWidgetOpen;
	}
	else
	{
		TempBool = true;
	}
	return TempBool;
}

void UStationsManager::SecureCraftingCost(const FCraftingData& InCraftingData, int32 AmountToCraft,
	UBaseCraftingStation* CraftingStation, AActor* OwningPlayer)
{
	if(nullptr == CraftingStation || nullptr == OwningPlayer)
	{
		return;
	}
	UInventoryComponent* LocalInventoryComp = OwningPlayer->FindComponentByClass<UInventoryComponent>();
	if(nullptr == LocalInventoryComp)
	{
		return;
	}
	

	if(CraftingStation->CraftingCostMultiplier > 0)
	{
		float LocalGold = InCraftingData.CraftingCost * CraftingStation->CraftingCostMultiplier * AmountToCraft;
		LocalInventoryComp->RemoveGoldFromOwner(LocalGold);
	}
}

void UStationsManager::ReturnSecuredCraftingCost(const FCraftingData& InCraftingData,
	UBaseCraftingStation* CraftingStation, AActor* OwningPlayer)
{
	if(nullptr == CraftingStation || nullptr == OwningPlayer)
	{
		return;
	}
	UInventoryComponent* LocalInventoryComp = OwningPlayer->FindComponentByClass<UInventoryComponent>();
	if(nullptr == LocalInventoryComp)
	{
		return;
	}
	

	if(CraftingStation->CraftingCostMultiplier > 0)
	{
		float LocalGold = InCraftingData.CraftingCost * CraftingStation->CraftingCostMultiplier * InCraftingData.CraftingQuantity;
		LocalInventoryComp->AddGoldToOwner(LocalGold);
	}
}

bool UStationsManager::IsCraftingRecipeLocked(const FCraftingData& InCraftingData)
{
	if(false == InCraftingData.StartLocked)
	{
		return false;	
	}

	return !UnlockedRecipes.Contains(InCraftingData.CraftingID);
}

bool UStationsManager::UnlockCraftingRecipe(const FString& CraftingID)
{
	if(!CraftingID.IsEmpty() && !UnlockedRecipes.Contains(CraftingID))
	{
		UnlockedRecipes.Add(CraftingID);
		return true;
	}
	return false;
}

int32 UStationsManager::FindItemQuantity(const FItemData& InItemData, AActor* OwningPlayer)
{
	if(nullptr == OwningPlayer)
	{
		return -1;
	}
	UInventoryComponent* LocalInventoryComp = OwningPlayer->FindComponentByClass<UInventoryComponent>();
	int32 LocalQuantity = 0;

	// Attempt to Find Item Quantity in Inventory
	if(LocalInventoryComp && LocalInventoryComp->GetItemsQuantityInInventory(InItemData,LocalQuantity))
	{
		return LocalQuantity;
	}

	UEquipmentComponent* LocalEquipmentComp = OwningPlayer->FindComponentByClass<UEquipmentComponent>();
	FItemData FoundItemData{};

	// Attempt to Find Item Quantity in Equipment
	if(LocalEquipmentComp && LocalEquipmentComp->GetItemByID(InItemData.ID,FoundItemData))
	{
		return FoundItemData.Quantity;
	}
	return 0;
}

void UStationsManager::OpenCraftingWidget(AActor* StationActor, TSubclassOf<UUserWidget> CraftingWidgetClass)
{
	if(nullptr == StationActor || nullptr == GetWorld())
	{
		return;
	}
	
	// Set the Current Station Actor - from which we can retrieve the specified Components later in UI (since an Actor can have multiple different Station Components attached to itself).
	// The Actor will be either an Interactable Object (i.e. Forge) or a Player Controller
	SetCurrentCraftingStationActor(StationActor);

	UInventoryUtilityLibrary::PlaySoundOnOpeningWidget(GetWorld());
	if(IWidgetManager* WidgetManager = Cast<IWidgetManager>(PlayerController))
	{
		WidgetManager->SetActiveWidget(EWidgetType::CRAFTING);
		WidgetManager->SetActiveTab(EWidgetType::CRAFTING);
		WidgetManager->StartPlayerCapture();

		CraftingWidgetRef = CreateWidget(PlayerController,CraftingWidgetClass);
		
		if(CraftingWidgetRef && !UKismetSystemLibrary::HasMultipleLocalPlayers(GetWorld()))
		{
			CraftingWidgetRef->AddToViewport(1);
		}
		else
		{
			CraftingWidgetRef->AddToPlayerScreen(1);	
		}
	}
	
	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController,CraftingWidgetRef);
	PlayerController->bShowMouseCursor = true;
	// Ignore Move & Look Inputs
	PlayerController->SetIgnoreMoveInput(true);
	PlayerController->SetIgnoreLookInput(true);
	bIsCraftingWidgetOpen = true;
}

void UStationsManager::CloseCraftingWidget()
{
	if(nullptr == GetWorld() || nullptr == PlayerController)
	{
		return;
	}
	
	SetCurrentCraftingStationActor(nullptr);

	UEquipmentComponent* LocalEquipmentComp = PlayerController->FindComponentByClass<UEquipmentComponent>();
	if(LocalEquipmentComp)
	{
		LocalEquipmentComp->OnItemDetach.Broadcast(PreviewItemData);
		LocalEquipmentComp->OnItemAttach.Broadcast(StoredPreviewData);
		PreviewItemData = {};
		StoredPreviewData = {};
	}

	if(IWidgetManager* WidgetManager = Cast<IWidgetManager>(PlayerController))
	{
		WidgetManager->SetActiveWidget(EWidgetType::NONE);
		WidgetManager->SetActiveTab(EWidgetType::NONE);
		WidgetManager->StopPlayerCapture();

		if(CraftingWidgetRef)
		{
			CraftingWidgetRef->RemoveFromParent();
		}
	}

	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
	PlayerController->bShowMouseCursor = false;
	// Ignore Move & Look Inputs
	PlayerController->ResetIgnoreInputFlags();
	bIsCraftingWidgetOpen = false;
	SelectedCraftingData = {};
}

void UStationsManager::Server_InitializeCraftingProcess_Implementation(UBaseCraftingStation* Sender,
                                                                       const FCraftingData& InCraftingData, int32 AmountToCraft, AActor* OwningActor)
{
	FText LocalFailureText;
	bool IsInitialize = TryToInitializeCraftingProcess(Sender,InCraftingData,AmountToCraft,OwningActor,LocalFailureText);
	Client_InitializeCraftingProcessReturnValue(Sender,InCraftingData,AmountToCraft,IsInitialize,LocalFailureText);
}

void UStationsManager::Server_TryToCraftItem_Implementation(UBaseCraftingStation* Sender,
	const FCraftingData& InCraftingData, AActor* OwningActor)
{
	FText LocalFailureText;
	bool IsCrafted = TryToCraftItem(Sender,InCraftingData,OwningActor,LocalFailureText);
	Client_TryToCraftReturnValue(Sender,IsCrafted,LocalFailureText);
}

void UStationsManager::Server_ReturnSecuredMaterials_Implementation(UBaseCraftingStation* Sender,
	const FCraftingData& InCraftingData, AActor* OwningActor)
{
	ReturnSecuredMaterials(InCraftingData,OwningActor);
	ReturnSecuredCraftingCost(InCraftingData,Sender,OwningActor);
	ClientRefreshUIData(Sender);
}

void UStationsManager::Client_UnlockCraftingRecipe_Implementation(const FString& CraftingID)
{
	bool IsUnlock = UnlockCraftingRecipe(CraftingID);
}

void UStationsManager::Client_TryToCraftReturnValue_Implementation(UBaseCraftingStation* Sender, bool ReturnValue,
	const FText& Message)
{
	if(ReturnValue)
	{
		AActor* Actor = Cast<AActor>(PlayerController);
		if(Actor)
		{
			Sender->EventItemCrafted(Actor);
		}
	}
	else
	{
		IWidgetManager* WidgetManager = Cast<IWidgetManager>(PlayerController);
		if(WidgetManager)
		{
			WidgetManager->DisplayMessageNotify(Message);
		}
		AActor* Actor = Cast<AActor>(PlayerController);
		if(Actor)
		{
			FCraftingData CraftingData = Sender->GetFirstItemFromQueue();
			Sender->StopCraftingProcess(CraftingData,Actor);
		}
	}
}

void UStationsManager::Client_InitializeCraftingProcessReturnValue_Implementation(UBaseCraftingStation* Sender,
	const FCraftingData& InCraftingData, int32 AmountToCraft, bool ReturnValue, const FText& Message)
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
			WidgetManager->DisplayMessageNotify(Message);
		}
		Sender->EventFailedToInitializeCraftingProcess(InCraftingData,AmountToCraft);
	}
}

void UStationsManager::ClientRefreshUIData_Implementation(UBaseCraftingStation* Target)
{
	Target->OnRefreshed.Broadcast();
}
