// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInterface.generated.h"

// This class does not need to be modified.

UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	EET_Backpack	UMETA(DisplayName = "Backpack"),
	EET_Vest		UMETA(DisplayName = "Vest"),
	
	EET_Rifle		UMETA(DisplayName = "Rifle"),
	EET_Pistol		UMETA(DisplayName = "Pistol"),
	EET_Helmet		UMETA(DisplayName = "Helmet"),
	EET_Plate		UMETA(Displayname = "Plate Carrier"),
	
	EET_MAX	= 6		UMETA(Hidden)
};


UENUM(BlueprintType)
enum class EInteractType : uint8
{
	EIT_Gizmo		UMETA(DisplayName = "EIT_Gizmo"),
	EIT_Item		UMETA(DisplayName = "EIT_Item"),
	EIT_Equipment	UMETA(DisplayName = "EIT_Equipment"),
	EIT_LootBox		UMETA(DisplayName = "EIT_LootBox"),

	EIT_MAX			UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class ERifleSlot :uint8
{
	ERS_Primary		UMETA(DisplayName = "Primary"),
	ERS_Sub			UMETA(DisplayName = "Sub"),

	ERS_MAX			UMETA(DisplayName = "defaultsMAX")
};

UINTERFACE(MinimalAPI)
class UItemInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OPENWORLDRPG_API IItemInterface
{
	GENERATED_BODY()
public:
	bool bCanDrop = false;
	//virtual bool CanDrop() = 0;

	//virtual bool SpawnEquip(class UNewItemObject* ItemObj);
};
