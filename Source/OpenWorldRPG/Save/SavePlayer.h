// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "OpenWorldRPG/GameData/StatManagementComponent.h"
#include "OpenWorldRPG/CustomLibrary/CustomEnumLibrary.h"
#include "SavePlayer.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API USavePlayer : public USaveGame
{
	GENERATED_BODY()

	//
	//player의 status (level, exp, item( secure box, pocket), equipment, equipment's storage) - 완료
	// player가 있는 car (vehicle에서만 save/load가 가능함)
	// Level, car의 위치.
	//intel 습득 status.(아직 추가 안함)
public:
	UPROPERTY()
	int32 PlayerLevel;

	UPROPERTY()
	struct FCharacterCurrentStat PlayerStat;

	UPROPERTY()
	struct FIntelAcquiredInfo IntelAcq;
	/*UPROPERTY(VisibleAnywhere, Category = SaveGame)
	TArray<class UNewItemObject*> PlayerEquipment;

	UPROPERTY(VisibleAnywhere, Category = SaveGame)
	TArray<UNewItemObject*> SavedPocketItems;
	UPROPERTY(VisibleAnywhere, Category = SaveGame)
	TArray<UNewItemObject*> SavedSecureItems;
	*/

	

	UPROPERTY(VisibleAnywhere, Category = SaveGame)
	TArray<struct FSaveItem> PlayerEquipment;

	UPROPERTY(VisibleAnywhere, Category = SaveGame)
	TArray<FSaveItem> SavedPocketItems;
	UPROPERTY(VisibleAnywhere, Category = SaveGame)
	TArray<FSaveItem> SavedSecureItems;

	//virtual void Serialize(FArchive& Ar) override
	//{
	//	Super::Serialize(Ar);
	//	Ar << PlayerStat;
	//}


};
