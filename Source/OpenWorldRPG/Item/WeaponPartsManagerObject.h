// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "WeaponPartsManagerObject.generated.h"

/**
 * 
 */

 DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnChangeParts);
 

UCLASS()
class OPENWORLDRPG_API UWeaponPartsManagerObject : public UObject
{
	GENERATED_BODY()

private:
    TWeakObjectPtr<class AWeapon> OwnerWeapon;

public:

	//WeaponPartsWidget怨?Weapon class?? Bind??? -> 2媛 ?⑥? bind
	FOnChangeParts OnChangeParts;

	
	
	
	TWeakObjectPtr<class UNewItemObject> MuzzleParts;
	TWeakObjectPtr<UNewItemObject> ScopeParts;
	TWeakObjectPtr<UNewItemObject> TacticalParts;

	TWeakObjectPtr<class AEquipment> A_MuzzleParts;
	TWeakObjectPtr<AEquipment> A_ScopeParts;
	TWeakObjectPtr<AEquipment> A_TacticalParts;

private:
	const FName MuzzleSocketName = TEXT("MuzzleParts");
	const FName ScopeSocketName = TEXT("ScopeParts");
	const FName TacticalSocketName = TEXT("TacticalParts");

public:

    void SetOwnerWeapon(AWeapon* Weapon);
    AWeapon* GetOwnerWeapon();

	/*void SetParts(UNewItemObject* SetPartsObj);
	void RemoveParts(UNewItemObject* RemovePartsObj);*/

	void UpdateParts(UWorld* World, AWeapon* VarWeapon);

	/* Weapon??InventoryGrid濡 ?ㅼ닿??(Destroy ??) 媛???몄???Parts?ㅼ Destory??? */
	void DestroyAllAttachParts(AWeapon* VarWeapon);


	void AddParts(UNewItemObject* Parts);
	void RemoveParts(UNewItemObject* Parts);

	AEquipment* GetWeaponParts(EWeaponPartsType PartsWantToGet);

private:
	/* Weapon??Equip? ?(Spawn??) 遺李⑸ Parts?ㅻ ?ㅽ고??*/
	void SpawnAndAttachParts(UWorld* World, UNewItemObject* PartsObj, EWeaponPartsType VarPartsType);//AWeapon* VarWeapon);
	
	void DeleteLink(UNewItemObject* PartsObj);

	
};
