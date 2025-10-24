

#include "Quest/Component/QuestManager.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Header/DebugHeader.h"
#include "Quest/RPGQuest.h"
//#include "Quest/Quest.h"

UQuestManager::UQuestManager()
{

	PrimaryComponentTick.bCanEverTick = true;

}


void UQuestManager::InitializeComponent()
{
	/* Get the Asset RegistryModule */
	FAssetRegistryModule& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	IAssetRegistry& AssetRegistry = AssetRegistryModule.Get();

	TArray<FAssetData> QuestDataAssets;

	/* Get all the Quest Data within the PLUGIN FOLDER */
// #if UE_VERSION_NEWER_THAN(5, 1, 9)
// 	FTopLevelAssetPath assetPath = UKismetSystemLibrary::MakeTopLevelAssetPath("/Script/AdvancedQuest", "AQ_QuestData");
// 	AssetRegistry.GetAssetsByClass(assetPath, QuestDataAssets, true);
// #else 
// 	FTopLevelAssetPath assetPath = FTopLevelAssetPath("/Script/AdvancedQuest", "AQ_QuestData");
// 	AssetRegistry.GetAssetsByClass(assetPath, QuestDataAssets, true);
// #endif

	/* Create a quest for each Quest Data found */
	for (auto assets : QuestDataAssets)
	{
		URPGQuest* QuestData = Cast<URPGQuest>(assets.GetAsset());
	
		if (!QuestData)
			return;
	
		URPGQuest* newQuest = NewObject<URPGQuest>(this, URPGQuest::StaticClass());
		// newQuest->SetQuestData(QuestData);
	
		// int QuestID = QuestData->QuestID;
	
		/* Add the quest in QuestDataCenter with it's ID as key to retrieve it easily*/
		// QuestDataCenter.Add(QuestID, newQuest);

		// if (QuestData->QuestType == EAQ_QuestType::Daily)
		// 	OnNewDayDelegate.AddUniqueDynamic(newQuest, &UAQ_Quest::OnNewDay);
		//
		// if (QuestData->QuestType == EAQ_QuestType::Weekly)
		// 	OnNewWeekDelegate.AddUniqueDynamic(newQuest, &UAQ_Quest::OnNewDay);
	}

	/* Load the save to update all the Objectives & current state for each quests */
	// LoadQuestData(); // This function is implemented in blueprint
}

void UQuestManager::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->OnWorldBeginPlay.AddUObject(this, &UQuestManager::LateBeginPlay);
}

void UQuestManager::LateBeginPlay()
{
	// /* Get the Player Channels component */
	// UAQ_PlayerChannels* playerChannels = GetOwner()->FindComponentByClass<UAQ_PlayerChannels>();
	// if (!playerChannels)
	// 	return;
	//
	// /* Generate an array with all the quests in the QuestDataCenter */
	// TArray<URPGQuest*> temporaryQuests;
	// QuestDataCenter.GenerateValueArray(temporaryQuests);
	//
	// /* Call On Quest Created for each quests */
	// for (auto quests : temporaryQuests)
	// {
	// 	/* According to the state of each quests, the Player Channels will update
	// 	the Quest channel and its book quest, and Functions will be bind to delegates */
	// 	playerChannels->OnQuestCreated(quests);
	// }
}

URPGQuest* UQuestManager::QueryQuest(int QuestID)
{
	/* Find the Quest corresponding to the ID */
	if (QuestDataCenter.Find(QuestID))
		return QuestDataCenter[QuestID];

	DebugHeader::LogWarning("THERE IS NO QUEST WITH %i ID",QuestID);;
	return nullptr;
}


