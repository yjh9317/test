

#include "Inventory/Components/InventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Header/DebugHeader.h"
#include "Interaction/Components/InteractableComponent.h"
#include "Interface/HUDManager.h"
#include "GameFramework/HUD.h"
#include "Interface/ObjectInteraction.h"
#include "Interface/WidgetManager.h"
#include "Inventory/Components/EquipmentComponent.h"
#include "Inventory/FunctionLibrary/InventoryUtilityLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Player/Controller/MainPlayerController.h"
#include "Shared/Components/ObjectPoolComponent.h"

namespace PlayerSpawnConstants
{
	constexpr float ForwardSpawnOffset = 50.0f;
	constexpr float VerticalSpawnOffset = -50.0f;
}

UInventoryComponent::UInventoryComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	
}


void UInventoryComponent::BeginPlay()
{
	if(GetWorld())
	{
		FTimerHandle LocalTimerHandle{};
		GetWorld()->GetTimerManager().SetTimer(
			LocalTimerHandle,
			this,
			&UInventoryComponent::CreateInventoryWidget,
			0.5f,
			false);
	}
	// CreateInventoryWidget();
	// CreateVendorWidget();
	// CreateStorageWidget();
}

void UInventoryComponent::InputInventoryFromSwitcher()
{
	IWidgetManager* WidgetManager = Cast<IWidgetManager>(PlayerController);
	if(WidgetManager)
	{
		if(WidgetManager->GetActiveWidget() == EWidgetType::INVENTORY)
		{
			CloseInventoryWidget();
		}
		else
		{
			WidgetManager->SwitchWidgetTo(EWidgetType::INVENTORY);
		}
	}
}

void UInventoryComponent::InputInventory()
{
	if(nullptr == InventoryWidgetRef)
	{
		DebugHeader::LogWarning("InputInventory : InventoryWidgetRef is nullptr");
		return ;
	}

	IWidgetManager* WidgetManager = nullptr;;
	if(PlayerController)
	{
		 WidgetManager = Cast<IWidgetManager>(PlayerController);
	}

	if(!InventoryWidgetRef->IsInViewport())
	{
		InventoryWidgetRef->AddToViewport();
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController,InventoryWidgetRef);
		PlayerController->bShowMouseCursor = true;
		PlayerController->SetIgnoreMoveInput(true);
		PlayerController->SetIgnoreLookInput(true);

		if(WidgetManager)
		{
			WidgetManager->StartPlayerCapture();
		}
	}
	else
	{
		InventoryWidgetRef->RemoveFromParent();
		UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
		PlayerController->bShowMouseCursor = false;
		PlayerController->SetIgnoreMoveInput(false);
		PlayerController->SetIgnoreLookInput(false);
		
		if(WidgetManager)
		{
			WidgetManager->StopPlayerCapture();
		}
	}
	
}

void UInventoryComponent::InitializeInventory(APlayerController* PC)
{
	ClientInitializeInventory(PC);
	Super::InitializeInventory(PC);
}

void UInventoryComponent::ClientInitializeInventory(APlayerController* PC)
{
	PlayerController = PC;

	if(!UKismetSystemLibrary::IsStandalone(GetWorld()))
	{
		TArray<EInventoryPanel> PanelArray{};
		for(const auto& Panel : PanelsToUse)
		{
			BuildInventory(Panel);
		}
	}
}


void UInventoryComponent::FinalizeTransferUpdates(UInventoryCoreComponent* Sender,const FItemData& InItemData)
{
	float StackedItemWeight = UInventoryUtilityLibrary::CalculateStackedItemWeight(InItemData);
	AddWeightToInventory(StackedItemWeight);

	if(Sender->CheckOwnerGold && CheckOwnerGold)
	{
		float StackedItemGold = UInventoryUtilityLibrary::CalculateStackedItemValue(InItemData);
		RemoveGoldFromOwner(StackedItemGold);
		Sender->AddGoldToOwner(StackedItemGold);
	}

	Sender->RemoveItemFromInventoryArray(InItemData);
	Sender->RemoveWeightFromInventory(StackedItemWeight);
}

bool UInventoryComponent::TransferItemFromInventory(UInventoryCoreComponent* Sender,FItemData& InItemData,
                                                    const FItemData& InSlotData, EInputMethod Method, AActor* OwningPlayer)
{
    if (!UInventoryUtilityLibrary::IsItemClassValid(InItemData)) { return false; }
    if (!Sender) { return false; }
    if (!AttemptUsingTransferredItem(Sender, InItemData)) { return false; }
    if (Sender->CheckOwnerGold && !HasEnoughGold(InItemData)) { return false; }

    if (OwningPlayer)
    {
        UEquipmentComponent* LocalEquipmentComponent = OwningPlayer->FindComponentByClass<UEquipmentComponent>();
        if (LocalEquipmentComponent && LocalEquipmentComponent->TryToAddToPartialStack(InItemData))
        {
        	FinalizeTransferUpdates(Sender, InItemData);
            return true;
        }
    }


    EInventoryPanel LocalPanel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(InItemData);
    int LocalInventorySize = 0;
    TArray<FItemData> LocalInventory{};
    GetInventoryAndSize(LocalPanel, LocalInventory, LocalInventorySize);

    int32 SlotIndex;
    if (UInventoryUtilityLibrary::HasPartialStack(LocalInventory, InItemData, SlotIndex))
    {
        AddToStackInInventory(InItemData, SlotIndex);
    	FinalizeTransferUpdates(Sender, InItemData);
    }

    if (Method == EInputMethod::RightClick)
    {
        int32 EmptySlot = -1;
        if (GetEmptyInventorySlot(InItemData, EmptySlot) && EmptySlot >= 0) 
        {
            AddItemToInventoryArray(InItemData, EmptySlot);
        	FinalizeTransferUpdates(Sender, InItemData);
        	return true;
        }
    	else
    	{
    		return false;
    	}
    }
    if (Method == EInputMethod::DragAndDrop)
    {
        AddItemToInventoryArray(InItemData, InSlotData.Index);
    	FinalizeTransferUpdates(Sender, InItemData);
    	return true;
    }

    return false;
}

