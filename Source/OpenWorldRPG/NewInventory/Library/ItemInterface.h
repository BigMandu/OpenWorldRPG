// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ItemInterface.generated.h"




UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	EET_Backpack	UMETA(DisplayName = "Backpack"),
	EET_Vest		UMETA(DisplayName = "Vest"),
	
	EET_Rifle		UMETA(DisplayName = "Rifle"),
	EET_Pistol		UMETA(DisplayName = "Pistol"),
	EET_Helmet		UMETA(DisplayName = "Helmet"),
	EET_Plate		UMETA(Displayname = "Plate_Carrier"),
	
	EET_MAX	= 6		UMETA(Hidden)
};


UENUM(BlueprintType)
enum class EInteractType : uint8
{
	EIT_Gizmo		UMETA(DisplayName = "Gizmo"),
	EIT_Item		UMETA(DisplayName = "Item"),
	EIT_Equipment	UMETA(DisplayName = "Equipment"),
	EIT_LootBox		UMETA(DisplayName = "LootBox"),

	EIT_MAX			UMETA(DisplayName = "DefaultMAX")
};

UENUM()
enum class ERifleSlot :uint8
{
	ERS_Primary		UMETA(DisplayName = "Primary"),
	ERS_Sub			UMETA(DisplayName = "Sub"),

	ERS_MAX			UMETA(DisplayName = "defaultsMAX")
};


UENUM()
enum class EItemType : uint8
{
	EIT_NONE		UMETA(DisplayName = "NONE_Type"),
	EIT_Food		UMETA(DisplayName = "Food_Type"),
	EIT_Medical		UMETA(DisplayName = "Medical_Type"),
	EIT_Scrap		UMETA(DisplayName = "Scrap_Type"),
	EIT_Ammo		UMETA(DisplayName = "Ammo_Type"),

	EIT_MAX			UMETA(DisplayName = "DefaultMAX")
};

UENUM()
enum class EAmmoType : uint8
{
	EAT_556		UMETA(DisplayName = "5.56mm"),
	EAT_762		UMETA(DisplayName = "7.62mm"),
	EAT_9		UMETA(DisplayName = "9mm"),

	EAT_MAX		UMETA(DisplayName = "DefaultMAX")
};

UENUM()
enum class EQuickSlotType : uint8 
{
	EQST_Weapon		UMETA(DisplayName = "Weapon"),
	EQST_Item		UMETA(DisplayName = "Item"),

	EQST_MAX		UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EQuickSlotNumber : uint8 
{
	EQSN_N4		UMETA(DisplayName = "Num4"),
	EQSN_N5		UMETA(DisplayName = "Num5"),
	EQSN_N6		UMETA(DisplayName = "Num6"),
	EQSN_N7		UMETA(DisplayName = "Num7"),
	EQSN_N8		UMETA(DisplayName = "Num8"),
	EQSN_N9		UMETA(DisplayName = "Num9"),

	EQSN_MAX = 6	UMETA(Hidden)
};

//UENUM()
//enum class EEquipSlot : uint8 
//{
//	EES_Helmet		UMETA(DisplayName = "HelmetSlot"),
//	EES_Plate		UMETA(DisplayName = "PlateSlot"),
//	EES_Vest		UMETA(DisplayName = "VestSlot"),
//	EES_Backpack	UMETA(DisplayName = "BackpackSlot"),
//	EES_Pistol		UMETA(DisplayName = "PistolSlot"),
//	EES_Primary		UMETA(DisplayName = "PrimarySlot"),
//	EES_Sub			UMETA(DisplayName = "SubSlot"),
//
//	EES_MAX			UMETA(DisplayName = "DefaultMAX")
//};

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
