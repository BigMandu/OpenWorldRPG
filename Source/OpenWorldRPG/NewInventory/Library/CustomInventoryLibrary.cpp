// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"
#include "OpenWorldRPG/NewInventory/Library/InventoryStruct.h"

#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"

#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/Widget/EquipmentSlot.h"

#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/Item/Weapon_Instant.h"
#include "OpenWorldRPG/Item/CoreUsableItem.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Container.h"
#include "OpenWorldRPG/Item/WeaponPartsManagerObject.h"
#include "OpenWorldRPG/BaseCharacter.h"

#include "Kismet/KismetMathLibrary.h"

UNewItemObject* UCustomInventoryLibrary::CreateObject(FItemSetting ItemStruct, bool& bIsCreated)
{
	if ( ItemStruct.DataAsset )
	{
		UNewItemObject* ReturnObj = nullptr;
		UCustomPDA* T_CPDA = Cast<UCustomPDA>(ItemStruct.DataAsset);

		if ( T_CPDA && T_CPDA->bHasStorage )
		{
			ReturnObj = NewObject<UItemStorageObject>();
		}
		else
		{
			ReturnObj = NewObject<UNewItemObject>();
		}
		//ReturnObj->ItemInfo = FItemSetting(ItemStruct.DataAsset, 1, 0);
		ReturnObj->ItemInfo = ItemStruct;

		

		bIsCreated = true;
		return ReturnObj;
	}
	else
	{
		bIsCreated = false;
		return nullptr;
	}

}

void UCustomInventoryLibrary::RemoveFromPreviousMotherContainer(UNewItemObject* Obj)
{
	if ( Obj->MotherStorage )
	{
		Obj->MotherStorage->RemoveItem(Obj);
		Obj->MotherStorage = nullptr;
	}
	else if ( Obj->MotherEquipComp )
	{
		Obj->MotherEquipComp->RemoveEquipment(Obj);
		Obj->MotherEquipComp = nullptr;
	}
	//해당 Item이 이전에 WPM에 있던 Item이면 거기서 지워준다.
	else if ( Obj->WeaponPartsManager )
	{
		Obj->WeaponPartsManager->RemoveParts(Obj);
	}
}

/*ItemObj를 이용해 Item class를 Spawn한다.*/
AItem* UCustomInventoryLibrary::SpawnItem(UWorld* World, UNewItemObject* ItemObj)
{
	if ( World && ItemObj )
	{
		AItem* ReturnItem = nullptr;
		UBasePDA* BPDA = Cast<UBasePDA>(ItemObj->ItemInfo.DataAsset);
		if ( BPDA )
		{
			if ( BPDA->ItemType == EItemType::EIT_Usable )
			{
				ReturnItem = Cast<ACoreUsableItem>(World->SpawnActor<ACoreUsableItem>(ACoreUsableItem::StaticClass()));
			}
			else
			{
				ReturnItem = Cast<AItem>(World->SpawnActor<AItem>(AItem::StaticClass()));
			}

			if ( ReturnItem )
			{
				ItemObj->bIsDestoryed = false;
				ReturnItem->ItemSetting = ItemObj->ItemInfo;
				ReturnItem->SetMesh();
				ReturnItem->SetItemState(EItemState::EIS_Pickup);
				return ReturnItem;
			}
		}
	}
	return nullptr;
}