void UInventoryComponent::SplitItemsInInventory(UInventoryCoreComponent* Sender, FItemData InItemData,
	const FItemData& InSlotData, FItemData StackableLeft, EInputMethod Method, EItemDestination Initiator,
	EItemDestination Destination, AActor* OwningPlayer)
{
	switch (Initiator)
	{
	case EItemDestination::InventorySlot:
		if(Destination == EItemDestination::InventorySlot)
		{
			RemoveItemFromInventoryArray(InItemData);
			if(UInventoryUtilityLibrary::IsItemClassValid(InSlotData))
			{
				AddToStackInInventory(InItemData,InSlotData.Index);	
			}
			else
			{
				AddItemToInventoryArray(InItemData,InSlotData.Index);
			}

			if(UInventoryUtilityLibrary::IsStackableAndHaveStacks(StackableLeft,0))
			{
				AddItemToInventoryArray(StackableLeft,StackableLeft.Index);			
			}
		}
		else if (Destination == EItemDestination::DropBar)
		{
			DropItemOnTheGround(InItemData,EItemDestination::InventorySlot,OwningPlayer);
			if(UInventoryUtilityLibrary::IsStackableAndHaveStacks(StackableLeft,0))
			{
				AddItemToInventoryArray(StackableLeft,StackableLeft.Index);			
			}
		}
		break;
	case EItemDestination::EquipmentSlot:
		{
			UEquipmentComponent* LocalEquipmentComp = OwningPlayer->FindComponentByClass<UEquipmentComponent>();
			if(LocalEquipmentComp)
			{
				LocalEquipmentComp->RemoveItemFromEquipmentArray(InItemData);
				if(UInventoryUtilityLibrary::IsItemClassValid(InSlotData))
				{
					AddToStackInInventory(InItemData,InSlotData.Index);
				}
				else
				{
					AddItemToInventoryArray(InItemData,InSlotData.Index);
				}
				if(UInventoryUtilityLibrary::IsStackableAndHaveStacks(StackableLeft,0))
				{
					LocalEquipmentComp->AddItemToEquipmentArray(StackableLeft,StackableLeft.EquipmentSlot);
				}
			}
		}
		break;
	case EItemDestination::VendorSlot:
	case EItemDestination::StorageSlot:
		bool IsTransferred = TransferItemFromInventory(Sender,InItemData,InSlotData,Method,OwningPlayer);
		const FString FailureMSG = {};
		Client_TransferItemReturnValue(IsTransferred,FailureMSG);
		if(IsTransferred && UInventoryUtilityLibrary::IsStackableAndHaveStacks(StackableLeft,0))
		{
			Sender->AddItemToInventoryArray(StackableLeft,StackableLeft.Index);
		}
		break;
	}
}

void UInventoryComponent::ConfirmationPopupAccepted(UInventoryCoreComponent* Sender, FItemData InItemData,
	const FItemData& InSlotData, EInputMethod Method, EItemDestination Initiator, EItemDestination Destination,
	AActor* OwningPlayer)
{
	if(!UInventoryUtilityLibrary::IsStackableAndHaveStacks(InItemData,1) &&
		(Initiator == EItemDestination::InventorySlot && Initiator ==EItemDestination::EquipmentSlot) &&
		 Destination == EItemDestination::DropBar)
	{
		DropItemOnTheGround(InItemData,Initiator,OwningPlayer);
	}
}

