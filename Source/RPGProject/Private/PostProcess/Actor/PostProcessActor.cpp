#include "PostProcess/Actor/PostProcessActor.h"

#include "Components/PostProcessComponent.h"
#include "Kismet/KismetMaterialLibrary.h"

APostProcessActor::APostProcessActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	PostProcessComponent = CreateDefaultSubobject<UPostProcessComponent>(TEXT("PostProcessComponent"));
	PostProcessComponent->SetupAttachment(GetRootComponent());
}


void APostProcessActor::BeginPlay()
{
	Super::BeginPlay();
	PostProcessComponent->Settings = PostProcessSettings;
}


UMaterialInstanceDynamic* APostProcessActor::CreateMaterialInstance(UMaterial* Material)
{
	if(nullptr == Material) return nullptr;
	AllMaterials.Add(Material);
	return UKismetMaterialLibrary::CreateDynamicMaterialInstance(GetWorld(),Material);
}