/*ItemObj를 이용해 Equipment Class를 Spawn한다*/
AEquipment* UCustomInventoryLibrary::SpawnEquipment(UWorld* World, UNewItemObject* ItemObj)//, AActor* Actor)
{
	//UE_LOG(LogTemp, Warning, TEXT("UCustomInventoryLibrary::SpawnEquipment"));
	if ( World && ItemObj )
	{
		AEquipment* Equipment = nullptr;



		UCustomPDA* CPDA = Cast<UCustomPDA>(ItemObj->ItemInfo.DataAsset);
		check(CPDA);

		//Weapon인 경우 WeaponInstant class로 생성해준다.
		UWeaponPDA* WeaponPDA = Cast<UWeaponPDA>(ItemObj->ItemInfo.DataAsset);
		if ( WeaponPDA )
		{
			Equipment = Cast<AWeapon_Instant>(World->SpawnActor<AActor>(AWeapon_Instant::StaticClass()));
			Cast<AWeapon>(Equipment)->SettingWeaponPartsManager();
			/*if (ItemObj->WeaponPartsManager.IsValid())
			{
				Cast<AWeapon>(Equipment)->WeaponPartsManager = ItemObj->WeaponPartsManager.Get();
			}*/
		}
		else
		{
			Equipment = Cast<AEquipment>(World->SpawnActor<AActor>(AEquipment::StaticClass()));// ItemObj->GetItemClass()));
		}


		if ( Equipment )
		{
			ItemObj->bIsDestoryed = false;
			Equipment->ItemSetting = ItemObj->ItemInfo;
			Equipment->SetMesh();
			//Storage가 있으면 Inventory를 넘겨준다.
			if ( CPDA->bHasStorage )
			{
				UItemStorageObject* StorageObj = Cast<UItemStorageObject>(ItemObj);
				if ( StorageObj )
				{
					//Inventory를 넘기지 않고, Object를 통째로 넘겨
					//장착 해제후 재장착시 Inventory를 그대로 불러온다.
					Equipment->ItemObj = StorageObj;

					//아래 방식으로 하면 ItemObj를 생성하는 과정중에 날아감.
					//Equipment->ItemSetting.Inventory = StorageObj->Inventory;
				}
			}



			Equipment->SetItemState(EItemState::EIS_Pickup);
			return Equipment;
		}
	}
	return nullptr;
}

ABaseGrenade* UCustomInventoryLibrary::SpawnGrenade(UWorld* World, UNewItemObject* ItemObj)
{
	if ( World && ItemObj )
	{
		ABaseGrenade* ReturnActor = nullptr;
		UGrenadePDA* GPDA = Cast<UGrenadePDA>(ItemObj->ItemInfo.DataAsset);
		if ( GPDA )
		{
			ReturnActor = Cast<ABaseGrenade>(World->SpawnActor<ABaseGrenade>(ABaseGrenade::StaticClass()));

			if ( ReturnActor )
			{
				ItemObj->bIsDestoryed = false;
				ReturnActor->ItemSetting = ItemObj->ItemInfo;
				ReturnActor->SetMesh();
				ReturnActor->SetItemState(EItemState::EIS_Pickup);
				return ReturnActor;
			}
		}
	}
	return nullptr;
}




/*PDA를 이용해 Item class를 Spawn한다.*/
AItem* UCustomInventoryLibrary::SpawnItem(UWorld* World, UBasePDA* ItemDA)
{
	AItem* ReturnItem = nullptr;
	if ( World && ItemDA )
	{
		UBasePDA* BPDA = Cast<UBasePDA>(ItemDA);
		if ( BPDA )
		{

			ReturnItem = Cast<AItem>(World->SpawnActor<AItem>(AItem::StaticClass()));

			if ( ReturnItem )
			{
				//ItemObj->bIsDestoryed = false;
				ReturnItem->ItemSetting = FItemSetting(ItemDA, 1, 0);
				ReturnItem->SetMesh();
				ReturnItem->SetItemState(EItemState::EIS_Spawn);
				return ReturnItem;
			}
		}
	}
	return ReturnItem;
}

/*PDA를 이용해 Equipment Class를 Spawn한다 */
AEquipment* UCustomInventoryLibrary::SpawnEquipment(UWorld* World, UCustomPDA* EquipDA)
{
	if ( World && EquipDA )
	{
		AEquipment* Equipment = nullptr;

		//Weapon인 경우 WeaponInstant로 생성해준다.

		UCustomPDA* T_CPDA = Cast<UCustomPDA>(EquipDA);
		UCustomPDA* CPDA = T_CPDA ? T_CPDA : nullptr;


		UWeaponPDA* WeaponPDA = Cast<UWeaponPDA>(EquipDA);
		if ( WeaponPDA )
		{
			Equipment = Cast<AWeapon_Instant>(World->SpawnActor<AActor>(AWeapon_Instant::StaticClass()));
		}
		else if ( CPDA )
		{
			Equipment = Cast<AEquipment>(World->SpawnActor<AActor>(AEquipment::StaticClass()));// ItemObj->GetItemClass()));
		}


		if ( Equipment )
		{
			//ItemObj->bIsDestoryed = false;
			Equipment->ItemSetting = FItemSetting(EquipDA, 1, 0);
			Equipment->SetMesh();
			Equipment->SetItemState(EItemState::EIS_Pickup);
			return Equipment;
		}
	}
	return nullptr;
}