void UInventoryComponent::AddItemToInventoryArray(FItemData& InItemData, int32 SlotIndex)
{
    if (!GetOwner() || !GetOwner()->HasAuthority())
    {
        return;
    }

    ModifyItemValue(InItemData);
	
    EInventoryPanel LocalPanel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(InItemData);
    int32 LocalInventorySize = 0 ;
    TArray<FItemData> LocalInventory{};
    GetInventoryAndSize(LocalPanel, LocalInventory, LocalInventorySize);

    int32 FinalItemIndex = -1;
    bool bSuccessfullyAdded = false;

    if (UseInventorySize || SlotIndex >= 0)
    {
        int32 TargetIndex = InItemData.Index;

        if (LocalInventory.IsValidIndex(TargetIndex))
        {
            InItemData.IsEquipped = false;
            LocalInventory[TargetIndex] = InItemData;
            FinalItemIndex = TargetIndex;
            bSuccessfullyAdded = true;
        }
    }
    else
    {
        if (UInventoryUtilityLibrary::IsItemClassValid(InItemData))
        {
            InItemData.IsEquipped = false;
            FinalItemIndex = LocalInventory.Add(InItemData); 
         
            InItemData.Index = FinalItemIndex;
         
            if (LocalInventory.IsValidIndex(FinalItemIndex))
            {
                 LocalInventory[FinalItemIndex].Index = FinalItemIndex;
                 bSuccessfullyAdded = true;
            }
        }
    }
    if (bSuccessfullyAdded && LocalInventory.IsValidIndex(FinalItemIndex))
    {
        ApplyChangesToInventoryArray(LocalPanel, LocalInventory);

        const FItemData& AddedItem = LocalInventory[FinalItemIndex];

        if (UKismetSystemLibrary::IsStandalone(GetWorld()))
        {
            SwitchActivePanel(LocalPanel);
            OnRefreshInventory.Broadcast(LocalPanel);
            OnAddedToInventoryArray.Broadcast(AddedItem);
            OnHighlightInventorySlot.Broadcast(FinalItemIndex);
        }
        else
        {
            Client_UpdateAddedItem(this, AddedItem);
        }
    }
}

void UInventoryComponent::SwapItemsInInventory(FItemData FirstItemData, FItemData SecondItemData)
{
	if (UInventoryUtilityLibrary::SwitchHasOwnerAuthority(this) &&
		UInventoryUtilityLibrary::IsItemClassValid(FirstItemData) &&
		UInventoryUtilityLibrary::IsItemClassValid(SecondItemData))
	{
		if (UInventoryUtilityLibrary::AreItemsTheSame(FirstItemData, SecondItemData) &&
			UInventoryUtilityLibrary::AreItemsStackable(FirstItemData, SecondItemData)) // Stack 
		{
			RemoveItemFromInventoryArray(FirstItemData);
			AddToStackInInventory(FirstItemData, SecondItemData.Index);
		}
		else // Swap
		{
			EInventoryPanel FirstItemPanel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(FirstItemData);
			TArray<FItemData> LocalInventory{};
			int32 InventorySize = 0;
			GetInventoryAndSize(FirstItemPanel, LocalInventory, InventorySize);

			// swap index
			int32 FirstItemIndex = FirstItemData.Index;
			int32 SecondItemIndex = SecondItemData.Index;

			if(!LocalInventory.IsValidIndex(FirstItemIndex) || !LocalInventory.IsValidIndex(SecondItemIndex))
			{
				DebugHeader::LogWarning(TEXT("SwapItemsInInventory : not valid index"));
				return;
			}

			FirstItemData.IsEquipped = false;
			FirstItemData.Index = SecondItemIndex;
			LocalInventory[SecondItemIndex] = FirstItemData;
			
			SecondItemData.IsEquipped = false;
			SecondItemData.Index = FirstItemIndex;
			LocalInventory[FirstItemIndex] = SecondItemData;
			ApplyChangesToInventoryArray(FirstItemPanel, LocalInventory);
			OnRefreshInventory.Broadcast(FirstItemPanel);

			// Client
			if (!UKismetSystemLibrary::IsStandalone(GetWorld()))
			{
				Client_UpdateItems(this,FirstItemPanel,LocalInventory);
			}
		}
	}
}

void UInventoryComponent::MoveItemInInventory(FItemData SourceItemData, int32 TargetIndex)
{
	if (UInventoryUtilityLibrary::SwitchHasOwnerAuthority(this) &&
	UInventoryUtilityLibrary::IsItemClassValid(SourceItemData))
	{
		EInventoryPanel LocalPanel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(SourceItemData);
		TArray<FItemData> LocalInventory{};
		int32 InventorySize = 0;
		GetInventoryAndSize(LocalPanel, LocalInventory, InventorySize);

		int32 SourceItemIndex  = SourceItemData.Index;

		SourceItemData.IsEquipped = false;
		SourceItemData.Index = TargetIndex;
		LocalInventory[TargetIndex] = SourceItemData;

		LocalInventory[SourceItemIndex] = FItemData();
		LocalInventory[SourceItemIndex].Index = SourceItemIndex;
		ApplyChangesToInventoryArray(LocalPanel, LocalInventory);
		OnRefreshInventory.Broadcast(LocalPanel);

		if (!UKismetSystemLibrary::IsStandalone(GetWorld()))
		{
			Client_UpdateItems(this,LocalPanel,LocalInventory);
		}
	}
}

void UInventoryComponent::RefreshInventoryOnAdd(FItemData& ItemData, int32 SlotIndex, EInventoryPanel LocalPanel)
{
	if (UKismetSystemLibrary::IsStandalone(GetWorld()))
	{
		// IF STANDALONE: Switch Active Panel and Refresh Inventory
		SwitchActivePanel(LocalPanel);
		OnRefreshInventory.Broadcast(LocalPanel);
		OnAddedToInventoryArray.Broadcast(ItemData);
		OnHighlightInventorySlot.Broadcast(SlotIndex);
	}
	else
	{
		Client_UpdateAddedItem(this,ItemData);
	}
}

