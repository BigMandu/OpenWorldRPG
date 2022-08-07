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
	//if (bRotated) //��ȣ�� �Ǹ鼭 �ٽ� �ø��Ǵ� ������ �߻���.
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
		//ȸ���� �ߴٸ� item size�� swap����.
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
	//Mothercontainer�� nullptr�� ������ ���� �����Ƿ� ������ ���� �ʴ´�.
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
	//�굵 ���� �ʿ� ����.
	MotherEquipSlot = Var_EquipSlot;
}

