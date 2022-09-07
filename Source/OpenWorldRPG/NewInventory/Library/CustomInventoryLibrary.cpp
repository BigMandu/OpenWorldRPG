// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"
#include "OpenWorldRPG/NewInventory/Library/InventoryStruct.h"

#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"

#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/Widget/EquipmentSlot.h"

#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/Item/Weapon_Instant.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Container.h"
#include "OpenWorldRPG/BaseCharacter.h"

UNewItemObject* UCustomInventoryLibrary::CreateObject(FItemSetting ItemStruct, bool& bIsCreated)
{
	if (ItemStruct.DataAsset)
	{
		UNewItemObject* ReturnObj = nullptr;
		if (ItemStruct.DataAsset->bHasStorage)
		{
			ReturnObj = NewObject<UItemStorageObject>();
		}
		else
		{
			ReturnObj = NewObject<UNewItemObject>();
		}
		//ReturnObj->ItemInfo = FItemSetting(ItemStruct.DataAsset, 1, 0);
		ReturnObj->ItemInfo = ItemStruct;

		bIsCreated = true;
		return ReturnObj;
	}
	else
	{
		bIsCreated = false;
		return nullptr;
	}

}

void UCustomInventoryLibrary::BackToItem(UNewItemObject* ItemObj)
{
	UE_LOG(LogTemp, Warning, TEXT("UCustomInventoryLibrary::BackToItem"));
	if (ItemObj)
	{
		//ItemObj가 원래 있던 MotherContainer를 가져와
		if (ItemObj->GetMotherStorage() != nullptr)
		{
			//UNewInventoryGrid* GridInv = ItemObj->GetMotherContainer();
			UNewInventoryGrid* GridInv = nullptr;// = ItemObj->GetMotherContainer();
			//원래 있던 Mothercontainer에 넣는다.
			if (GridInv->GetInventoryComp() != nullptr)
			{
				if (GridInv->GetInventoryComp()->IsAvailableSpace(ItemObj, ItemObj->TopLeftIndex))
				{
					GridInv->GetInventoryComp()->AddItemAtIndex(ItemObj, ItemObj->TopLeftIndex);
				}
				else
				{
					GridInv->GetInventoryComp()->TryAddItem(ItemObj);
				}
			}
		}
		//Mothercontainer에 있지 않고, 장착중이었던 장비라면
		/*else if (ItemObj->GetMotherEquipSlot() != nullptr)
		{
			ItemObj->GetMotherEquipSlot()->TrySlotEquip(ItemObj);
		}*/
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UNewInvGrid::OnDrop , Error!"));
		}
	}
}

void UCustomInventoryLibrary::DirectInToInventory(UNewItemObject* ItemObj, ABaseCharacter* BChar)
{
	UE_LOG(LogTemp, Warning, TEXT("UCustomInventoryLibrary::DirectInToInv"));
	if (ItemObj)
	{
		//if (ItemObj->GetMotherContainer() != nullptr)
		{
			UNewInventoryGrid* GridInv = nullptr;// = ItemObj->GetMotherContainer();
			if (GridInv->GetInventoryComp() != nullptr)
			{
				if (GridInv->GetInventoryComp()->IsAvailableSpace(ItemObj, ItemObj->TopLeftIndex))
				{
					GridInv->GetInventoryComp()->AddItemAtIndex(ItemObj, ItemObj->TopLeftIndex);
				}
				else
				{
					GridInv->GetInventoryComp()->TryAddItem(ItemObj);
				}
			}
		}
		//Mothercontainer에 있지 않고, 장착중이었던 장비라면
		//else if (ItemObj->GetMotherEquipSlot() != nullptr)
		{
			if (BChar)
			{
				if (BChar->InteractLootBox)
				{
					BChar->InteractLootBox->ContainerInventoryComp->TryAddItem(ItemObj);
				}
			}

		}
	}
}


AEquipment* UCustomInventoryLibrary::SpawnEquipment(UWorld* World, UNewItemObject* ItemObj)//, AActor* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("UCustomInventoryLibrary::SpawnEquipment"));
	if (World && ItemObj)
	{
		AEquipment* Equipment = nullptr;		

		//Weapon인 경우 WeaponInstant로 생성해준다.

		UWeaponPDA* WeaponPDA = Cast<UWeaponPDA>(ItemObj->ItemInfo.DataAsset);
		if (WeaponPDA)
		{
			Equipment = Cast<AWeapon_Instant>(World->SpawnActor<AActor>(AWeapon_Instant::StaticClass()));
		}
		else
		{
			Equipment = Cast<AEquipment>(World->SpawnActor<AActor>(AEquipment::StaticClass()));// ItemObj->GetItemClass()));
		}

		
		if (Equipment)
		{
			ItemObj->bIsDestoryed = false;
			Equipment->ItemSetting = ItemObj->ItemInfo;
			Equipment->SetMesh();
			//Storage가 있으면 Inventory를 넘겨준다.
			if (ItemObj->ItemInfo.DataAsset->bHasStorage)
			{
				UItemStorageObject* StorageObj = Cast<UItemStorageObject>(ItemObj);
				if (StorageObj)
				{
					Equipment->ItemSetting.Inventory = StorageObj->Inventory;
				}
			}
			Equipment->SetItemState(EItemState::EIS_Pickup);
			return Equipment;
		}
	}
	return nullptr;
}