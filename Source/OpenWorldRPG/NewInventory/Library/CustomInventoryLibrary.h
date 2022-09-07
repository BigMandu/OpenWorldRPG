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

	static UNewItemObject* CreateObject(FItemSetting ItemStruct, bool& bIsCreated);

	static void BackToItem(UNewItemObject* ItemObj);
	static void DirectInToInventory(UNewItemObject* ItemObj, ABaseCharacter* BChar);



	//if Actor var is valid, Call StepEquip func. else return AEquipment
	static AEquipment* SpawnEquipment(UWorld* World, UNewItemObject* ItemObj);// , AActor* Actor = nullptr);
};
