// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"

#include "OpenWorldRPG/CustomLibrary/CustomEnumLibrary.h"

#include "OpenWorldRPG/UI/Inventory/EquipmentSlot.h"

#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/Item/WeaponPartsManagerObject.h"

// Sets default values for this component's properties
UEquipmentComponent::UEquipmentComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


// Called when the game starts
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UEquipmentComponent::AddEquipment(AEquipment* Equip)
{

	UE_LOG(LogTemp, Warning, TEXT("EquipComp : AddEquip"));
	bool bAlreadyHave = false;
	bool bReturn = false;

	/*
	if (Equip)
	{
		//if (Equip->OwningInventory != nullptr) //인벤토리에 있던 Weapon이면,
		//{
		//	Equip->OwningInventory->RemoveItem(Cast<AItem>(this)); //인벤토리에서 지워준다.
		//}
		// 동일한 Equip이 이미 있는지 확인한다. Swap일때 AddEquip을 하기 때문에
		for(auto& EquippedItem : EquipmentItems)
		{
			if(EquippedItem == Equip)
			{
				bAlreadyHave = true;
				break;
			}
		}
		// 동일 Equip이 없다면 추가시킨다.
		if (bAlreadyHave == false)
		{
			Equip->OwningEquipment = this;
			EquipmentItems.Add(Equip);
			Equip->SKMesh->SetHiddenInGame(false); //임시로 해둔것임.
			//Backpack, Vest의 장착 여부를 확인한다.
			if(Equip->EquipmentType == EEquipmentType::EET_Backpack)
			{
				bHasBackpack = true;
			}
			else if(Equip->EquipmentType == EEquipmentType::EET_Vest)
			{
				bHasVest = true;
			}
			bReturn = true;
		}
		//EquipWidget::RefreshEquipWidget과 bind시킴.
		OnEquipmentUpdated.Broadcast();
		UE_LOG(LogTemp, Warning, TEXT("EquipComp : AddSuccess"));
	}*/
	return bReturn;
}



bool UEquipmentComponent::RemoveEquipment(AEquipment* Equip)
{
	if ( Equip )
	{
		/*
		* EquipSlot::Ondrop에서 기존 SpawnActor의 루틴이 변경됨에 따라
		* RemoveSingle시에 같지 않게 됨.
		* 따라서 remove를 변경해야됨.
		*/
		//if (Equip->EquipmentType == EEquipmentType::EET_Backpack)
		{
			//bHasBackpack = false;
		}
		//else if (Equip->EquipmentType == EEquipmentType::EET_Vest)
		{
			//bHasVest = false;
		}

		/*
		AEquipment* InnerEquipment = GetEquippedWeaponSameType(Equip->EquipmentType);
		if (InnerEquipment)
		{
			//이름을 비교해서 같은 이름을 갖고있는 객체를 EquipmentItems Array에서 삭제하고
			// Equip Object를 Destory해준다.
			//if (InnerEquipment->ItemName.EqualTo(Equip->ItemName))
			{
				EquipmentItems.RemoveSingle(InnerEquipment);
				Equip->OwningPlayer = nullptr;
				InnerEquipment->OwningPlayer = nullptr;
				Equip->Destroy();
				InnerEquipment->Destroy();
				OnEquipmentUpdated.Broadcast();
				return true;
			}
		}*/
	}
	return false;
}




/*
AEquipment* UEquipmentComponent::GetEquippedWeaponSameType(EEquipmentType EquipType, AEquipment* Equip, ERifleSlot RifleSlot)
{
	for (auto& EquipItem : EquipmentItems)
	{
		AEquipment* Equipped = Cast<AEquipment>(EquipItem);
		if (Equipped)
		{
			//Rifle인 경우에 RiffleAssign을 보고 가져온다.
			//Rifle 타입인 경우 Primary Rifle, Sub Rifle 이렇게 지정되어 있을 수 있기 때문임.
			if ((EquipType == EEquipmentType::EET_Rifle || (Equip != nullptr && Equip->EquipmentType == EEquipmentType::EET_Rifle)
				&& RifleSlot != ERifleSlot::ERS_MAX))
			{
				AWeapon* EquippedWeapon = Cast<AWeapon>(Equipped);
				if (EquippedWeapon)
				{
					if (EquippedWeapon->RifleAssign == RifleSlot)
					{
						return EquippedWeapon;
					}
				}
			}
			else
			{
				//World에 스폰된 Equipment인 경우 여길 사용한다.
				if (Equip != nullptr && EquipType == EEquipmentType::EET_MAX)
				{
					if (Equipped->EquipmentType == Equip->EquipmentType)
					{
						return Equipped;
					}
				}
				else //Equip없이 EquipType으로만 넘겨줬을때
				{
					if (Equipped->EquipmentType == EquipType)
					{
						return Equipped;
					}
				}
			}
		}
	}
	return nullptr;
}
*/


