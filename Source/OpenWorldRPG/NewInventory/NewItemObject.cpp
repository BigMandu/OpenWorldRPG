// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"
#include "OpenWorldRPG/Item/Item.h"

UNewItemObject::UNewItemObject()
{
	
}

FIntPoint UNewItemObject::GetItemSize()
{
	//if (bRotated) //��ȣ�� �Ǹ鼭 �ٽ� �ø��Ǵ� ������ �߻���.
	//{
	//	int32 temp = itemsize.X;
	//	itemsize.X = itemsize.Y;
	//	itemsize.Y = temp;
	//}
	return itemsize;
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
		//ȸ���� �ߴٸ� item size�� swap����.
		int32 temp = itemsize.X;
		itemsize.X = itemsize.Y;
		itemsize.Y = temp;
		

		/* flip flop*/
		bRotated = !bRotated;
	}
}

UMaterialInterface* UNewItemObject::GetItemIcon()
{
	UMaterialInterface* ReturnIcon = nullptr;
	if (bRotated)
	{
		if (iconRotated)
		{
			ReturnIcon = iconRotated;
		}
	}
	else
	{
		if (icon)
		{
			ReturnIcon = icon;
		}
	}
	return ReturnIcon;
}

UNewInventoryComponent* UNewItemObject::GetItemInvComp()
{
	if (InvComp)
	{
		return InvComp;
	}
	return nullptr;
}

void UNewItemObject::SetItemInvComp(UNewInventoryComponent* Var_InvCmp)
{
	if (Var_InvCmp != nullptr)
	{
		InvComp = Var_InvCmp;
	}
}

UNewInventoryGrid* UNewItemObject::GetMotherContainer()
{
	if (MotherContainer != nullptr)
	{
		return MotherContainer;
	}
	return nullptr;
}
void UNewItemObject::SetMotherContainer(UNewInventoryGrid* Var_InvGrid)
{
	//Mothercontainer�� nullptr�� ������ ���� �����Ƿ� ������ ���� �ʴ´�.
	//if (Var_InvGrid != nullptr) 
	{
		MotherContainer = Var_InvGrid;
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

