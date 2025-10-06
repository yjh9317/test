// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Crafting/Component/BaseCraftingStation.h"
#include "Item/Data/ItemDataStructs.h"
#include "StationsManager.generated.h"

class UBaseCraftingStation;
DECLARE_MULTICAST_DELEGATE(FOnStationActorChanged);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnNewItemSelected,const FCraftingData& CraftingData);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UStationsManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStationsManager();

	FOnStationActorChanged OnStationActorChanged;
	FOnNewItemSelected OnNewItemSelected;

	UFUNCTION(Client,Reliable)
	void Client_InitializeStationsManager(APlayerController* PC);

	UFUNCTION()
	virtual void EventStationDestroyed(AActor* Actor,EEndPlayReason::Type EndPlayReason);

	UFUNCTION(Server,Reliable)
	void Server_InitializeCraftingProcess(UBaseCraftingStation* Sender,const FCraftingData& InCraftingData,int32 AmountToCraft, AActor* OwningActor);

	UFUNCTION(Server,Reliable)
	void Server_TryToCraftItem(UBaseCraftingStation* Sender, const FCraftingData& InCraftingData,AActor* OwningActor);

	UFUNCTION(Server,Reliable)
	void Server_ReturnSecuredMaterials(UBaseCraftingStation* Sender, const FCraftingData& InCraftingData,AActor* OwningActor);

	UFUNCTION(Client,Reliable)
	void Client_UnlockCraftingRecipe(const FString& CraftingID);

	UFUNCTION(Client,Reliable)
	void Client_TryToCraftReturnValue(UBaseCraftingStation* Sender,bool ReturnValue,const FText& Message);
	
	UFUNCTION(Client,Reliable)
	void Client_InitializeCraftingProcessReturnValue(UBaseCraftingStation* Sender,const FCraftingData& InCraftingData,int32 AmountToCraft,bool ReturnValue,const FText& Message);
	
	UFUNCTION(Client,Reliable)
	void ClientRefreshUIData(UBaseCraftingStation* Target);

	void SetCurrentCraftingStationActor(AActor* StationActor);
	AActor* GetCurrentCraftingStationActor() const;
	void BindStationEndPlayEvent();
	void UnbindStationEndPlayEvent();
	bool TryToInitializeCraftingProcess(UBaseCraftingStation* Sender,const FCraftingData& InCraftingData,int32 AmountToCraft,AActor* OwningActor,FText& FailureMessage);
	bool TryToCraftItem(UBaseCraftingStation* Sender,const FCraftingData& InCraftingData,AActor* OwningActor,const FText& FailureMessage);
	void GetCurrentCraftableData(FItemData& OutItemData,TArray<FItemStack>& RequiredMaterials);
	void AssignCraftableData(const FCraftingData& InCraftingData);
	bool CanBeAddedToInventory(const TArray<FItemData>& CraftableItems,UBaseCraftingStation* CraftingStation,AActor* OwningPlayer);
	void FocusSelectedItem(const FCraftingData& InCraftingData,UBaseCraftingStation* Station);
	void AttachNewItemToPlayerPreview(const FCraftingData& InCraftingData);
	bool SecureMaterialItems(const TArray<FItemStack>& Materials,int32 AmountToCraft,AActor* OwningPlayer,TArray<FItemData>& SecuredMaterials);
	void ReturnSecuredMaterials(const FCraftingData& InCraftingData,AActor* OwningPlayer);
	bool CanCraft(UBaseCraftingStation* CraftingStation);
	void SecureCraftingCost(const FCraftingData& InCraftingData,int32 AmountToCraft,UBaseCraftingStation* CraftingStation,AActor* OwningPlayer);
	void ReturnSecuredCraftingCost(const FCraftingData& InCraftingData,UBaseCraftingStation* CraftingStation,AActor* OwningPlayer);
	bool IsCraftingRecipeLocked(const FCraftingData& InCraftingData);
	bool UnlockCraftingRecipe(const FString& CraftingID);
	int32 FindItemQuantity(const FItemData& InItemData,AActor* OwningPlayer);
	void OpenCraftingWidget(AActor* StationActor,TSubclassOf<UUserWidget> CraftingWidgetClass);
	void CloseCraftingWidget();
	
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Station Managing")
	TObjectPtr<AActor> CurrentStationActor;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Crafting System")
	FCraftingData SelectedCraftingData;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Crafting System")
	FItemData PreviewItemData;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Crafting System")
	FItemData StoredPreviewData;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Crafting System")
	bool bIsCraftingWidgetOpen;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Crafting System")
	TSet<FString> UnlockedRecipes;

	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Reference")
	TObjectPtr<UUserWidget> CraftingWidgetRef;
	
	UPROPERTY(VisibleDefaultsOnly,BlueprintReadOnly,Category="Reference")
	TObjectPtr<APlayerController> PlayerController;
};
