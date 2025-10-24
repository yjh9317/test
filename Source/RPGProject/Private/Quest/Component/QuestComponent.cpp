// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/Component/QuestComponent.h"

#include "AnalyticsEventAttribute.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Quest/Component/QuestManager.h"
#include "Quest/UI/Giver/QuestMarkerWidget.h"

UQuestComponent::UQuestComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Owner = GetOwner();
}

UQuestComponent::~UQuestComponent()
{
}


void UQuestComponent::BeginPlay()
{
	Super::BeginPlay();
#if WITH_EDITOR
	/* Return if the play mode is Simulate */
	if (GetWorld()->WorldType == EWorldType::PIE && GEditor->IsSimulateInEditorInProgress())
		return;
#endif

	GetWorld()->OnWorldBeginPlay.AddUObject(this, &UQuestComponent::LateBeginPlay);

	RemoveVisualMarker();
}

void UQuestComponent::LateBeginPlay()
{
	/* Get the local Player */
	APawn* localPlayer = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!localPlayer)
		return;

	/* Get the quest manager */
	QuestManager = localPlayer->FindComponentByClass<UQuestManager>();

	if (QuestMarkerClass)
		CreateQuestMarkerWidget();

	BindFunctionsToQuestDelegates();
}

void UQuestComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(QuestMarkerWidget && QuestMarkerWidget->IsVisible())
		MarkerFloatingMovement(DeltaTime);
}


void UQuestComponent::MarkerFloatingMovement(float DeltaTime)
{
	if (!CameraManager)
		CameraManager = GetWorld()->GetFirstLocalPlayerFromController()->PlayerController->PlayerCameraManager;

	if (CameraManager)
	{
		CurrentDelta += DeltaTime;

		float floatingEffect = sin(CurrentDelta * FloatingSpeed) * FloatingAmplitude;

		QuestMarkerWidget->SetWorldRotation(UKismetMathLibrary::FindLookAtRotation(QuestMarkerWidget->GetComponentLocation(), CameraManager->GetCameraLocation()));
		QuestMarkerWidget->AddWorldOffset(FVector(0, 0, floatingEffect));
	}
}

void UQuestComponent::SetQuestMarker(bool isMarkerVisible, bool isQuestValid, EQuestType QuestState)
{
	/* If no quest to show, set the component to be silent */
	if (isMarkerVisible)
		bIsComponentSilent = false;
	else
		bIsComponentSilent = true;

	OnSleepStateChanged.Broadcast(bIsComponentSilent);

	if (!QuestMarkerClass)
		return;

	UQuestMarkerWidget* widget = Cast<UQuestMarkerWidget>(QuestMarkerWidget->GetWidget());

	/* Update the quest marker */
	if (widget)
	{
		QuestMarkerWidget->SetVisibility(isMarkerVisible);
		widget->SetImageQuest(isQuestValid, QuestState);
	}
}


void UQuestComponent::UpdateQuestMarker()
{
	if (!QuestMarkerClass || !QuestManager)
		return;


	/* If there is no quest we hide the quest Marker and return */
	if (QuestList.Num() == 0)
	{
		SetQuestMarker(false, false);
		return;
	}


	// SORT 
	bool bIsAnyQuestValid = false;
	bool bIsAnyQuestPending = false;
	TArray<URPGQuest*> PendingQuests;
	TArray<URPGQuest*> ValidQuests;

	for (auto QuestData : QuestList)
	{
		/* Query to the QuestManager the quest of the corresponding ID */
		URPGQuest* quest = QuestManager->QueryQuest(QuestData.Key);

		if (quest == nullptr)
			continue;

		/* Check if there is any Valid Quests */
		if (QuestData.Value.bIsQuestReceiver && quest->QuestState == EQuestState::Valid)
		{
			ValidQuests.Add(quest);
			bIsAnyQuestValid = true;
		}
		/* Check if there is any Pending Quests*/
		else if (QuestData.Value.bIsQuestGiver 
			&& quest->QuestState == EQuestState::Pending 
			&& quest->bIsRequirementMet
			&& !bIsAnyQuestValid)
		{
			PendingQuests.Add(quest);
			bIsAnyQuestPending = true;
		}
	}

	/* Check the Quest Type of the Valid Quests */
	EQuestType currentType = EQuestType::EQuest_Weekly;
	if (bIsAnyQuestValid)
	{
		CheckQuestTypes(ValidQuests, currentType);
		SetQuestMarker(true, true, currentType);
		return;
	}

	/* Check the Quest Type of the Pending Quests */
	if (bIsAnyQuestPending)
	{
		CheckQuestTypes(PendingQuests, currentType);
		SetQuestMarker(true, false, currentType);
		return;
	}

	/* If no Pending nor Valid Quests, hide the Quest Marker */
	SetQuestMarker(false, false);
}

