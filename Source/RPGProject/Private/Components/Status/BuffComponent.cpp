#pragma once
#include "Components/Status/BuffComponent.h"
#include "Inventory/Components/EquipmentComponent.h"
#include "Item/Consumable/BaseConsumable.h"
#include "Kismet/KismetSystemLibrary.h"

UBuffComponent::UBuffComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

}

void UBuffComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void UBuffComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBuffComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UBuffComponent::BeginDestroy()
{
	Super::BeginDestroy();
}

void UBuffComponent::UpdateActiveBuffs()
{
}

void UBuffComponent::RemoveExpiredBuffs()
{
}

void UBuffComponent::ApplyBuffEffects(const FActiveBuff& Buff, bool bApply)
{
}

void UBuffComponent::RecalculateBuffEffects()
{
}

FActiveBuff* UBuffComponent::FindActiveBuff(const FGameplayTag& BuffTag)
{
	return nullptr;
}

bool UBuffComponent::CanApplyBuff(const FBuffInfo& BuffInfo) const
{
	return true;
}

void UBuffComponent::SortBuffsByPriority()
{
}


// bool UBuffComponent::SpawnConsumableActor(UEquipmentComponent* EquipmentComponent, float& LocalCooldown)
// {
// 	if(EquipmentComponent)
// 	{
// 		FItemData SlotItemData;
// 		if(EquipmentComponent->GetItemByEquipmentSlot(UsedSlot,SlotItemData))
// 		{
// 			UItemBase* SlotItemBase = Cast<UItemBase>(SlotItemData.ItemClass->GetDefaultObject());
// 			if(nullptr == SlotItemBase) return false;
// 			
// 			TSoftClassPtr<AActor> SoftConsumableClass = SlotItemBase->ConsumableClass;
// 			if(UKismetSystemLibrary::IsValidSoftClassReference(SoftConsumableClass))
// 			{
// 				UClass* LoadClass = UKismetSystemLibrary::LoadClassAsset_Blocking(SoftConsumableClass);
// 				if(LoadClass)
// 				{
// 					TSubclassOf<AAbstractConsumable> Buffclass = LoadClass;
// 					LocalCooldown = Buffclass.GetDefaultObject()->Cooldown;
// 					AAbstractConsumable* LocalBuff{};
// 					if(IsBuffAlreadyInStack(Buffclass,LocalBuff))
// 					{
// 						RemoveConsumableBuffFromStack(LocalBuff);
// 			 			
// 						AAbstractConsumable* ConsumableActor = GetWorld()->SpawnActor<AAbstractConsumable>(Buffclass,GetOwner()->GetActorTransform());
// 						AddConsumableBuffToStack(ConsumableActor);
// 						return true;
// 					}
// 				}
// 			}
// 		}
// 	}
// 	return false;
// }
//
// bool UBuffComponent::TryToUseConsumableFromEquipmentSlot(EItemSlot Slot, float& Cooldown)
// {
// 	UsedSlot = Slot;
//
// 	UEquipmentComponent* EquipmentComponent = GetOwner()->FindComponentByClass<UEquipmentComponent>();
// 	float LocalCooldown = 0;
// 	
// 	if(SpawnConsumableActor(EquipmentComponent, LocalCooldown))
// 	{
// 		FItemData LocalItemData;
// 		EquipmentComponent->GetItemByEquipmentSlot(UsedSlot,LocalItemData);
// 		EquipmentComponent->ServerRemoveItemQuantity(LocalItemData,1);
// 		OnBuffItemUsed.Broadcast(UsedSlot,LocalCooldown);
// 		Cooldown = LocalCooldown;
// 		return true;
// 	}
// 	return false;
// }
//
// void UBuffComponent::AddConsumableBuffToStack(AAbstractConsumable* Buff)
// {
// 	if(Buff)
// 	{
// 		Buffs.Add(Buff->GetClass(),Buff);
// 	}
// }
//
// void UBuffComponent::RemoveConsumableBuffFromStack(AAbstractConsumable* Buff)
// {
// 	if(Buff) 
// 	{
// 		if(!Buff->IsActorBeingDestroyed())
// 		{
// 			Buffs.Remove(Buff->GetClass());
// 			Buff->RemoveBuff();
// 		}
// 	}
// }
//
// bool UBuffComponent::IsBuffAlreadyInStack(TSubclassOf<AAbstractConsumable> BuffClass, AAbstractConsumable*& OutBuff)
// {
// 	if(Buffs.Find(BuffClass))
// 	{
// 		OutBuff = Buffs[BuffClass];
// 		return true;
// 	}
// 	
// 	return false;
// }
//
// void UBuffComponent::InitializeCircularAnimation(const FGameplayTag& StatTag)
// {
// 	OnInitCircularAnimation.Broadcast(StatTag);
// }
//
// void UBuffComponent::ClearBuffsBar()
// {
// 	OnClearBuffsBar.Broadcast();
// }

// void UBuffComponent::ServerApplyBuff_Implementation(const FBuffInfo& BuffInfo, AActor* Instigator)
// {
// }
//
// void UBuffComponent::ServerRemoveBuff_Implementation(const FGameplayTag& BuffTag)
// {
// }
//
// void UBuffComponent::MulticastBuffApplied_Implementation(const FActiveBuff& AppliedBuff)
// {
// }
//
// void UBuffComponent::MulticastBuffRemoved_Implementation(const FGameplayTag& BuffTag)
// {
// }
// void UBuffComponent::OnRep_ActiveBuffs()
// {
// }