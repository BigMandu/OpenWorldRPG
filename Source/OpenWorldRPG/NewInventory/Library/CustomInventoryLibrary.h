// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CustomInventoryLibrary.generated.h"

/**
 * 
 */
class UNewItemObject;
class UBasePDA;
class UCustomPDA;
class ABaseCharacter;
class AEquipment;
class AItem;

UCLASS()
class OPENWORLDRPG_API UCustomInventoryLibrary : public UObject
{
	GENERATED_BODY()

public:

	static UNewItemObject* CreateObject(FItemSetting ItemStruct, bool& bIsCreated);

	/*ItemObj瑜??댁⑺?Item class瑜?Spawn???*/
	static AItem* SpawnItem(UWorld* World, UNewItemObject* ItemObj);

	/*ItemObj瑜??댁⑺?Equipment Class瑜?Spawn???/
	static AEquipment* SpawnEquipment(UWorld* World, UNewItemObject* ItemObj);
	
	/*ItemObj瑜??댁⑺?Equipment Class瑜?Spawn???/
	static ABaseGrenade* SpawnGrenade(UWorld* World, UNewItemObject* ItemObj);


	/*PDA瑜??댁⑺?Item class瑜?Spawn???*/
	static AItem* SpawnItem(UWorld* World, UBasePDA* ItemDA);

	/*PDA瑜??댁⑺?Equipment Class瑜?Spawn???*/
	static AEquipment* SpawnEquipment(UWorld* World, UCustomPDA* EquipDA);


	/*  */
	static void GenerateRandomCount(UNewItemObject* ItemObj);

	/* Weapon? ??명 ?μ갑以??紐⑤ Equipment瑜?Hide???*/
	static void HideAllEquipment(UEquipmentComponent* EComp);

	/* ?μ갑以??紐⑤ Equipment瑜?Show???*/
	static void ShowAllEquipment(UEquipmentComponent* EComp);

    /** Weapon??? Item??異媛?嫄곕, ?μ갑??
     * 湲곗〈 Actor?? ??깅怨 愿由щ? WPM? Obj? ?ｌ댁?? 媛깆?댁???
     * @Weapon - ?μ갑?嫄곕 Inventory? 異媛? Weapon Actor
     * @Obj - ?μ갑?嫄곕 Inventory? 異媛?? ??깅? Object
     */
    
    static void SetWeaponPartsManager(AWeapon* Weapon, UNewItemObject* Obj);


	/// ?댄 ?ъ⑺吏 ?? ?⑥??
	static void BackToItem(UNewItemObject* ItemObj);
	static void DirectInToInventory(UNewItemObject* ItemObj, ABaseCharacter* BChar);
};