void UQuestComponent::CheckQuestTypes(TArray<URPGQuest*>& ValidQuests, EQuestType& currentType)
{
	for (auto quests : ValidQuests)
	{
		switch (quests->QuestData->QuestType)
		{
		case EQuestType::EQuest_MainQuest:
			currentType = EQuestType::EQuest_MainQuest;
			return;

		case EQuestType::EQuest_SubQuest:
			currentType = EQuestType::EQuest_SubQuest;
			break;

		case EQuestType::EQuest_Daily:
		case EQuestType::EQuest_Weekly:
			{
				if (currentType != EQuestType::EQuest_SubQuest)
					currentType = quests->QuestData->QuestType;
				break;
			}
		}
	}
}

void UQuestComponent::RerunScript()
{
#if WITH_EDITOR
	AActor* Actor = GetOwner();
	Actor->RerunConstructionScripts();
#endif
}


void UQuestComponent::OnQuestStateChangedWrapper(URPGQuest* questUpdate, EQuestState QuestState)
{
	UpdateQuestMarker();
	OnQuestStateChanged.Broadcast(QuestState); // Trigger the blueprint event

	/* If the Quest is now archive, we unbind to the delegates */
	if (questUpdate->QuestState == EQuestState::Archive)
	{
		/* Unbind only if it's not a daily quest */
		if (questUpdate->QuestData->QuestType != EQuestType::EQuest_Daily &&
			questUpdate->QuestData->QuestType != EQuestType::EQuest_Weekly)
			questUpdate->QuestStateChangedDelegate.RemoveDynamic(this, &UQuestComponent::OnQuestStateChangedWrapper);
	}
}

void UQuestComponent::OnQuestRequirementMet(URPGQuest* quest)
{
	UpdateQuestMarker();

	/* Unbind to the Requirement Met delegate */
	quest->QuestRequirementMetDelegate.RemoveDynamic(this, &UQuestComponent::OnQuestRequirementMet);
	
	/* Bind to the Quest State Changed delegate*/
	quest->QuestStateChangedDelegate.AddDynamic(this, &UQuestComponent::OnQuestStateChangedWrapper);
}

void UQuestComponent::BindFunctionsToQuestDelegates()
{
	if (!QuestManager)
		return;

	bool bIsAnyQuestPending = false;
	bool bIsAnyQuestValid = false;

	for (auto QuestData : QuestList)
	{
		/* Query the quests to the Quest Manager Data Center*/
		URPGQuest* newQuest = QuestManager->QueryQuest(QuestData.Key);

		if (newQuest == nullptr)
			continue;

		/* Subscribe to the Quest Requirement Met Delegate if needed */
		if(!newQuest->bIsRequirementMet)
			newQuest->QuestRequirementMetDelegate.AddDynamic(this, &UQuestComponent::OnQuestRequirementMet);
		else
		{
			/* Subscribe to the Quest State Changed Delegate if the Quest isn't archived */
			/* Update the Quest Marker */
			switch (newQuest->QuestState)
			{
			case EQuestState::Valid:
			case EQuestState::Pending:
			case EQuestState::Active:
				newQuest->QuestStateChangedDelegate.AddDynamic(this, &UQuestComponent::OnQuestStateChangedWrapper);
				break;

			default:
				break;
			}

			/* Bind also to QuestStateChangedDelegate if the quest is a daily */
			if (newQuest->QuestData->QuestType == EQuestType::EQuest_Daily || 
				newQuest->QuestData->QuestType == EQuestType::EQuest_Weekly)
			{
				if(newQuest->QuestState == EQuestState::Archive)
					newQuest->QuestStateChangedDelegate.AddDynamic(this, &UQuestComponent::OnQuestStateChangedWrapper);
			}
		}
	}
	
	UpdateQuestMarker();
}


void UQuestComponent::CreateQuestMarkerWidget()
{
	if(!Owner) return;
	
	FVector origin, extent;
	Owner->GetActorBounds(true, origin, extent);


	QuestMarkerWidget = Cast<UWidgetComponent>(GetOwner()->AddComponentByClass(UWidgetComponent::StaticClass(), false, FTransform(), false));

	// Check if the WidgetComponent was successfully added
	if (QuestMarkerWidget)
	{
		QuestMarkerWidget->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::SnapToTargetIncludingScale);
		QuestMarkerWidget->SetWidgetClass(QuestMarkerClass);
		QuestMarkerWidget->SetMaterial(0, QuestMarkerMaterial);
		QuestMarkerWidget->SetWidgetSpace(EWidgetSpace::World);
		QuestMarkerWidget->RegisterComponent();
		QuestMarkerWidget->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		float zCoord = extent.Z * 2 + zOffset;
		QuestMarkerWidget->SetRelativeLocation(FVector(0, 0, zCoord));
		QuestMarkerWidget->SetDrawSize(FVector2D(512.f));
		QuestMarkerWidget->SetWorldScale3D(FVector(.2f));
	}

	SetQuestMarker(false, false);
}

void UQuestComponent::ShowVisualMarker()
{
	if(VisualWidget) return;
	
	TArray<AActor*> TempArray = { Owner };
	FVector OutCenter{}, OutBoxExtent{};
	UGameplayStatics::GetActorArrayBounds(TempArray,true,OutCenter,OutBoxExtent);
	float ZExtend = OutBoxExtent.Z;

	
}

void UQuestComponent::RemoveVisualMarker()
{
	if(!VisualWidget)
	{
		VisualWidget = nullptr;
		RerunScript();
	}
}