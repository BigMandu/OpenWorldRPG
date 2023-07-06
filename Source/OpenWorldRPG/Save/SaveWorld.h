// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SaveWorld.generated.h"

/** 월드에 배치된 Spawn volume(Item spawn vol, Enemy spawn vol)
 * 
 */


UCLASS()
class OPENWORLDRPG_API USaveWorld : public USaveGame
{
	GENERATED_BODY()

public:
	
	UPROPERTY()
	//TArray<class ASpawnVolume*> SavedSpawnVolume;
	TMap<class ASpawnVolume*, int32> SavedSpawnVolStatusMap;
	
};
