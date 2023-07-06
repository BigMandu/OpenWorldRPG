// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "OpenWorldRPGGameModeBase.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class OPENWORLDRPG_API AOpenWorldRPGGameModeBase : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AOpenWorldRPGGameModeBase();
	virtual void StartPlay() override;

	UPROPERTY()
	class USaveWorld* SaveGame_World;

	//Falling Dmg type
	UPROPERTY()
	TSubclassOf<class UMyDamageType> FallingDmgType;
	 
	void SaveWorldStatus();
	void LoadWorldStatus();


	float ModifyApplyDamage(float BeforeDamage, struct FDamageEvent const& DamageEvent, class AActor* DamagedVictim, class AController* EventInstigator, AActor* DamageCauser);

	void ProcessKillEvent(AController* Killer, AController* KilledChar, struct FDamageEvent const& DamageEvent);
	void ProcessHitEvent(AController* DamageGiver, AController* DamageReceiver, struct FDamageEvent const& DamageEvent);
};