bool UEquipmentComponent::SwapEquipment(AEquipment* Before, AEquipment* Next)
{
	/*
	//double check
	bool bReturn = false;
	if(Before->EquipmentType == Next->EquipmentType)
	{
		if(RemoveEquipment(Before))
		{
			bReturn = AddEquipment(Next);
		}
	}
	return bReturn;
	*/
	return false;
}



/*******************************************************************************************/
/*********************************** New Version *******************************************/
/*******************************************************************************************/

UNewItemObject* UEquipmentComponent::GetEquipStorage(EEquipmentType Type)
{
	switch ( Type )
	{
	case EEquipmentType::EET_Backpack:
		return BackpackObj;
	case EEquipmentType::EET_Vest:
		return VestObj;
	default:
		return nullptr;
	}
}

bool UEquipmentComponent::AddEquipment(FItemSetting ItemSetting, AEquipment* WantToEquip)
{
	UE_LOG(LogTemp, Warning, TEXT("EquipComp : AddEquip"));
	bool bCreated = false;
	bool bReturn = false;

	UNewItemObject* EquipObj = nullptr;

	//Equip의 ItemObj가 없다면 생성해준다.
	if ( WantToEquip->ItemObj == nullptr )
	{
		EquipObj = UCustomInventoryLibrary::CreateObject(ItemSetting, bCreated); //CreateObject(ItemSetting, bCreated);
		if ( bCreated == false ) return false;
		
		if ( UItemStorageObject* T_Obj = Cast<UItemStorageObject>(EquipObj) )
		{
			//Inventory에 Item이 있었다면 넘겨준다.
			if ( ItemSetting.Inventory.Num() > 0 )
			{
				T_Obj->ItemInfo.Inventory = ItemSetting.Inventory;
			}
		}

	}
	else
	{
		EquipObj = WantToEquip->ItemObj;
	}


	UCustomPDA* CPDA = Cast<UCustomPDA>(EquipObj->ItemInfo.DataAsset);

	if ( EquipObj && CPDA )
	{
		UCustomInventoryLibrary::RemoveFromPreviousMotherContainer(EquipObj);

		EquipObj->bTempRotate = false;
		EquipObj->bRotated = false;

		EquipObj->Equipment = WantToEquip;
		EquipObj->MotherEquipComp = this;

		//RifleType이면 RifleAssign을 대입해준다. //for QuickSlot & EquipWidget
		if ( CPDA->EquipmentType == EEquipmentType::EET_Rifle || CPDA->EquipmentType == EEquipmentType::EET_Pistol )
		{
			EquipObj->RifleAssign = Cast<AWeapon>(WantToEquip)->RifleAssign;
			EquipObj->AmmoLeftInMag = Cast<AWeapon>(WantToEquip)->AmmoLeftInMag;

			UCustomInventoryLibrary::SetWeaponPartsManager(EquipObj, Cast<AWeapon>(WantToEquip));
			if ( EquipObj->WeaponPartsManager )
			{
				EquipObj->WeaponPartsManager->OnChangeParts.Clear();
				EquipObj->WeaponPartsManager->OnChangeParts.AddDynamic(Cast<AWeapon>(WantToEquip), &AWeapon::UpdateWeaponParts);
			}
			OnWeaponSetSlot.Broadcast(EquipObj);
		}



		//Backpack, Vest의 장착 여부를 확인한다., Enum Range iter를 위해 Obj를 저장해둔다.
		if ( CPDA->EquipmentType == EEquipmentType::EET_Backpack )
		{
			BackpackObj = EquipObj;
		}
		else if ( CPDA->EquipmentType == EEquipmentType::EET_Vest )
		{
			VestObj = EquipObj;
		}


		EquipmentItems.Add(EquipObj);
		WantToEquip->ItemObj = EquipObj;
		bReturn = true;


		//EquipWidget::RefreshEquipWidget과 bind시켰음.
		OnEquipmentUpdated.Broadcast();
		UE_LOG(LogTemp, Warning, TEXT("EquipComp : AddSuccess"));

	}
	return bReturn;
}