TArray<UCustomPDA*> UCustomInventoryLibrary::RandomSpawnUsingTableButCount(int32 SpawnCount, TArray<UCustomPDA*> SpawnTable)
{
	TArray<UCustomPDA*> ReturnTable;
	int32 loop = 0;
	int32 loopingMax = 0;
	while ( loop < SpawnCount )
	{
		//원하는 SpawnCount만큼 얻었거나, 반복문을 SpawnTable 개수 3배만큼 돌았으면 빠져나오도록 한다.
		//두번째는 무한 루프 방지.
		if ( loop >= SpawnCount || loopingMax >= SpawnTable.Num()*3)
		{
			break;
		}

		int32 RandNum = FMath::RandRange(0,SpawnTable.Num());
		if ( SpawnTable.IsValidIndex(RandNum) )
		{
			ReturnTable.Add(SpawnTable[RandNum]);
			++loop;
		}
		++loopingMax;
	}

	return ReturnTable;
}

//Item의 Type별로 개수를 랜덤 지정한다.
void UCustomInventoryLibrary::GenerateRandomCount(UNewItemObject* ItemObj)
{
	//UE_LOG(LogTemp, Warning, TEXT("CustomInvLib::GenerateRandomCount"));
	int32 MAX_Generate = 1;
	int32 MIN_Generate = 1;

	switch ( ItemObj->ItemInfo.DataAsset->ItemType )
	{
	case EItemType::EIT_Food:
		MAX_Generate = 5;
		MIN_Generate = 2;
		break;
	case EItemType::EIT_Medical:
		MAX_Generate = 3;
		MIN_Generate = 1;
		break;
	case EItemType::EIT_Scrap:
		MAX_Generate = 8;
		MIN_Generate = 3;
		break;
	}

	int32 Count = UKismetMathLibrary::RandomIntegerInRange(MIN_Generate, MAX_Generate);
	ItemObj->ItemInfo.Count = Count;

}


//bRenderInMainPass를 false로 하게되면 Rendering은 되지 않지만, Shadow는 렌더링 된다.
// 이걸 사용하려면 Mesh가 Show되어야 한다. SetHiddenInGame을 True로 하면 안됨.

/* Weapon을 제외한 장착중인 모든 Equipment를 Hide한다. */
void UCustomInventoryLibrary::HideAllEquipment(UEquipmentComponent* EComp)
{
	UCustomPDA* CPDA;
	/* 장착한 장비 모두 HIde하기 */
	for ( auto Equipped : EComp->EquipmentItems )
	{
		if ( Equipped && Equipped->Equipment )
		{
			CPDA = Cast<UCustomPDA>(Equipped->Equipment->ItemSetting.DataAsset);

			if ( CPDA &&
				( CPDA->EquipmentType == EEquipmentType::EET_Rifle
					|| CPDA->EquipmentType == EEquipmentType::EET_Pistol ) )
			{
				continue;
			}
			Equipped->Equipment->SKMesh->SetRenderInMainPass(false);
		}
	}
}


/* 장착중인 모든 Equipment를 show 한다.*/
void UCustomInventoryLibrary::ShowAllEquipment(UEquipmentComponent* EComp)
{
	for ( auto Equipped : EComp->EquipmentItems )
	{
		if ( Equipped && Equipped->Equipment )
		{

			Equipped->Equipment->SKMesh->SetRenderInMainPass(true);
		}
	}
}



