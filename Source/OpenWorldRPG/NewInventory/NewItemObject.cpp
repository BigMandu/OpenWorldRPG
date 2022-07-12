// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"
#include "OpenWorldRPG/Item/Item.h"

UNewItemObject::UNewItemObject()
{
	
}

FIntPoint UNewItemObject::GetItemSize()
{
	//if (bRotated) //재호출 되면서 다시 플립되는 문제가 발생함.
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
		//회전을 했다면 item size를 swap해줌.
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
	//Mothercontainer를 nullptr로 지정할 때가 있으므로 검증은 하지 않는다.
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
	//얘도 검증 필요 없음.
	MotherEquipSlot = Var_EquipSlot;
}

