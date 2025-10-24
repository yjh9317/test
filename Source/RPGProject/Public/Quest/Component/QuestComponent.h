// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Quest/Data/QuestStruct.h"
#include "Quest/RPGQuest.h"
#include "Quest/Data/RPGQuestData.h"
#include "Quest/UI/Giver/QuestMarkerWidget.h"
#include "QuestComponent.generated.h"

class APlayerCameraManager;
class UQuestManager;
class UQuestMarkerWidget;
// class URPGQuest;
class UWidgetComponent;
class UUserWidget;

USTRUCT(Blueprintable, BlueprintType)
struct FIsGiverOrReceiver
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Component")
	bool bIsQuestReceiver = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Component")
	bool bIsQuestGiver = true;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStateChangedDelegate, EQuestState QuestState);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnSleepStateChangedDelegate, bool bIsComponentSilent);


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RPGPROJECT_API UQuestComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UQuestComponent();
	~UQuestComponent();
	

protected:
	virtual void BeginPlay() override;
	void LateBeginPlay();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Advanced Quest | Component")
	TMap<int, FIsGiverOrReceiver> QuestList;

	UFUNCTION(Category = "Quest | Events")
	void OnQuestStateChangedWrapper(URPGQuest* questUpdate, EQuestState QuestState);
	
	UFUNCTION(Category = "Quest | Events")
	void OnQuestRequirementMet(URPGQuest* quest);
	
	void BindFunctionsToQuestDelegates();
	
	/* Interaction */
	// UFUNCTION(BlueprintCallable, Category = "Advanced Quest | Component")
	// void Interact(const TScriptInterface<IAQ_PlayerChannelsFacade>& PlayerChannel);
	
	UPROPERTY(BlueprintReadOnly, Category = "Advanced Quest | Component")
	bool bIsComponentSilent = true;
	
	// Should the Quest Component be triggered before any other interaction
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Advanced Quest | Component")
	bool bHasPriority = true;
	
	/* Widgets */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | UI")
	TSubclassOf<UQuestMarkerWidget> QuestMarkerClass = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | UI")
	UMaterialInterface* QuestMarkerMaterial = nullptr; // Put a two sided material for the widget to be two sided
	
	// You can adjust this value if the quest Marker looks too low or to high of the actor
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Advanced Quest | UI")
	float zOffset = 75.f;
	
	UFUNCTION(BlueprintCallable, Category = "Advanced Quest | UI")
	void SetQuestMarker(bool isMarkerVisible, bool isQuestValid, EQuestType QuestState = EQuestType::EQuest_MainQuest);
	
	UFUNCTION(BlueprintCallable, Category = "Advanced Quest | UI")
	void UpdateQuestMarker();
	
	void CheckQuestTypes(TArray<URPGQuest*>& ValidQuests, EQuestType& currentType);

	void ShowVisualMarker();
	void RemoveVisualMarker();
protected:
	AActor* Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Advanced Quest | Component")
	UQuestManager* QuestManager = nullptr;

	/* Floating movement */
	float CurrentDelta = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Floating Movement")
	float FloatingSpeed = 2.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Advanced Quest | Floating Movement")
	float FloatingAmplitude = 0.05f;

	
	FOnStateChangedDelegate OnQuestStateChanged;
	FOnSleepStateChangedDelegate OnSleepStateChanged;
private:
	UPROPERTY()
	UWidgetComponent* QuestMarkerWidget = nullptr;
	void CreateQuestMarkerWidget();
	void MarkerFloatingMovement(float DeltaTime);

	/* References */
	UPROPERTY()
	TObjectPtr<APlayerCameraManager> CameraManager = nullptr;

	UPROPERTY()
	TObjectPtr<UWidgetComponent> VisualWidget;
	UPROPERTY()
	TObjectPtr<UQuestMarkerWidget> VisualMarkWidget;

	/* DEBUG */
	UFUNCTION(BlueprintCallable, Category = "Advanced Quest | Debug")
	void RerunScript(); // Usefull only to see all the quest Marker when using the Tool
};
