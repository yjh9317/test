
#include "Quest/Component/QuestListenerComponent.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Quest/Component/QuestManager.h"


UQuestListenerComponent::UQuestListenerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


// Called when the game starts
void UQuestListenerComponent::BeginPlay()
{
	Super::BeginPlay();

#if WITH_EDITOR
	// GIsEditor; && GEditor->IsSimulateInEditorInProgress()
	if (GetWorld()->WorldType == EWorldType::PIE && GIsEditor)
		return;
#endif

	/* Get the local Player */
	APawn* localPlayer = GetWorld()->GetFirstPlayerController()->GetPawn();
	if (!localPlayer)
		return;

	/* Get the quest manager */
	QuestManager = localPlayer->FindComponentByClass<UQuestManager>();
	ListenToQuests();
}

void UQuestListenerComponent::ListenToQuests()
{
	if (!QuestManager) return;

	for (auto quest : QuestToFollow)
	{
		URPGQuest* questToFollow = QuestManager->QueryQuest(quest);
		if (!questToFollow) continue;
		
		questToFollow->QuestStateChangedDelegate.AddDynamic(
			this,
			&UQuestListenerComponent::OnQuestStateChangedWrapper
		);
	}
}

void UQuestListenerComponent::OnQuestStateChangedWrapper(URPGQuest* questUpdate, EQuestState QuestState)
{
	OnQuestStateChanged.Broadcast(questUpdate, QuestState);

	/* Unbind if the quest isn't active anymore */
	switch (questUpdate->QuestState)
	{
	case EQuestState::Pending:
	case EQuestState::Failed:
	case EQuestState::Archive:
		{
			questUpdate->QuestStateChangedDelegate.RemoveDynamic(this, &UQuestListenerComponent::OnQuestStateChangedWrapper);
			break;
		}

	default:
		break;
	}
}

void UQuestListenerComponent::BindFunctionsToQuestDelegates(URPGQuest* quest)
{
	if (!QuestManager
	|| quest == nullptr
	|| !quest->bIsRequirementMet)
		return;

	quest->QuestStateChangedDelegate.AddDynamic(this, &UQuestListenerComponent::OnQuestStateChangedWrapper);
}