bool UEquipmentComponent::RemoveEquipment(UNewItemObject* EquipObj)
{
	if ( EquipObj )
	{
		/* EquipSlot::Ondrop에서 기존 SpawnActor의 루틴이 변경됨에 따라
		 * RemoveSingle시에 같지 않게 됨.
		 * 따라서 remove를 변경해야됨.
		 */

		UCustomPDA* OuterCPDA = Cast<UCustomPDA>(EquipObj->ItemInfo.DataAsset);
		UNewItemObject* InnerEquipment = GetEquippedWeaponSameType(OuterCPDA->EquipmentType, EquipObj);// ->ItemInfo);

		if ( InnerEquipment )
		{
			UCustomPDA* InnerCPDA = Cast<UCustomPDA>(InnerEquipment->ItemInfo.DataAsset);
			//이름을 비교해서 같은 이름을 갖고있는 객체를 EquipmentItems Array에서 삭제하고
			// Equip Object를 Destory해준다.

			if ( InnerCPDA->ItemName.EqualTo(OuterCPDA->ItemName) )
			{
				//BackPack,Vest의 경우 Set한 Obj를 날려준다.
				if ( InnerCPDA->EquipmentType == EEquipmentType::EET_Backpack )
				{
					BackpackObj = nullptr;
				}
				else if ( InnerCPDA->EquipmentType == EEquipmentType::EET_Vest )
				{
					VestObj = nullptr;
				}

				//for QuickSlot
				if ( InnerCPDA->EquipmentType == EEquipmentType::EET_Rifle || InnerCPDA->EquipmentType == EEquipmentType::EET_Pistol )
				{
					OnWeaponRemoveSlot.Broadcast(EquipObj->RifleAssign);
					EquipObj->RifleAssign = ERifleSlot::ERS_MAX;

				}

				EquipmentItems.RemoveSingle(InnerEquipment);

				if ( EquipObj->SettedSlot )
				{
					EquipObj->RemoveLinkSlot();
				}

				//Weapon::Remove에서 OwningPlayer를 사용하기 때문에 순서는 이렇게 한다.
				EquipObj->Equipment->Remove();
				/*EquipObj->Equipment->OwningPlayer = nullptr;
				EquipObj->Equipment->Destroy();*/

				OnEquipmentUpdated.Broadcast();

				return true;
			}
		}
	}
	return false;
}

/* EqiupType 또는 Obj와 같은 Type의 EquipObj를 리턴한다.
*/
UNewItemObject* UEquipmentComponent::GetEquippedWeaponSameType(EEquipmentType EquipType, UNewItemObject* Object, ERifleSlot RifleSlot)
{
	for ( auto& Equipped : EquipmentItems )
	{
		if ( Equipped )
		{
			UCustomPDA* CPDA = Cast<UCustomPDA>(Equipped->ItemInfo.DataAsset);
			UCustomPDA* ObjCPDA = Cast<UCustomPDA>(Object->ItemInfo.DataAsset);

			if ( !CPDA || !ObjCPDA ) continue;


			//Rifle인 경우에 RiffleAssign을 보고 가져온다. 또는 ItemSetting의 Type이 Rifle이면서 Rifle Slot이 지정된 경우.
			//Rifle 타입인 경우 Primary Rifle, Sub Rifle 이렇게 지정되어 있을 수 있기 때문임.
			if ( EquipType == EEquipmentType::EET_Rifle )
			{

				if ( CPDA && CPDA->EquipmentType != EEquipmentType::EET_Rifle ) continue;

				if ( Object != nullptr && ObjCPDA && ObjCPDA->EquipmentType == EEquipmentType::EET_Rifle )
				{
					if ( Equipped->RifleAssign == Object->RifleAssign )
					{
						return Equipped;
					}
				}
				else if ( RifleSlot != ERifleSlot::ERS_MAX )
				{
					if ( Equipped->RifleAssign == RifleSlot )
					{
						return Equipped;
					}
				}
			}
			else
			{
				//World에 스폰된 Equipment인 경우 여길 사용한다.
				if ( EquipType == EEquipmentType::EET_MAX && CPDA )
				{
					if ( Object != nullptr && CPDA->EquipmentType == ObjCPDA->EquipmentType )
					{
						return Equipped;
					}
				}
				else //Equip없이 EquipType으로만 넘겨줬을때
				{
					if ( CPDA->EquipmentType == EquipType )
					{
						return Equipped;
					}
				}
			}
		}
	}
	return nullptr;
}