void UInventoryComponent::TakeItem(UInventoryCoreComponent* Sender, FItemData InItemData, AActor* OwningPlayer)
{
	if (Sender == nullptr) return;
	if (!UInventoryUtilityLibrary::IsItemClassValid(InItemData)) return;

	bool IsTransferred = TransferItemFromInventory(Sender, InItemData, FItemData{}, EInputMethod::RightClick,
	                                               OwningPlayer);

	TArray<FItemData> LocalInventory{};
	int32 InventorySize = 0;
	GetInventoryAndSize(EInventoryPanel::P1, LocalInventory, InventorySize);

	bool IsNotValidIndex = !LocalInventory.IsValidIndex(0);
	FString LocalFailureMessage;
	Client_TakeItemReturnValue(IsTransferred, LocalFailureMessage, IsNotValidIndex);
	if (IsTransferred)
	{
		Client_ItemLooted(InItemData);
	}

}

void UInventoryComponent::TakeAllItems(UInventoryCoreComponent* Sender, AActor* OwningPlayer)
{
	TArray<FItemData> LocalInventory{};
	int32 InventorySize = 0;
	if(Sender)
	{
		Sender->GetInventoryAndSize(EInventoryPanel::P1,LocalInventory,InventorySize);

		bool LocalReturnValue = false;
		for(auto ItemData : LocalInventory)
		{
			if(UInventoryUtilityLibrary::IsItemClassValid(ItemData))
			{
				LocalReturnValue = TransferItemFromInventory(Sender,ItemData,FItemData{},EInputMethod::RightClick,OwningPlayer);
				if(LocalReturnValue)
				{
					Client_ItemLooted(ItemData);
				}
				else
				{
					LocalReturnValue = false;
					break;
				}
			}
		}
		FString LocalFailureMessage;
		Sender->GetInventoryAndSize(EInventoryPanel::P1,LocalInventory,InventorySize);
		Client_TakeItemReturnValue(LocalReturnValue,LocalFailureMessage,!LocalInventory.IsValidIndex(0));
	}
}

void UInventoryComponent::DropItemOnTheGround(const FItemData& InItemData,EItemDestination Initiator,AActor* OwningPlayer)
{
	switch (Initiator)
	{
	case EItemDestination::None:
	case EItemDestination::InventorySlot:
		{
			RemoveItemFromInventoryArray(InItemData);
			float Weight = UInventoryUtilityLibrary::CalculateStackedItemWeight(InItemData);
			RemoveWeightFromInventory(Weight);
			Server_SpawnLootBag(InItemData,OwningPlayer);
		}
		break;
	case EItemDestination::EquipmentSlot:
		{
			if(UEquipmentComponent* EquipmentComponent = OwningPlayer->FindComponentByClass<UEquipmentComponent>())
			{
				EquipmentComponent->RemoveItemFromEquipmentArray(InItemData);
				EquipmentComponent->DetachItemFromEquipment(InItemData);
			}
		}
		break;
	default:
		break;
	}

	float StackedItemWeight = UInventoryUtilityLibrary::CalculateStackedItemWeight(InItemData);
	RemoveWeightFromInventory(StackedItemWeight);

	Server_SpawnLootBag(InItemData,OwningPlayer);
	Server_SpawnInteractable(InItemData,OwningPlayer);
}


void UInventoryComponent::SpawnLootBagNearThePlayer(FItemData& InItemData, AActor* OwningPlayer)
{
	APlayerController* LocalPC = Cast<APlayerController>(OwningPlayer);
	if (!LocalPC)
	{
		DebugHeader::LogWarning("SpawnLootBagNearThePlayer: LocalPC is not valid");
		return;
	}

	APawn* LocalPawn = LocalPC->GetPawn();
	if (!LocalPawn)
	{
		DebugHeader::LogWarning("SpawnLootBagNearThePlayer: LocalPawn is not valid");
		return;
	}

	
	if(DroppedIndex < 1)
	{
		FVector PawnLocation = LocalPawn->GetActorLocation();
		FVector PawnForwardVector  = LocalPawn->GetActorForwardVector();
		
		FVector SpawnLocation = PawnLocation + PawnForwardVector * PlayerSpawnConstants::ForwardSpawnOffset;
		SpawnLocation.Z += PlayerSpawnConstants::VerticalSpawnOffset;
		FVector CheckSpawnEnd = SpawnLocation;
		CheckSpawnEnd.Z -=  PlayerSpawnConstants::VerticalSpawnOffset;

		FTransform SpawnTransform = UKismetMathLibrary::MakeTransform(SpawnLocation,FRotator::ZeroRotator);
		IsCollidingWithLootBag(SpawnLocation,CheckSpawnEnd,LootBagPtr.Get());
		if (!LootBagPtr.IsValid())
		{
			UClass* LoadedClass = nullptr;
			if(LootBagClass)
			{
				LoadedClass = UKismetSystemLibrary::LoadClassAsset_Blocking(LootBagClass);
			}
			if(LoadedClass)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = GetOwner();
				SpawnParams.Instigator = LocalPawn; 
				LootBagPtr = GetWorld()->SpawnActor(LoadedClass,&SpawnTransform,SpawnParams);
			}
		}
	}
	DroppedIndex += 1;

	if(UInventoryCoreComponent* ReceiverInventory = LootBagPtr->FindComponentByClass<UInventoryCoreComponent>())
	{
		EInventoryPanel InventoryPanel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(InItemData);
		int LocalInventorySize = 0;
		TArray<FItemData> LocalInventory{};
		GetInventoryAndSize(InventoryPanel, LocalInventory, LocalInventorySize);

		int32 LocalSlotIndex = -1;
		if(UInventoryUtilityLibrary::HasPartialStack(LocalInventory,InItemData,LocalSlotIndex))
		{
			ReceiverInventory->AddToStackInInventory(InItemData,LocalSlotIndex);
		}
		else
		{
			ReceiverInventory->AddItemToInventoryArray(InItemData,LocalSlotIndex);
		}
	}
}

