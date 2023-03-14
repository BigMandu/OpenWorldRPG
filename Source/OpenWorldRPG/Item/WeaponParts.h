// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "WeaponParts.generated.h"

/**
 * 
 */
UCLASS()
class OPENWORLDRPG_API AWeaponParts : public AEquipment
{
	GENERATED_BODY()
private:
	UNewItemObject* OwnerWeapon;
public:
	AWeaponParts();



	//UNewItemObject* GetOwnerWeapon();

	//void SetOwnerWeapon(UNewItemObject* OwnerWeapon);

	//void RemoveOwnerWeapon(UNewItemObject* OwnerWeapon);
	
};
