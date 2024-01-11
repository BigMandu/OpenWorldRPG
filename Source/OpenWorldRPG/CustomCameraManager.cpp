// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomCameraManager.h"

ACustomCameraManager::ACustomCameraManager(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
	ViewPitchMax = 77.9f;
	ViewPitchMin = -74.9f;
}

void ACustomCameraManager::UpdateCamera(float DeltaTime)
{
	Super::UpdateCamera(DeltaTime);

	AMainCharacter* Player = PCOwner ? Cast<AMainCharacter>(PCOwner->GetPawn()) : nullptr;
	if (Player)
	{
		
	}
} 