void UInventoryComponent::SpawnItemMeshNearThePlayer(FItemData& InItemData, AActor* OwningPlayer)
{
	APlayerController* LocalPC = Cast<APlayerController>(OwningPlayer);
	if(nullptr == LocalPC)
	{
		DebugHeader::LogWarning("UInventoryComponent::SpawnItemMeshNearThePlayer : LocalPC is not valid");
		return;
	}

	APawn* LocalPawn = LocalPC->GetPawn();
	if(nullptr == LocalPawn)
	{
		DebugHeader::LogWarning("UInventoryComponent::SpawnItemMeshNearThePlayer : LocalPawn is not valid");
		return;
	}

	if (!InItemData.ItemClass)
	{
		DebugHeader::LogWarning("SpawnItemMeshNearThePlayer: ItemToSpawn.ItemClass가 유효하지 않습니다.");
		return;
	}
	const UItemBase* ItemCDO = Cast<UItemBase>(InItemData.ItemClass->GetDefaultObject());
	if (!ItemCDO)
	{
		DebugHeader::LogWarning("SpawnItemMeshNearThePlayer: ItemClass에서 CDO를 가져올 수 없습니다.");
		return;
	}
	UClass* InteractableClass = nullptr;
	if (ItemCDO->InteractableClass.IsValid())
	{
		TSoftClassPtr<UObject> ClassPtr(ItemCDO->InteractableClass.ToSoftObjectPath());
		InteractableClass = UKismetSystemLibrary::LoadClassAsset_Blocking(ClassPtr);

		if (!InteractableClass)
		{
			DebugHeader::LogWarning(TEXT("SpawnItemMeshNearThePlayer: Failed to load InteractableClass"));
			return;
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnItemMeshNearThePlayer: InteractableClass is not valid"));
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("SpawnItemMeshNearThePlayer: World is null"));
		return;
	}
	// DroppedIndex 값에 따라 플레이어 주변 원형 경로 상의 위치를 계산합니다.

	const FVector PawnLocation = LocalPawn->GetActorLocation();
	const FVector ForwardVector = LocalPawn->GetActorForwardVector();

	// DroppedIndex 값 제한 (0 ~ 12 사이 값 -> 30도 간격으로 360도 표현)
	const int32 ClampedDropCounter = FMath::Clamp(DroppedIndex, 0, 12);

	// 기본 회전 각도 (Z축 기준 30도)
	const FRotator BaseRotationStep(0.f, 0.f, 30.f);
	// 현재 카운터에 따른 최종 회전 오프셋 계산
	const FRotator CurrentRotationOffset = UKismetMathLibrary::Multiply_RotatorInt(BaseRotationStep, ClampedDropCounter);
	// 플레이어 전방 벡터를 회전시켜 스폰 방향 벡터 계산
	const FVector SpawnDirection = ForwardVector.RotateAngleAxis(CurrentRotationOffset.Yaw, FVector::UpVector); // Yaw만 사용하여 회전

	// DroppedIndex의 홀/짝 여부에 따라 랜덤 거리 범위 결정
	float RandomDistance;
	if (DroppedIndex % 2 != 0) 
	{
		RandomDistance = UKismetMathLibrary::RandomFloatInRange(50.f, 150.f);
	}
	else 
	{
		RandomDistance = UKismetMathLibrary::RandomFloatInRange(50.f, 150.f);
	}
	FVector SpawnLocation = PawnLocation + (SpawnDirection * RandomDistance);
	SpawnLocation.Z += 5.0f;
	const FTransform SpawnTransform(FRotator::ZeroRotator, SpawnLocation); // 회전 없이 위치만 적용
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();      
	SpawnParams.Instigator = LocalPawn;  
	// SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AActor* SpawnedItemActor = GetWorld()->SpawnActor(InteractableClass, &SpawnTransform, SpawnParams);
	// 스폰 시도 후 DroppedIndex 증가 (원본 로직 유지)
	DroppedIndex += 1;

	if (SpawnedItemActor)
	{
		// 스폰된 액터에서 인벤토리 컴포넌트 찾기
		if (UInventoryCoreComponent* SpawnedItemInventory = SpawnedItemActor->FindComponentByClass<UInventoryCoreComponent>())
		{
			EInventoryPanel InventoryPanel = UInventoryUtilityLibrary::GetInventoryPanelFromItem(InItemData);
			int UnusedEmptySize{}; 
			TArray<FItemData> SourceInventoryItems; 
			GetInventoryAndSize(InventoryPanel, SourceInventoryItems, UnusedEmptySize); 

			int32 StackSlotIndex = -1; 
			const bool bCanItemStack = UInventoryUtilityLibrary::HasPartialStack(SourceInventoryItems, InItemData, StackSlotIndex);
			if (bCanItemStack)
			{
				// 스택 가능한 슬롯을 찾았으면 해당 슬롯에 스택 추가
				SpawnedItemInventory->AddToStackInInventory(InItemData, StackSlotIndex);
			}
			else
			{
				// 스택 가능한 슬롯이 없으면 새 슬롯에 추가
				SpawnedItemInventory->AddItemToInventoryArray(InItemData, StackSlotIndex);
			}
		}
		else
		{
			DebugHeader::LogWarning(FString::Printf(TEXT("SpawnItemMeshNearThePlayer: 스폰된 액터 '%s'에 UInventoryCoreComponent가 없습니다."), *SpawnedItemActor->GetName()));
		}
	}
	else
	{
		DebugHeader::LogError(FString::Printf(TEXT("SpawnItemMeshNearThePlayer: 클래스 '%s'의 액터 스폰에 실패했습니다."), *InteractableClass->GetName()));
	}
}

