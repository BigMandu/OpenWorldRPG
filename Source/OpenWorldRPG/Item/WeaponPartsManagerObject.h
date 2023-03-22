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
	UPROPERTY()
    class AWeapon* OwnerWeapon;

public:

	//WeaponPartsWidget과 Weapon class에서 Bind한다. -> 2개 함수와 bind
	FOnChangeParts OnChangeParts;

	
	
	UPROPERTY()
	class UNewItemObject* MuzzleParts;
	UPROPERTY()
	UNewItemObject* ScopeParts;
	UPROPERTY()
	UNewItemObject* TacticalParts;

	UPROPERTY()
	class AEquipment* A_MuzzleParts;
	UPROPERTY()
	AEquipment* A_ScopeParts;
	UPROPERTY()
	AEquipment* A_TacticalParts;

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

	/* Weapon이 InventoryGrid로 들어갈때(Destroy 될때) 같이 호출해 Parts들을 Destory한다. */
	void DestroyAllAttachParts(AWeapon* VarWeapon);


	void AddParts(UNewItemObject* Parts);
	void RemoveParts(UNewItemObject* Parts);

	AEquipment* GetWeaponParts(EWeaponPartsType PartsWantToGet);

private:
	/* Weapon이 Equip될 때(Spawn될때) 부착된 Parts들도 스폰한다.*/
	void SpawnAndAttachParts(UWorld* World, UNewItemObject* PartsObj, EWeaponPartsType VarPartsType);//AWeapon* VarWeapon);
	
	void DeleteLink(UNewItemObject* PartsObj);

	
};