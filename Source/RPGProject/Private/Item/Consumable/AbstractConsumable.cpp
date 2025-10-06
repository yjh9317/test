// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Consumable/AbstractConsumable.h"

AAbstractConsumable::AAbstractConsumable()
{
	PrimaryActorTick.bCanEverTick = false;

}

void AAbstractConsumable::BeginPlay()
{
	Super::BeginPlay();
	
}