bool UInventoryComponent::IsCollidingWithLootBag(const FVector& Start, const FVector& End, AActor* DroppedBag)
{
	FHitResult OutHit;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.bTraceComplex = false;

	bool bHit = GetWorld()->LineTraceSingleByChannel(
	OutHit,       
	Start,     
	End,        
	ECC_Visibility,   
	QueryParams       
	// FCollisionResponseParams::DefaultResponseParam 
);
	#if ENABLE_DRAW_DEBUG
        DrawDebugLine(
            GetWorld(),
            Start,
            bHit ? OutHit.ImpactPoint : End, // Hit 시 Hit 지점까지, 아니면 끝까지
            bHit ? FColor::Green : FColor::Red,     // Hit 여부에 따라 색상 변경
            false, // Persistent lines (false: 한 프레임만 표시)
            1.0f,  // LifeTime
            0,     // Depth Priority
            1.0f   // Thickness
        );
    #endif

	if(bHit && OutHit.GetActor() && OutHit.GetActor()->ActorHasTag("Interactable"))
	{
		return true;
	}
	return false;
}

bool UInventoryComponent::AttemptUsingTransferredItem(UInventoryCoreComponent* Sender,const FItemData& InItemData)
{
	if(!Sender || !UInventoryUtilityLibrary::IsItemClassValid(InItemData) || !InItemData.ItemClass)
	{
		return false;
	}
    
	const UItemBase* ItemCDO = Cast<UItemBase>(InItemData.ItemClass->GetDefaultObject());
	if (!ItemCDO)
	{
		return false;
	}

	switch (ItemCDO->UseType)
	{
	case EItemUseType::Currency:
		{
			float StackedItemValue = UInventoryUtilityLibrary::CalculateStackedItemValue(InItemData);
			AddGoldToOwner(StackedItemValue);
			Sender->RemoveItemFromInventoryArray(InItemData);
			OnItemUsed.Broadcast(InItemData);
		}
		break;
	case EItemUseType::CraftingRecipe:
		{
			Sender->RemoveItemFromInventoryArray(InItemData);
			OnItemUsed.Broadcast(InItemData);
		}
		break;
	}
	return true;
}

UInteractableComponent* UInventoryComponent::GetCurrentInteractable() const
{
    if (PlayerController == nullptr)
    {
        return nullptr;
    }
	
	IObjectInteraction* ObjectInteraction = Cast<IObjectInteraction>(PlayerController);
	UInteractableComponent* InteractableComp{};
	if(ObjectInteraction)
	{
		AActor* InteractionActor = ObjectInteraction->GetCurrentInteractableObject();
		InteractableComp = InteractionActor->FindComponentByClass<UInteractableComponent>();
	}
	return InteractableComp;
}

void UInventoryComponent::ModifyItemDurability(FItemData InItemData, int32 Amount, AActor* OwningPlayer)
{
	if(!InItemData.ItemClass) return;
	
	const UItemBase* ItemCDO = Cast<UItemBase>(InItemData.ItemClass->GetDefaultObject());
	if (nullptr == ItemCDO)
	{
		DebugHeader::LogWarning("UInventoryComponent::ModifyItemDurability: ItemCDO is nullptr.");
		return;
	}


	float NewDurability = FMath::Clamp(InItemData.Durability + Amount,0,100);
	InItemData.Durability = NewDurability;

	UEquipmentComponent* EquipmentComp = OwningPlayer->FindComponentByClass<UEquipmentComponent>();
	if(InItemData.IsEquipped && EquipmentComp)
	{
		EItemSlot EquipSlot = InItemData.EquipmentSlot;
		EquipmentComp->RemoveItemFromEquipmentArray(InItemData);
		EquipmentComp->AddItemToEquipmentArray(InItemData,EquipSlot);
	}
	else if(false == InItemData.IsEquipped)
	{
		int32 ItemIndex = InItemData.Index;
		AddItemToInventoryArray(InItemData,ItemIndex);
	}
}



