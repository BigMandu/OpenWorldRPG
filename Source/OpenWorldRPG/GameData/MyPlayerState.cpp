// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/GameData/MyPlayerState.h"

AMyPlayerState::AMyPlayerState(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	AMainCharacter* TPlayer = Cast<AMainCharacter>(GetOwner());
	Player = TPlayer ? TPlayer : nullptr;
	
}


void AMyPlayerState::AddHeadShotCount()
{
	++HeadShotCount;

	if (Player)
	{
		if ( Player->StatManagementComponent )
		{
			Player->StatManagementComponent->AddExpPoint(30);
		}
	}
}

void AMyPlayerState::AddKillCount()
{
	++KillCount;

	if (Player)
	{
		if ( Player->StatManagementComponent )
		{
			Player->StatManagementComponent->AddExpPoint(15);
		}
	}
}