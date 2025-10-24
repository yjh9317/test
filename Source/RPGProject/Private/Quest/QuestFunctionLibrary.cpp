// Fill out your copyright notice in the Description page of Project Settings.


#include "Quest/QuestFunctionLibrary.h"

#include "UObject/Package.h"
#include "UObject/UObjectGlobals.h"
#include "Components/SceneComponent.h"
#include "GameFramework/Actor.h"
#include "Quest/External/QuestFilesManager.h"

#if WITH_EDITOR
#include <FileHelpers.h>
#endif

#define LOCTEXT_NAMESPACE "FAdvancedQuestModule"

int UQuestFunctionLibrary::AssignUniqueID(const FString& valueName)
{
	int LastQuestID = QuestFilesManager::LoadLastQuestID(valueName);
	int NewQuestID = LastQuestID + 1;

	// Save the new LastQuestID
	QuestFilesManager::SaveLastQuestID(NewQuestID, valueName);

	return NewQuestID;
}

UActorComponent* UQuestFunctionLibrary::AddComponent(TSubclassOf<class UActorComponent> ComponentClass, AActor* Actor, USceneComponent* ParentComponent, FName Name)
{
	if (!Actor)
		return nullptr;

#if WITH_EDITOR
	Actor->RerunConstructionScripts();
#endif

	UActorComponent* Result = nullptr;
	const UClass* NewComponentClass = ComponentClass.Get();
	if (!NewComponentClass)
		return nullptr;

	Result = NewObject<UActorComponent>((UObject*)Actor, NewComponentClass, Name);
	USceneComponent* AsSceneComponent = Cast<USceneComponent>(Result);
	if (AsSceneComponent)
	{
		AsSceneComponent->AttachToComponent(ParentComponent ? ParentComponent : Actor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		FTransform T;
		AsSceneComponent->SetComponentToWorld(T);
	}
	Actor->AddInstanceComponent(Result);
	Result->OnComponentCreated();
	Result->RegisterComponent();
	
	UpdateActor(Actor);
	return Result;
}

void UQuestFunctionLibrary::RemoveComponent(UActorComponent* ActorComponent, AActor* Actor)
{
	if (!Actor || !ActorComponent)
		return;

	Actor->RemoveInstanceComponent(ActorComponent);
	ActorComponent->DestroyComponent();

	UpdateActor(Actor);
}

void UQuestFunctionLibrary::ShowFormattedDialog(const FString& InFileName)
{
	// Very Fancy Dialoge Prompt !
	FText DialogText = FText::Format
	(
		LOCTEXT("PluginButtonDialogText", "{0}"),
		FText::FromString(InFileName)
	);

#if WITH_EDITOR
	FMessageDialog::Open(EAppMsgType::Ok, DialogText);
#endif
}

void UQuestFunctionLibrary::UpdateActor(AActor* Actor)
{
#if WITH_EDITOR
	Actor->RerunConstructionScripts();
	FEditorFileUtils::SaveCurrentLevel();
#endif

	SavePackage(Actor);
}

void UQuestFunctionLibrary::SavePackage(UObject* Object)
{
	if (!Object)
	{
		UE_LOG(LogTemp, Error, TEXT("Invalid object pointer"));
		return;
	}

	// Get the package associated with the object
	UPackage* Package = Object->GetPackage();

	if (!Package)
	{
		Package = Object->GetExternalPackage();
		if(!Package)
			return;
	}

	// Modify the package to mark it as dirty
	Package->Modify();
	Package->MarkPackageDirty();
}
