// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/Item/WeaponParts.h"

AWeaponParts::AWeaponParts()
{
	ItemState = EItemState::EIS_Spawn;
	OwnerWeapon = nullptr;
}