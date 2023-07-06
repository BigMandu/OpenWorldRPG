// Fill out your copyright notice in the Description page of Project Settings.


#include "Cheat_Level.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/GameData/StatManagementComponent.h"

void ACheat_Level::Interaction(AActor* Actor)
{
	if ( AMainCharacter* Player = Cast<AMainCharacter>(Actor) )
	{
		UE_LOG(LogTemp,Warning,TEXT("ACheat_Level::Interaction // Try Level up"));
		int32 CurEXP = Player->StatManagementComponent->CurrentStat.ExperiencePoint;
		int32 CurMAXEXP = Player->StatManagementComponent->CurrentMAXStats->NextExp;

		int32 RemainEXP = CurMAXEXP - CurEXP;

		Player->StatManagementComponent->AddExpPoint(RemainEXP);
	}
}