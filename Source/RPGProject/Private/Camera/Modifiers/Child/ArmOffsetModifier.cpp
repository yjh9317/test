// Fill out your copyright notice in the Description page of Project Settings.


#include "Camera/Modifiers/Child/ArmOffsetModifier.h"

#include "Camera/RPG_ConfigurableCameraManager.h"

void UArmOffsetModifier::ProcessBoomOffsets_Implementation(float DeltaTime, FVector InSocketOffset, FVector InTargetOffset,
                                            FVector ViewLocation, FRotator ViewRotation, FVector& OutSocketOffset, FVector& OutTargetOffset)
{
	Super::ProcessBoomOffsets(DeltaTime,InSocketOffset,InTargetOffset,ViewLocation,ViewRotation,OutSocketOffset,OutTargetOffset);
	
	if(RPGCameraManager)
	{
		ARPG_ConfigurableCameraManager* ConfigurableCameraManager = Cast<ARPG_ConfigurableCameraManager>(RPGCameraManager);
		if(ConfigurableCameraManager)
		{
			ConfigurableCameraManager->GetCurrentArmOffsets(SocketOffset,TargetOffset);
			OutSocketOffset = SocketOffset;
			OutTargetOffset = TargetOffset;
		}
	}
}
