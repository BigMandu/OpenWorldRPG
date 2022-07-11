// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/CustomInventoryLibrary.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/EquipmentSlot.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Container.h"
#include "OpenWorldRPG/BaseCharacter.h"


void UCustomInventoryLibrary::BackToItem(UNewItemObject* ItemObj)
{
	UE_LOG(LogTemp, Warning, TEXT("UCustomInventoryLibrary::BackToItem"));
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

void UCustomInventoryLibrary::DirectInToInventory(UNewItemObject* ItemObj, ABaseCharacter* BChar)
{
	UE_LOG(LogTemp, Warning, TEXT("UCustomInventoryLibrary::DirectInToInv"));
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
		//Mothercontainer에 있지 않고, 장착중이었던 장비라면
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

AEquipment* UCustomInventoryLibrary::SpawnEquipment(UWorld* World, UNewItemObject* ItemObj)
{
	UE_LOG(LogTemp, Warning, TEXT("UCustomInventoryLibrary::SpawnEquipment"));
	if (World && ItemObj)
	{
		AEquipment* Equipment = Cast<AEquipment>(World->SpawnActor<AActor>(ItemObj->GetItemClass()));
		if (Equipment)
		{
			ItemObj->bIsDestoryed = false;

			Equipment->ReInitialize(ItemObj);
			Equipment->SetItemState(EItemState::EIS_Pickup);
			if (Equipment->bHasStorage)
			{
				Equipment->EquipInventoryComp = ItemObj->GetItemInvComp();
			}

			return Equipment;
		}
	}
	return nullptr;
}