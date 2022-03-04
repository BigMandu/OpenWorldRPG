// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/NewItemObject.h"
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
	UE_LOG(LogTemp, Warning, TEXT("NewItemObj::ItemRotate func called"));
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