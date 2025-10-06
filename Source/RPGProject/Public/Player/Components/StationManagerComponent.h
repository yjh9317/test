// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory/Data/InventoryDatas.h"
#include "Item/Data/ItemDataStructs.h"
#include "StationManagerComponent.generated.h"


class UCraftingWindowBase;
class UEquipmentComponent;
class UBaseCraftingStation;
class UInventoryComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStationActorChanged);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNewItemSelected, const FCraftingData&, CraftingData);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UStationManagerComponent : public UActorComponent
{
	GENERATED_BODY()


public:
	// Event Station End Play - when the Station would be (for some reason) destroyed (can no longer be referenced)
	UFUNCTION()
	void EventStationDestroyed(AActor* InActor,EEndPlayReason::Type EndPlayReason);

	// SERVER EVENTS
	UFUNCTION(Server,Reliable)
	void Server_InitializeCraftingProcess(UBaseCraftingStation* Sender,const FCraftingData& InCraftingData,int32 AmountToCraft,AActor* InOwningPlayer);

	UFUNCTION(Server,Reliable)
	void Server_TryToCraftItem(UBaseCraftingStation* Sender,const FCraftingData& InCraftingData,AActor* InOwningPlayer);

	UFUNCTION(Server,Reliable)
	void Server_ReturnSecuredMaterials(UBaseCraftingStation* Sender,const FCraftingData& InCraftingData,AActor* InOwningPlayer);
	// CLIENT EVENTS
	UFUNCTION(Client,Reliable)
	void Client_UnlockCraftingRecipe(const FString& InCraftingID);

	UFUNCTION(Client,Reliable)
	void Client_TryToCraftReturnValue(UBaseCraftingStation* Sender,bool ReturnValue,const FString& Message);

	UFUNCTION(Client,Reliable)
	void Client_InitializeCraftingProcessReturnValue(UBaseCraftingStation* Sender,const FCraftingData& InCraftingData,int32 AmountToCraft,bool ReturnValue,const FString& Message);
	
	UFUNCTION(Client,Reliable)
	void Client_RefreshUIData(UBaseCraftingStation* Target);
	
	UFUNCTION(Client,Reliable)
	void Client_InitializeStationsManager(APlayerController* InPlayerController);
	
	UFUNCTION(BlueprintCallable, Category="StationManaging")
	void SetCurrentCraftingStationActor(AActor* StationActor);
	
	UFUNCTION(BlueprintPure, Category="StationManaging")
	AActor* GetCurrentCraftingStationActor() const;
	
private:
	
	UFUNCTION(BlueprintCallable, Category="StationManaging")
	void BindStationEndPlayEvent();
	
	UFUNCTION(BlueprintCallable, Category="StationManaging")
	void UnbindStationEndPlayEvent();
public:
	
	UFUNCTION(BlueprintCallable, Category="CraftingSystem")
	bool TryToInitializeCraftingProcess(const FCraftingData& InCraftingData, int32 AmountToCraft, UBaseCraftingStation* Sender, AActor* InOwningPlayer, FText& FailureMessage);
	
	UFUNCTION(BlueprintCallable, Category="CraftingSystem")
	bool TryToCraftItem(const FCraftingData& InCraftingData, UBaseCraftingStation* Sender, AActor* InOwningPlayer, FText& FailureMessage);
	
	UFUNCTION(BlueprintPure, Category="CraftingSystem")
	void GetCurrentCraftableData(FItemData& OutItemData, TArray<FItemStack>& OutRequiredMaterials);
	
	UFUNCTION(BlueprintCallable, Category="CraftingSystem")
	void AssignCraftableData(const FCraftingData& InCraftingData);
	
	UFUNCTION(BlueprintPure, Category="CraftingSystem")
	bool CanBeAddedToInventory(UPARAM(ref) const TArray<FItemData>& CraftableItems, AActor* InOwningPlayer, UBaseCraftingStation* CraftingStation);
	
	UFUNCTION(BlueprintCallable, Category="CraftingSystem")
	void FocusSelectedItem(const FCraftingData& InCraftingData, UBaseCraftingStation* Station);
	
	UFUNCTION(BlueprintCallable, Category="CraftingSystem")
	void AttachNewItemToPlayerPreview(const FCraftingData& InCraftingData);
	
	UFUNCTION(BlueprintCallable, Category="CraftingSystem")
	bool SecureMaterialItems(UPARAM(ref)const TArray<FItemStack>& Materials, int32 AmountToCraft, AActor* InOwningPlayer, TArray<FItemData>& OutSecuredMaterials);
	
	UFUNCTION(BlueprintCallable, Category="CraftingSystem")
	void ReturnSecuredMaterials(const FCraftingData& InCraftingData, AActor* InOwningPlayer);
	
	UFUNCTION(BlueprintPure, Category="CraftingSystem")
	bool CanCraft(UBaseCraftingStation* CraftingStation);
	
	UFUNCTION(BlueprintCallable, Category="CraftingSystem")
	void SecureCraftingCost(const FCraftingData& InCraftingData, int32 AmountToCraft, UBaseCraftingStation* CraftingStation, AActor* InOwningPlayer);
	
	UFUNCTION(BlueprintCallable, Category="CraftingSystem")
	void ReturnSecuredCraftingCost(const FCraftingData& InCraftingData, UBaseCraftingStation* CraftingStation, AActor* InOwningPlayer);
	
	UFUNCTION(BlueprintPure, Category="CraftingSystem")
	bool IsCraftingRecipeLocked(const FCraftingData& InCraftingData);
	
	UFUNCTION(BlueprintCallable, Category="CraftingSystem")
	bool UnlockCraftingRecipe(const FString& InCraftingID);
	
	UFUNCTION(BlueprintCallable, Category="CraftingSystem")
	void FindItemQuantity(const FItemData& InItemData, AActor* InOwningPlayer, int32& QuantityFound);
	
	UFUNCTION(BlueprintCallable, Category="Widgets")
	void OpenCraftingWidget(AActor* StationActor, UClass* CraftingWidget);
	
	UFUNCTION(BlueprintCallable, Category="Widgets")
	void CloseCraftingWidget();
	
private:
	
	UPROPERTY(EditDefaultsOnly, Category="StationManaging", AdvancedDisplay, meta=(MultiLine="true"))
	TObjectPtr<AActor> CurrentStationActor;
public:
	
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category="StationManaging", AdvancedDisplay, meta=(MultiLine="true"))
	FOnStationActorChanged OnStationActorChanged;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="CraftingSystem", AdvancedDisplay, meta=(MultiLine="true"))
	FCraftingData SelectedCraftingData;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default", AdvancedDisplay, meta=(MultiLine="true"))
	TObjectPtr<APlayerController> PlayerController;
	
	UPROPERTY(BlueprintAssignable, EditDefaultsOnly, Category="CraftingSystem", AdvancedDisplay, meta=(MultiLine="true"))
	FOnNewItemSelected OnNewItemSelected;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="CraftingSystem", AdvancedDisplay, meta=(MultiLine="true"))
	FItemData PreviewItemData;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="CraftingSystem", AdvancedDisplay, meta=(MultiLine="true"))
	FItemData StoredPreviewData;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="CraftingSystem", AdvancedDisplay, meta=(MultiLine="true"))
	bool bIsCraftingWidgetOpen;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="CraftingSystem", meta=(MultiLine="true"))
	TSet<FString> UnlockedRecipes;
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Widget", AdvancedDisplay, meta=(MultiLine="true"))
	TObjectPtr<UCraftingWindowBase> CraftingWidgetRef;
};