#pragma region Widget

void UInventoryComponent::PresentModalWidget(UUserWidget* WidgetToShow, int32 ZOrder)
{
	if(!UKismetSystemLibrary::HasMultipleLocalPlayers(GetWorld()))
	{
		WidgetToShow->AddToViewport(ZOrder);
	}
	else
	{
		WidgetToShow->AddToPlayerScreen(ZOrder);
	}

	UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(PlayerController,WidgetToShow);
	PlayerController->bShowMouseCursor = true;
	PlayerController->SetIgnoreMoveInput(true);
	PlayerController->SetIgnoreLookInput(true);
}

void UInventoryComponent::DismissModalWidget(UUserWidget* WidgetToHide)
{
	if(WidgetToHide)
	{
		WidgetToHide->RemoveFromParent();
	}
	
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController);
	PlayerController->bShowMouseCursor = false;
	PlayerController->SetIgnoreMoveInput(false);
	PlayerController->SetIgnoreLookInput(false);
	PlayerController->ResetIgnoreInputFlags();
}

void UInventoryComponent::CreateInventoryWidget()
{
	if (InventoryWidgetRef && IsValid(InventoryWidgetRef))
	{
		return;
	}
    
	if (!PlayerController || !InventoryWidgetClass)
	{
		return;
	}
    
	InventoryWidgetRef = CreateWidget<UUserWidget>(PlayerController, InventoryWidgetClass);
}

void UInventoryComponent::OpenInventoryWidget()
{
	UInventoryUtilityLibrary::PlaySoundOnOpeningWidget(GetWorld());
	
	IWidgetManager* WidgetManager = Cast<IWidgetManager>(PlayerController);
	if(WidgetManager)
	{
		WidgetManager->SetActiveWidget(EWidgetType::INVENTORY);
		WidgetManager->SetActiveTab(EWidgetType::INVENTORY);
		WidgetManager->StartPlayerCapture();
	}
	
	PresentModalWidget(InventoryWidgetRef);
}

void UInventoryComponent::CloseInventoryWidget()
{
	if (nullptr == PlayerController)
	{
		DebugHeader::LogWarning("UInventoryComponent::CloseInventoryWidget: PlayerController is nullptr");
		return;
	}
	
	IWidgetManager* WidgetManager = Cast<IWidgetManager>(PlayerController);
	if(WidgetManager)
	{
		WidgetManager->SetActiveWidget(EWidgetType::NONE);
		WidgetManager->SetActiveTab(EWidgetType::NONE);
		WidgetManager->StopPlayerCapture();
	}
	DismissModalWidget(InventoryWidgetRef);
	Server_ChangeDroppedIndex(0);
}

void UInventoryComponent::OpenLootBarWidget()
{
	if (nullptr == PlayerController || nullptr == PlayerController->GetHUD())
	{
		DebugHeader::LogWarning("UInventoryComponent::OpenLootBarWidget: PlayerController or HUD is nullptr");
		return;
	}
	UInventoryUtilityLibrary::PlaySoundOnOpeningWidget(GetWorld());
	
	IsLootBarOpen = true;
	IsInteractableActorWidgetOpen= true;
	
	IHUDManager* HUDManager = Cast<IHUDManager>(PlayerController->GetHUD());
	if(HUDManager)
	{
		HUDManager->ShowLootBar();
	}
	PlayerController->bShowMouseCursor = true;
	PlayerController->SetIgnoreMoveInput(true);
	PlayerController->SetIgnoreLookInput(true);
}

void UInventoryComponent::CloseLootBarWidget()
{
	if (nullptr == PlayerController || nullptr == PlayerController->GetHUD())
	{
		DebugHeader::LogWarning("UInventoryComponent::CloseLootBarWidget: PlayerController or HUD is nullptr");
		return;
	}
	IsInteractableActorWidgetOpen = false;
	IsLootBarOpen = false;

	IHUDManager* HUDManager = Cast<IHUDManager>(PlayerController->GetHUD());
	if(HUDManager)
	{
		HUDManager->HideLootBar();
	}
	
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(PlayerController,false);
	PlayerController->bShowMouseCursor = false;
	PlayerController->ResetIgnoreInputFlags();
}

void UInventoryComponent::CreateVendorWidget()
{
	if (nullptr == PlayerController)
	{
		DebugHeader::LogWarning("UInventoryComponent::OpenVendorWidget: PlayerController is nullptr");
		return;
	}
}

void UInventoryComponent::OpenVendorWidget()
{
	UInventoryUtilityLibrary::PlaySoundOnOpeningWidget(GetWorld());

	IsInteractableActorWidgetOpen = true;

	IWidgetManager* WidgetManager = Cast<IWidgetManager>(PlayerController);
	if(WidgetManager)
	{
		WidgetManager->SetActiveWidget(EWidgetType::VENDOR);
		WidgetManager->SetActiveTab(EWidgetType::VENDOR);
		WidgetManager->StartPlayerCapture();
	}
	
	PresentModalWidget(VendorWidgetRef);
}

void UInventoryComponent::CloseVendorWidget()
{
	if (nullptr == PlayerController)
	{
		DebugHeader::LogWarning("UInventoryComponent::CloseVendorWidget: PlayerController is nullptr");
		return;
	}

}

