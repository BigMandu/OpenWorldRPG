// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/CustomInventoryLibrary.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/EquipmentSlot.h"

void UCustomInventoryLibrary::BackToItem(UNewItemObject* ItemObj)
{
	if (ItemObj)
	{
		//ItemObj가 원래 있던 MotherContainer를 가져와
		if (ItemObj->GetMotherContainer() != nullptr)
		{
			UNewInventoryGrid* GridInv = ItemObj->GetMotherContainer();
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
		else if (ItemObj->GetMotherEquipSlot() != nullptr)
		{
			ItemObj->GetMotherEquipSlot()->TrySlotEquip(ItemObj);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UNewInvGrid::OnDrop , Error!"));
		}
	}
}