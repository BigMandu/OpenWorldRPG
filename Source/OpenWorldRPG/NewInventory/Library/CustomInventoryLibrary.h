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

	/*ItemObj�� �̿��� Item class�� Spawn�Ѵ�.*/
	static AItem* SpawnItem(UWorld* World, UNewItemObject* ItemObj);

	/*ItemObj�� �̿��� Equipment Class�� Spawn�Ѵ�*/
	static AEquipment* SpawnEquipment(UWorld* World, UNewItemObject* ItemObj);
	
	/*ItemObj�� �̿��� Equipment Class�� Spawn�Ѵ�*/
	static ABaseGrenade* SpawnGrenade(UWorld* World, UNewItemObject* ItemObj);


	/*PDA�� �̿��� Item class�� Spawn�Ѵ�.*/
	static AItem* SpawnItem(UWorld* World, UBasePDA* ItemDA);

	/*PDA�� �̿��� Equipment Class�� Spawn�Ѵ� */
	static AEquipment* SpawnEquipment(UWorld* World, UCustomPDA* EquipDA);


	/*  */
	static void GenerateRandomCount(UNewItemObject* ItemObj);

	/* Weapon�� ������ �������� ��� Equipment�� Hide�Ѵ�.*/
	static void HideAllEquipment(UEquipmentComponent* EComp);

	/* �������� ��� Equipment�� Show�Ѵ�.*/
	static void ShowAllEquipment(UEquipmentComponent* EComp);



	/// ���� ������� �ʴ� �Լ���
	static void BackToItem(UNewItemObject* ItemObj);
	static void DirectInToInventory(UNewItemObject* ItemObj, ABaseCharacter* BChar);
};