void UInventoryComponent::CreateStorageWidget()
{
	if (nullptr == PlayerController)
	{
		DebugHeader::LogWarning("UInventoryComponent::OpenVendorWidget: PlayerController is nullptr");
		return;
	}
	UInventoryUtilityLibrary::PlaySoundOnOpeningWidget(GetWorld());

	IsInteractableActorWidgetOpen = true;
	
	IWidgetManager* WidgetManager = Cast<IWidgetManager>(PlayerController);
	if(WidgetManager)
	{
		WidgetManager->SetActiveWidget(EWidgetType::VENDOR);
		WidgetManager->SetActiveTab(EWidgetType::VENDOR);
		WidgetManager->StartPlayerCapture();
	}
	
	StorageWidgetRef = CreateWidget(PlayerController,StorageWidgetClass);
	if(!StorageWidgetRef)
	{
		DebugHeader::LogWarning("UInventoryComponent::OpenStorageWidget: StorageWidgetRef or StorageWidgetClass is nullptr");
		return;
	}
}

void UInventoryComponent::OpenStorageWidget()
{
		
	IsInteractableActorWidgetOpen = false;
	IWidgetManager* WidgetManager = Cast<IWidgetManager>(PlayerController);
	if(WidgetManager)
	{
		WidgetManager->SetActiveWidget(EWidgetType::STORAGE);
		WidgetManager->SetActiveTab(EWidgetType::STORAGE);
		WidgetManager->StartPlayerCapture();
	}
	DismissModalWidget(VendorWidgetRef);
	
	PresentModalWidget(StorageWidgetRef);
}

void UInventoryComponent::CloseStorageWidget()
{
	if (nullptr == PlayerController)
	{
		DebugHeader::LogWarning("UInventoryComponent::CloseStorageWidget: PlayerController is nullptr");
		return;
	}

	IsInteractableActorWidgetOpen = false;
	IWidgetManager* WidgetManager = Cast<IWidgetManager>(PlayerController);
	if (WidgetManager)
	{
		WidgetManager->SetActiveWidget(EWidgetType::NONE);
		WidgetManager->SetActiveTab(EWidgetType::NONE);
		WidgetManager->StartPlayerCapture();
	}
	DismissModalWidget(StorageWidgetRef);
}

#pragma endregion 

#pragma region NetworkFunction

void UInventoryComponent::Server_TakeItem_Implementation(UInventoryCoreComponent* Sender,FItemData InItemData,
	AActor* OwningPlayer)
{
	TakeItem(Sender,InItemData,OwningPlayer);
}

void UInventoryComponent::Server_TakeAllItems_Implementation(UInventoryCoreComponent* Sender, AActor* OwningPlayer)
{
	TakeAllItems(Sender,OwningPlayer);
}

void UInventoryComponent::Server_SpawnLootBag_Implementation(FItemData InItemData, AActor* OwningPlayer)
{
	SpawnLootBagNearThePlayer(InItemData,OwningPlayer);
}

void UInventoryComponent::Server_SpawnInteractable_Implementation(FItemData InItemData, AActor* OwningPlayer)
{
	SpawnItemMeshNearThePlayer(InItemData,OwningPlayer);
}

void UInventoryComponent::Server_DropItemOnTheGround_Implementation(const FItemData& InItemData, EItemDestination Initiator,
	AActor* OwningPlayer)
{
	DropItemOnTheGround(InItemData,Initiator,OwningPlayer);
}

void UInventoryComponent::Server_ModifyItemDurability_Implementation(const FItemData& InItemData, int32 Amount,
	AActor* OwningPlayer)
{
	ModifyItemDurability(InItemData,Amount,OwningPlayer);
}

void UInventoryComponent::Server_ChangeDroppedIndex_Implementation(int32 InDroppedIndex)
{
	DroppedIndex = InDroppedIndex;
}

void UInventoryComponent::Client_ItemLooted_Implementation(const FItemData& InItemData)
{
	OnItemLooted.Broadcast(InItemData);
}

void UInventoryComponent::Client_TakeItemReturnValue_Implementation(bool Success, const FString& FailureMessage, bool RemoveInteraction)
{
	if(!Success)
	{
		IWidgetManager* WidgetManager = Cast<IWidgetManager>(PlayerController);
		if(WidgetManager)
		{
			WidgetManager->DisplayMessageNotify(FText::FromString(FailureMessage));
		}
		return;
	}

	// UInventoryUtilityLibrary::PlaySoundOnItemPickuedUp();
	if(RemoveInteraction)
	{
		IObjectInteraction* ObjectInteraction = Cast<IObjectInteraction>(PlayerController);
		if(ObjectInteraction)
		{
			ObjectInteraction->RemoveInteractionWithObject(GetCurrentInteractable());
		}
	}
}

void UInventoryComponent::Client_TransferItemReturnValue(bool Success, const FString& FailureMessage)
{
	if(!Success)
	{
		IWidgetManager* WidgetManager = Cast<IWidgetManager>(PlayerController);
		if(WidgetManager)
		{
			WidgetManager->DisplayMessageNotify(FText::FromString(FailureMessage));
		}
	}
}
#pragma endregion