// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventoryGrid.h"
#include "OpenWorldRPG/Item/Item.h"

UNewItemObject::UNewItemObject()
{
	//ObjInvComp = CreateDefaultSubobject<UNewInventoryComponent>(TEXT("EquipInventoryComp"));
}

FIntPoint UNewItemObject::GetItemSize()
{
	//if (bRotated) //재호출 되면서 다시 플립되는 문제가 발생함.
	//{
	//	int32 temp = itemsize.X;
	//	itemsize.X = itemsize.Y;
	//	itemsize.Y = temp;
	//}
	FIntPoint ItemSize;
	if (ItemInfo.DataAsset != nullptr)
	{
		ItemSize = FIntPoint(ItemInfo.DataAsset->ItemSize.X, ItemInfo.DataAsset->ItemSize.Y);
		return ItemSize;
	}
	return ItemSize;
}

UClass* UNewItemObject::GetItemClass()
{
	return item->GetClass();
}

void UNewItemObject::ItemRotate()
{
	//UE_LOG(LogTemp, Warning, TEXT("NewItemObj::ItemRotate func called"));
	if (bCanRotated)
	{	
		FIntPoint ItemSize = FIntPoint(ItemInfo.DataAsset->ItemSize.X, ItemInfo.DataAsset->ItemSize.Y);
		//회전을 했다면 item size를 swap해줌.
		int32 temp = ItemSize.X;
		ItemSize.X = ItemSize.Y;
		ItemSize.Y = temp;
		

		/* flip flop*/
		bRotated = !bRotated;
	}
}

UMaterialInterface* UNewItemObject::GetItemIcon()
{
	if (ItemInfo.DataAsset == nullptr) return nullptr;

	UMaterialInterface* ReturnIcon = nullptr;
	if (bRotated)
	{
		if (ItemInfo.DataAsset->IconRotated)
		{
			ReturnIcon = ItemInfo.DataAsset->IconRotated;
		}
	}
	else
	{
		if (ItemInfo.DataAsset->Icon)
		{
			ReturnIcon = ItemInfo.DataAsset->Icon;
		}
	}
	return ReturnIcon;
}

UItemStorageObject* UNewItemObject::GetMotherStorage()
{
	if (MotherStorage != nullptr)
	{
		return MotherStorage;
	}
	return nullptr;
}
void UNewItemObject::SetMotherStorage(UItemStorageObject* Var_MotherStorage)
{
	//Mothercontainer를 nullptr로 지정할 때가 있으므로 검증은 하지 않는다.
	//if (Var_InvGrid != nullptr) 
	{
		MotherStorage = Var_MotherStorage;
	}
}

UEquipmentSlot* UNewItemObject::GetMotherEquipSlot()
{
	if (MotherEquipSlot != nullptr)
	{
		return MotherEquipSlot;
	}
	return nullptr;
}
void UNewItemObject::SetMotherEquipSlot(UEquipmentSlot* Var_EquipSlot)
{
	//얘도 검증 필요 없음.
	MotherEquipSlot = Var_EquipSlot;
}

