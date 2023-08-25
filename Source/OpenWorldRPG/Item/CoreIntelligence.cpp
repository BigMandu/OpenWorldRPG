// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/Item/CoreIntelligence.h"
#include "OpenWorldRPG/GameData/MyPlayerState.h"
#include "OpenWorldRPG/MainCharacter.h"
	
void ACoreIntelligence::Interaction(AActor* Actor)
{
	if ( AMainCharacter* Player = Cast<AMainCharacter>(Actor) )
	{
		AMyPlayerState* PlayerState = Cast<AMyPlayerState>(Player->GetPlayerState());
		if ( PlayerState )
		{			
			//PlayerState->GetIntel(Inteltype);

			//Destroy();
		}
	}
}