bool UEquipmentComponent::IsSameTypeExist(AEquipment* Equip, ERifleSlot RifleSlot)
{

	int32 RifleCnt = 0;

	for ( auto& Equipped : EquipmentItems )
	{
		if ( Equipped )
		{
			UCustomPDA* Var_CPDA = Cast<UCustomPDA>(Equip->ItemSetting.DataAsset);
			UCustomPDA* Inner_CPDA = Cast<UCustomPDA>(Equipped->ItemInfo.DataAsset);

			//장착하려는 EquipType이 Weapon이고 장착된 Type이 Weapon인 경우 서로 비교한다.
			if ( Inner_CPDA->EquipmentType == EEquipmentType::EET_Rifle && Var_CPDA->EquipmentType == EEquipmentType::EET_Rifle )
			{

				//Rifle Slot이 지정되지 않은 경우 (WorldSpawn 상태) 카운트 한다.
				//Rifle Type은 2개까지 허용된다.
				if ( RifleSlot == ERifleSlot::ERS_MAX )
				{
					RifleCnt++;
				}
				//Rifle Slot이 지정된 경우 Rifle이 지정된 슬롯에 해당하는게 있는지 확인한다.
				else
				{
					if ( Equipped->RifleAssign == RifleSlot )
					{
						return true;
					}
				}
			}
			else if ( Inner_CPDA->EquipmentType == Var_CPDA->EquipmentType ) //파라미터 Weapon의 Type이 이미 있으면 true
			{
				return true;
			}

			if ( RifleCnt >= 2 ) //RifleType의 무기가 2개이상일 경우 true
			{
				return true;
			}
		}
	}
	//없으면 false리턴
	return false;
}

void UEquipmentComponent::UpdateEquipment(TArray<UNewItemObject*>& SavedEquipment)
{
	if ( SavedEquipment.Num() < 1 ) return;

	for ( UNewItemObject* Equipment : SavedEquipment )
	{
		if ( Equipment == nullptr || Equipment->ItemInfo.DataAsset == nullptr ) continue;

		AEquipment* SpawnEquip = UCustomInventoryLibrary::SpawnEquipment(GetWorld(), Equipment);

		UCustomPDA* CPDA = Cast<UCustomPDA>(SpawnEquip->ItemSetting.DataAsset);
		if ( CPDA->EquipmentType == EEquipmentType::EET_Rifle )
		{
			SpawnEquip->Equip(GetOwner(), ERifleSlot::ERS_Primary);
		}
		else
		{
			SpawnEquip->Equip(GetOwner());
		}
	}

	/*if ( EquipmentItems.Num() < 1 ) return;

	for ( UNewItemObject* Equipment : EquipmentItems )
	{
		if ( Equipment == nullptr || Equipment->ItemInfo.DataAsset == nullptr ) continue;

		AEquipment* SpawnEquip = UCustomInventoryLibrary::SpawnEquipment(GetWorld(), Equipment);

		UCustomPDA* CPDA = Cast<UCustomPDA>(SpawnEquip->ItemSetting.DataAsset);
		if ( CPDA->EquipmentType == EEquipmentType::EET_Rifle )
		{
			SpawnEquip->Equip(GetOwner(), ERifleSlot::ERS_Primary);
		}
		else
		{
			SpawnEquip->Equip(GetOwner());
		}
	}*/
	

}
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

//Weapon에 있는 WPM을 Obj의 WPM에 대입해서 갱신해준다.
//void UEquipmentComponent::SetWeaponPartsManager(AEquipment* WantToEquip, UNewItemObject* WeaponObj)
//{
//	if (UWeaponPDA* WeaponPDA = Cast<UWeaponPDA>(WeaponObj->ItemInfo.DataAsset))
//	{
//		if (WeaponObj->WeaponPartsManager.IsValid() == false)
//		{
//			if (UWeaponPartsManagerObject* WeapWPM = Cast<AWeapon>(WantToEquip)->WeaponPartsManager)
//			{
//				WeaponObj->WeaponPartsManager = WeapWPM;
//			}
//			else
//			{
//				WeaponObj->WeaponPartsManager = NewObject<UWeaponPartsManagerObject>(UWeaponPartsManagerObject::StaticClass());
//			}
//		}
//	}	
//}