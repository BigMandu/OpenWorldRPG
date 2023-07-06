// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "MyPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API AMyPlayerState : public APlayerState
{
	GENERATED_BODY()
private:
	int32 HeadShotCount;
	int32 KillCount;

	int32 DeathCount;

	float HeadshotRate;

	AMainCharacter* Player;
public:
	AMyPlayerState(const FObjectInitializer& ObjectInitializer);

	void AddHeadShotCount();

	int32 GetHeadShotCount()
	{
		return HeadShotCount;
	}

	void AddKillCount();

	int32 GetKillCount()
	{
		return KillCount;
	}

	float GetHeadshotRate()
	{
		return HeadShotCount/KillCount;
	}

	void AddDeathCount()
	{
		++DeathCount;
	}

	int32 GetDeathCount()
	{
		return DeathCount;
	}

};
