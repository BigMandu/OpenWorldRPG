// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/CustomInventoryLibrary.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/EquipmentSlot.h"
#include "OpenWorldRPG/Item/Container.h"
#include "OpenWorldRPG/BaseCharacter.h"


void UCustomInventoryLibrary::BackToItem(UNewItemObject* ItemObj)
{
	if (ItemObj)
	{
		//ItemObj�� ���� �ִ� MotherContainer�� ������
		if (ItemObj->GetMotherContainer() != nullptr)
		{
			UNewInventoryGrid* GridInv = ItemObj->GetMotherContainer();
			//���� �ִ� Mothercontainer�� �ִ´�.
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
		//Mothercontainer�� ���� �ʰ�, �������̾��� �����
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

void UCustomInventoryLibrary::DirectInToInventory(UNewItemObject* ItemObj, ABaseCharacter* BChar)
{
	if (ItemObj)
	{
		if (ItemObj->GetMotherContainer() != nullptr)
		{
			UNewInventoryGrid* GridInv = ItemObj->GetMotherContainer();
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
		//Mothercontainer�� ���� �ʰ�, �������̾��� �����
		else if (ItemObj->GetMotherEquipSlot() != nullptr)
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