// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "OpenWorldRPG/CustomLibrary/CustomStructLibrary.h"
#include "CustomInventoryLibrary.generated.h"

/**
 * 
 */
class UEquipmentComponent;
class UNewItemObject;
class UItemStorageObject;
class UBasePDA;
class UCustomPDA;
class ABaseCharacter;
class AWeapon;
class AEquipment;
class AItem;


UCLASS()
class OPENWORLDRPG_API UCustomInventoryLibrary : public UObject
{
	GENERATED_BODY()

public:

	static UNewItemObject* CreateObject(FItemSetting ItemStruct, bool& bIsCreated);

	static void RemoveFromPreviousMotherContainer(UNewItemObject* Obj);

	/*ItemObj를 이용해 Item class를 Spawn한다.*/
	static AItem* SpawnItem(UWorld* World, UNewItemObject* ItemObj);

	/*ItemObj를 이용해 Equipment Class를 Spawn한다*/
	static AEquipment* SpawnEquipment(UWorld* World, UNewItemObject* ItemObj);
	
	/*ItemObj를 이용해 Equipment Class를 Spawn한다*/
	static class AGrenadeBase* SpawnGrenade(UWorld* World, UNewItemObject* ItemObj);


	/*PDA를 이용해 Item class를 Spawn한다.*/
	static AItem* SpawnItem(UWorld* World, UBasePDA* ItemDA);

	/*PDA를 이용해 Equipment Class를 Spawn한다 */
	static AEquipment* SpawnEquipment(UWorld* World, UCustomPDA* EquipDA);


	static TArray<UCustomPDA*> RandomSpawnUsingTableButCount(int32 SpawnCount, TArray<UCustomPDA*> SpawnTable);

	/*  */
	static void GenerateRandomCount(UNewItemObject* ItemObj);

	/* Weapon을 제외한 장착중인 모든 Equipment를 Hide한다.*/
	static void HideAllEquipment(UEquipmentComponent* EComp);

	/* 장착중인 모든 Equipment를 Show한다.*/
	static void ShowAllEquipment(UEquipmentComponent* EComp);

    /** Weapon형태의 Item이 추가되거나, 장착될때
     * 기존 Actor에서 생성되고 관리되던 WPM을 Obj에 넣어줘서 갱신해준다.
     * @To - Data를 이관 받을 WPM
     * @From  - Data를 To로 전달할 WPM
	 *
	 * From에서 3개의 Parts data를 To로 이동시키고,
	 * From의 OwnerWeapon을 To의 OwnerWeapon으로 변경한다.
     */    
    static void SetWeaponPartsManager(UNewItemObject* Obj, AWeapon* Weapon);

	static void CopyItem(UItemStorageObject& To, UItemStorageObject& From);


	/// 이하 사용하지 않는 함수들
	static void BackToItem(UNewItemObject* ItemObj);
	static void DirectInToInventory(UNewItemObject* ItemObj, ABaseCharacter* BChar);
};