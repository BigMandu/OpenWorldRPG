// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/Item/Intel.h"
#include "OpenWorldRPG/GameData/MyPlayerState.h"

void AIntel::Interaction(class AActor* Actor)
{
	//Super::Interaction(Actor);

	if ( ItemSetting.DataAsset->IntelType == EIntelType::EIT_MAX || ItemSetting.DataAsset->IntelType == EIntelType::EIT_NONE )
	{
		return;
	}

	if ( AMainCharacter* Player = Cast<AMainCharacter>(Actor) )
	{
		if ( AMyPlayerState* PlayerState = Cast<AMyPlayerState>(Player->GetPlayerState()))
		{
			PlayerState->GetIntel(ItemSetting.DataAsset->IntelType, this);
		}
	}	
}