/**이 함수가 호출 되는 경우
 * 1. Weapon을 해제하여(Destory) Inventory에 넣을 때
 * 2. Weapon을 Equip할 때
 *
 * 두 경우 모두 ItemObj가 생성될 때 호출된다.
 */

 /**여기서 해야할거
  * 이 함수는 "Obj가 생길때 호출된다."
  * Weapon의 WPM은 Weapon이 생성될때 생성되는데 이 WPM의 세팅값을
  *
  * 생성된 OBJ에 WPM이 없다면 기존걸 대입하고, 있다면 SetOwnerWeap을 해주면될거같다.
  * 
  */
void UCustomInventoryLibrary::SetWeaponPartsManager(UNewItemObject* Obj, AWeapon* Weapon)
{
	/*Obj에 이미 WPM이 있다면
	* Obj->WPM의 Data를 Weapon의 WPM에 넘겨준다.
	* */
	if ( Obj->WeaponPartsManager)
	{
		if ( Obj->WeaponPartsManager->MuzzleParts )
		{
			Obj->WeaponPartsManager->A_MuzzleParts = nullptr;
			Weapon->WeaponPartsManager->MuzzleParts = Obj->WeaponPartsManager->MuzzleParts;
		}
		if ( Obj->WeaponPartsManager->ScopeParts )
		{
			Obj->WeaponPartsManager->A_ScopeParts = nullptr;
			Weapon->WeaponPartsManager->ScopeParts = Obj->WeaponPartsManager->ScopeParts;
		}
		if ( Obj->WeaponPartsManager->TacticalParts )
		{
			Obj->WeaponPartsManager->A_TacticalParts = nullptr;
			Weapon->WeaponPartsManager->TacticalParts = Obj->WeaponPartsManager->TacticalParts;
		}

		Obj->WeaponPartsManager->SetOwnerWeapon(Weapon->WeaponPartsManager->GetOwnerWeapon());
	}
	//Obj에 WPM이 없다면 Weapon의 Data를 그대로 대입해주고 끝낸다.
	else
	{
		Obj->WeaponPartsManager = Weapon->WeaponPartsManager;
	}
}



/////////////////////////////////////////////////////////////////////////
/**********************   이하 사용하지 않는 함수들 **********************/
/////////////////////////////////////////////////////////////////////////


void UCustomInventoryLibrary::BackToItem(UNewItemObject* ItemObj)
{
	UE_LOG(LogTemp, Warning, TEXT("UCustomInventoryLibrary::BackToItem"));
	if ( ItemObj )
	{
		//ItemObj가 원래 있던 MotherContainer를 가져와
		if ( ItemObj->GetMotherStorage() != nullptr )
		{
			//UNewInventoryGrid* GridInv = ItemObj->GetMotherContainer();
			UNewInventoryGrid* GridInv = nullptr;// = ItemObj->GetMotherContainer();
			//원래 있던 Mothercontainer에 넣는다.
			if ( GridInv->GetInventoryComp() != nullptr )
			{
				if ( GridInv->GetInventoryComp()->IsAvailableSpace(ItemObj, ItemObj->TopLeftIndex) )
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
		/*else if (ItemObj->GetMotherEquipSlot() != nullptr)
		{
			ItemObj->GetMotherEquipSlot()->TrySlotEquip(ItemObj);
		}*/
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UNewInvGrid::OnDrop , Error!"));
		}
	}
}

void UCustomInventoryLibrary::DirectInToInventory(UNewItemObject* ItemObj, ABaseCharacter* BChar)
{
	UE_LOG(LogTemp, Warning, TEXT("UCustomInventoryLibrary::DirectInToInv"));
	if ( ItemObj )
	{
		//if (ItemObj->GetMotherContainer() != nullptr)
		{
			UNewInventoryGrid* GridInv = nullptr;// = ItemObj->GetMotherContainer();
			if ( GridInv->GetInventoryComp() != nullptr )
			{
				if ( GridInv->GetInventoryComp()->IsAvailableSpace(ItemObj, ItemObj->TopLeftIndex) )
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
		//else if (ItemObj->GetMotherEquipSlot() != nullptr)
		{
			if ( BChar )
			{
				if ( BChar->InteractLootBox )
				{
					BChar->InteractLootBox->ContainerInventoryComp->TryAddItem(ItemObj);
				}
			}

		}
	}
}