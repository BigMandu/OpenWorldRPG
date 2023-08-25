// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CustomEnumLibrary.generated.h"


////////////////////////////////////////////////////////////////
/*                          Item Enums                        */
////////////////////////////////////////////////////////////////
UENUM(BlueprintType)
enum class EEquipmentType : uint8
{
	EET_Backpack	UMETA(DisplayName = "Backpack"),
	EET_Vest		UMETA(DisplayName = "Vest"),
	EET_Rifle		UMETA(DisplayName = "Rifle"),
	EET_Pistol		UMETA(DisplayName = "Pistol"),
	EET_Helmet		UMETA(DisplayName = "Helmet"),
	EET_Plate		UMETA(Displayname = "Plate_Carrier"),
	EET_WeaponParts	UMETA(Displayname = "WeaponParts"),
	
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


//this is for WeaponPartsClass, Adjust attach location along this name.
UENUM()
enum class EWeaponName : uint8
{
	EWN_M4	UMETA(DisplayName = "M4"),
	EWN_AK	UMETA(DisplayName = "AK"),
	EWN_M9	UMETA(DisplayName = "M9"),

	EWN_MAX	UMETA(DisplayName = "DefaultsMAX")
};

UENUM()
enum class ERifleSlot :uint8
{
	ERS_Primary		UMETA(DisplayName = "Primary"),
	ERS_Sub			UMETA(DisplayName = "Sub"),
	ERS_Pistol		UMETA(DisplayName = "Pistol"),

	ERS_MAX			UMETA(DisplayName = "defaultsMAX")
};


UENUM()
enum class EItemType : uint8
{
	EIT_NONE		UMETA(DisplayName = "NONE_Type"),
	EIT_Usable		UMETA(DisplayName = "Usable_Type"),
	EIT_Food		UMETA(DisplayName = "Food_Type"),
	EIT_Medical		UMETA(DisplayName = "Medical_Type"),
	EIT_Scrap		UMETA(DisplayName = "Scrap_Type"),
	EIT_Ammo		UMETA(DisplayName = "Ammo_Type"),
	EIT_Grenade		UMETA(DisplayName = "Grenade_Type"),

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
enum class EGrenadeType : uint8
{
	EGT_Fragment	UMETA(DisplayName = "Fragmentation"),
	EGT_Smoke		UMETA(DisplayName = "Smoke"),

	EGT_MAX			UMETA(DisplayName = "DefaultMAX")
};


UENUM(BlueprintType)
enum class EWeaponPartsType : uint8
{
	EWPT_Muzzle		UMETA(DisplayName = "Muzzle"),
	EWPT_Tactical	UMETA(DisplayName = "Tactical"),
	EWPT_Scope		UMETA(DisplayName = "Scope"),

	EWPT_MAX = 3	UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ETacticalPartsType : uint8
{
	ETPT_Laser			UMETA(DisplayName = "Laser"),
	ETPT_Flashlight		UMETA(DisplayName = "Flashlight"),

	ETPT_MAX			UMETA(DisplayName = "DefaultMAX")
};


UENUM(BlueprintType)
enum class EWeaponPartsLimitationType : uint8
{
	EWPLT_RifleONLY		UMETA(DisplayName = "Rifle_ONLY"),
	EWPT_PistolONLY		UMETA(DisplayName = "Pistol_ONLY"),
	EWPT_UnLimited		UMETA(DisplayName = "Unlimited"),

	EWPT_MAX			UMETA(DisplayName = "DefaultsMAX")
};


//for Enum iterator
ENUM_RANGE_BY_COUNT(EEquipmentType, EEquipmentType::EET_MAX)
ENUM_RANGE_BY_COUNT(EWeaponPartsType, EWeaponPartsType::EWPT_MAX)





////////////////////////////////////////////////////////////////
/*                        System Enums                        */
////////////////////////////////////////////////////////////////


/* Sound Enums */
UENUM()
enum class EPlayReloadSound: uint8
{
	EPRS_EjectSound,
	EPRS_InsertSound,
	EPRS_BoltReleaseSound,

	EPRS_MAX
};


//UENUM()
//enum class EMissionStatus : uint8
//{
//	EMS_NotStarted,
//	EMS_InProgress,
//	EMS_Completed
//};


////////////////////////////////////////////////////////////////
/*                          UI Enums                          */
////////////////////////////////////////////////////////////////

UENUM(BlueprintType)
enum class ESystemNotificationMessageType : uint8
{
	ESNMT_SaveFailed			UMETA(DisplayName = "Save Failed"),
	ESNMT_ReloadFailed			UMETA(DisplayName = "Reload Failed"),
	ESNMT_GetInCarFailed		UMETA(DisplayName = "GetInCarFailed Failed"),
	ESNMT_WeaponIsBusy			UMETA(DisplayName = "WeaponBusy"),


	ESNMT_MAX					UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EGameProgressNotificationMessageType : uint8
{
	EGPNMT_MissionClear			UMETA(DisplayName = "Mission Clear"),
	EGPNMT_Healing				UMETA(DisplayName = "Healing"),
	EGPNMT_Levelup				UMETA(DisplayName = "Levelup"),

	EGPNMT_MAX					UMETA(DisplayName = "DefaultMAX")
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

////////////////////////////////////////////////////////////////
/*                   Shooting Target Enums                    */
////////////////////////////////////////////////////////////////

UENUM(BlueprintType)
enum class EMissionType : uint8
{
	EMT_Normal		UMETA(DisplayName = "Normal Mission"),
	EMT_Choice		UMETA(DisplayName = "Choice Mission"),

	EMT_MAX			UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EMissionActionType: uint8
{
	EMAT_NONE		UMETA(DisplayName = "NONE"),
	EMAT_Collect	UMETA(DisplayName = "Collect"),
	EMAT_Reach		UMETA(DisplayName = "Reach"),
	EMAT_Kill		UMETA(DisplayName = "Kill"),
	EMAT_Interact	UMETA(DisplayName = "Interact"),

	EMAT_MAX		UMETA(DisplayName = "DefaultMAX")
};


UENUM(BlueprintType)
enum class EIntelType : uint8
{
	EIT_NONE	UMETA(DisplayName = "NONE"),
	EIT_Intel1	UMETA(DisplayName = "BNS_ONP project"),
	EIT_Intel2	UMETA(DisplayName = "BNS_ONP instructions"),
	EIT_Intel3	UMETA(DisplayName = "BNS_EyesOnly1"),
	EIT_Intel4	UMETA(DisplayName = "BNS_Evacuation, Eyesonly2"),
	EIT_Intel5	UMETA(DisplayName = "IIA_Operation"),

	EIT_MAX		UMETA(DisplayName = "DefaultMAX")
};

////////////////////////////////////////////////////////////////
/*                   Shooting Target Enums                    */
////////////////////////////////////////////////////////////////

UENUM(BlueprintType)
enum class ETargetMeshType: uint8
{
	ETMT_Head	UMETA(DisplayName = "Head"),
	ETMT_Body	UMETA(DisplayName = "Body"),

	ETMT_MAX	UMETA(DisplayName = "DefaultMAX")
};



UINTERFACE(MinimalAPI)
class UCustomEnumLibrary: public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class OPENWORLDRPG_API ICustomEnumLibrary
{
	GENERATED_BODY()
public:
	bool bCanDrop = false;
	//virtual bool CanDrop() = 0;

	//virtual bool SpawnEquip(class UNewItemObject* ItemObj);
};
