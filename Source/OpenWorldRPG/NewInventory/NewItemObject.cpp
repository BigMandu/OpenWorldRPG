// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"
#include "OpenWorldRPG/NewInventory/Widget/EquipmentSlot.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"
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
		if (bRotated)
		{
			//ItemRotateSize.X = ItemInfo.DataAsset->ItemSize.Y;
			//ItemRotateSize.Y = ItemInfo.DataAsset->ItemSize.X,
			ItemSize = FIntPoint(ItemInfo.DataAsset->ItemSize.Y, ItemInfo.DataAsset->ItemSize.X);
		}
		else
		{
			ItemSize = FIntPoint(ItemInfo.DataAsset->ItemSize.X, ItemInfo.DataAsset->ItemSize.Y);
		}
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
	if (ItemInfo.DataAsset->bCanRotate)
	{	
		//FIntPoint ItemSize = FIntPoint(ItemInfo.DataAsset->ItemSize.X, ItemInfo.DataAsset->ItemSize.Y);
		//ȸ���� �ߴٸ� item size�� swap����.
		/*int32 temp = ItemSize.X;
		ItemSize.X = ItemSize.Y;
		ItemSize.Y = temp;*/

		//ItemRotateSize.X = ItemInfo.DataAsset->ItemSize.Y; 
		//ItemRotateSize.Y = ItemInfo.DataAsset->ItemSize.X,
		
		

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

void UNewItemObject::RemoveLinkSlot()
{ 
	if (SettedSlot)
	{
		SettedSlot->SettedObj = nullptr;
		SettedSlot = nullptr;
	}	
}

const UItemStorageObject* UNewItemObject::GetMotherStorage()
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

const UEquipmentComponent* UNewItemObject::GetMotherEquipComp()
{
	if (MotherEquipComp != nullptr)
	{
		return MotherEquipComp;
	}
	return nullptr;
}
void UNewItemObject::SetMotherEquipComp(UEquipmentComponent* Var_EquipSlot)
{
	//�굵 ���� �ʿ� ����.
	MotherEquipComp = Var_EquipSlot;
}

void UNewItemObject::AddCount(int32 Cnt)
{
	ItemInfo.Count = ItemInfo.Count + Cnt;
	if(MotherStorage)
	{
		MotherStorage->OnInventoryAdd.Broadcast(nullptr);
	}
}

void UNewItemObject::RemoveCount(int32 Cnt)
{
	ItemInfo.Count -= Cnt;
	if(MotherStorage)
	{
		MotherStorage->OnInventoryAdd.Broadcast(nullptr);
	}
	
}


void UNewItemObject::UseItem(UWorld* World)
{
	UE_LOG(LogTemp,Warning,TEXT("UNewItemObj::UseItem"));
	//UWorld* World = GetMotherStorage()->GetWorld();
	UE_LOG(LogTemp, Warning, TEXT("UNewItemObj::Try SpawnItem"));
	AItem* SpawnItem = UCustomInventoryLibrary::SpawnItem(World,this);
	
	if (SpawnItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("UNewItemObj::Success SpawnItem"));
		ABaseCharacter* BChar = Cast<ABaseCharacter>(World->GetFirstPlayerController()->GetCharacter());
		SpawnItem->Use(BChar, this);
	}
}