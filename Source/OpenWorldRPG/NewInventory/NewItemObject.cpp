// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/Item/Item.h"

UNewItemObject::UNewItemObject()
{

}

FIntPoint UNewItemObject::GetItemSize()
{
	return itemsize;
}

UClass* UNewItemObject::GetItemClass()
{
	return item->GetClass();
}