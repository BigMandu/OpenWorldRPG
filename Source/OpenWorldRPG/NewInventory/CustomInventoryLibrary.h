// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CustomInventoryLibrary.generated.h"

/**
 * 
 */
class UNewItemObject;
class ABaseCharacter;
class AEquipment;

UCLASS()
class OPENWORLDRPG_API UCustomInventoryLibrary : public UObject
{
	GENERATED_BODY()

public:

	static void BackToItem(UNewItemObject* ItemObj);
	static void DirectInToInventory(UNewItemObject* ItemObj, ABaseCharacter* BChar);
	static AEquipment* SpawnEquipment(UWorld* World, UNewItemObject* ItemObj);
};
