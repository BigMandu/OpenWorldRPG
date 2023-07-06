// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"
#include "OpenWorldRPG/NewInventory/Widget/EquipmentSlot.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"

#include "OpenWorldRPG/Item/Item.h"
#include "OpenWorldRPG/Item/CoreUsableItem.h"


UNewItemObject::UNewItemObject()
{
	//ObjInvComp = CreateDefaultSubobject<UNewInventoryComponent>(TEXT("EquipInventoryComp"));
}

//void UNewItemObject::Serialize(FArchive& Ar)
//{
//	Super::Serialize(Ar);
//
//	Ar << ItemInfo;
//	Ar << TopLeftIndex;
//	Ar << RifleAssign;
//	Ar << WeaponPartsManager;
//	Ar << AmmoLeftInMag;
//}

FIntPoint UNewItemObject::GetTempItemSize()
{
	FIntPoint ItemSize;
	if (ItemInfo.DataAsset != nullptr)
	{
		if (bTempRotate)
		{
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

void UNewItemObject::SetItemRotate()
{
	bRotated = bTempRotate;
}

void UNewItemObject::ItemRotate()
{
	//UE_LOG(LogTemp, Warning, TEXT("NewItemObj::ItemRotate func called"));
	if (ItemInfo.DataAsset->bCanRotate)
	{	
		//FIntPoint ItemSize = FIntPoint(ItemInfo.DataAsset->ItemSize.X, ItemInfo.DataAsset->ItemSize.Y);
		//회전을 했다면 item size를 swap해줌.
		/*int32 temp = ItemSize.X;
		ItemSize.X = ItemSize.Y;
		ItemSize.Y = temp;*/

		//ItemRotateSize.X = ItemInfo.DataAsset->ItemSize.Y; 
		//ItemRotateSize.Y = ItemInfo.DataAsset->ItemSize.X,
		
		/* flip flop*/
		bTempRotate = !bTempRotate;
	}
}

UMaterialInterface* UNewItemObject::GetTempItemIcon()
{
	if (ItemInfo.DataAsset == nullptr) return nullptr;
	UMaterialInterface* ReturnIcon = nullptr;

	if (bTempRotate)
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
	//Mothercontainer를 nullptr로 지정할 때가 있으므로 검증은 하지 않는다.
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
	//얘도 검증 필요 없음.
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
	
	AItem* SpawnItem = nullptr;

	if (this->ItemInfo.DataAsset->ItemType == EItemType::EIT_Grenade)
	{
		SpawnItem = UCustomInventoryLibrary::SpawnGrenade(World,this);
	}
	else
	{
		SpawnItem = UCustomInventoryLibrary::SpawnItem(World,this);
	}
	
	if (SpawnItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("UNewItemObj::Success SpawnItem"));
		ABaseCharacter* BChar = Cast<ABaseCharacter>(World->GetFirstPlayerController()->GetCharacter());
		
		if (ItemInfo.DataAsset->bIsNeedToAttachHandBeforeUse)
		{
			SpawnItem->AttachToHand(BChar,this);

			//Grenade Type이 아니라면 Attach 이후에 바로 Use 함수를 호출한다.
			if (ItemInfo.DataAsset->ItemType != EItemType::EIT_Grenade)
			{
				SpawnItem->Use(BChar, this);				
			}
		}
		else
		{
			SpawnItem->Use(BChar,this);
		}
		
	}
}