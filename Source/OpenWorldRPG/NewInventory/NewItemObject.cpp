// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/Item/Item.h"

UNewItemObject::UNewItemObject()
{

}

FIntPoint UNewItemObject::GetItemSize()
{
	UE_LOG(LogTemp, Warning, TEXT("NewItemObj::GetItemSize func called"));
	if (bRotated)
	{
		int32 temp = itemsize.X;
		itemsize.X = itemsize.Y;
		itemsize.Y = temp;
	}
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
		/* flip flop*/
		bRotated = !bRotated;
	}
}

UMaterialInterface* UNewItemObject::GetItemIcon()
{
	UE_LOG(LogTemp, Warning, TEXT("NewItemObj::GetItemIcon func called"));
	if (bRotated)
	{
		return iconRotated;
	}
	return icon;
}