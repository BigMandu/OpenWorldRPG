// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"

#include "OpenWorldRPG/NewInventory/Widget/EquipmentSlot.h"
#include "OpenWorldRPG/NewInventory/Library/InventoryStruct.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"

#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Weapon.h"

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
	if (Equip)
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
	switch (Type)
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
	UNewItemObject* EquipObj = nullptr;

	if (WantToEquip->ItemObj == nullptr)
	{
		EquipObj = UCustomInventoryLibrary::CreateObject(ItemSetting, bCreated); //CreateObject(ItemSetting, bCreated);
		if (bCreated == false) return false;
	}
	else
	{
		EquipObj = WantToEquip->ItemObj;
	}

	bool bAlreadyHave = false;
	bool bReturn = false;

	if (EquipObj)
	{
		// 동일한 Equip이 이미 있는지 확인한다. Swap일때 AddEquip을 하기 때문에
		for(auto& EquippedItem : EquipmentItems)
		{
			if(EquippedItem == EquipObj)
			{
				bAlreadyHave = true;
				break;
			}
		}

		// 동일 Equip이 없다면 추가시킨다.
		if (bAlreadyHave == false)
		{
			//Equip->OwningEquipment = this;
			if (EquipObj->MotherStorage)
			{
				EquipObj->MotherStorage->RemoveItem(EquipObj);
				EquipObj->MotherStorage = nullptr;
			}
			else if (EquipObj->MotherEquipComp)
			{
				EquipObj->MotherEquipComp->RemoveEquipment(EquipObj);
				EquipObj->MotherEquipComp = nullptr;
			}

			//EquipObj->item = WantToEquip;
			EquipObj->Equipment = WantToEquip;
			EquipObj->MotherEquipComp = this;

			if (EquipObj->ItemInfo.DataAsset->EquipmentType == EEquipmentType::EET_Rifle)
			{
				
				EquipObj->RifleAssign = Cast<AWeapon>(WantToEquip)->RifleAssign;
			}

			EquipmentItems.Add(EquipObj);

			//Equip->SKMesh->SetHiddenInGame(false); //임시로 해둔것임.

			//Backpack, Vest의 장착 여부를 확인한다., Enum Range iter를 위해 Obj를 저장해둔다.
			if(EquipObj->ItemInfo.DataAsset->EquipmentType == EEquipmentType::EET_Backpack)
			{
				BackpackObj = EquipObj;
				//bHasBackpack = true;
			}
			else if(EquipObj->ItemInfo.DataAsset->EquipmentType == EEquipmentType::EET_Vest)
			{
				VestObj = EquipObj;
				//bHasVest = true;
			}
			bReturn = true;
		}
		//EquipWidget::RefreshEquipWidget과 bind시켰음. 함수호출
		OnEquipmentUpdated.Broadcast();

		UE_LOG(LogTemp, Warning, TEXT("EquipComp : AddSuccess"));

	}
	return bReturn;
}

bool UEquipmentComponent::RemoveEquipment(UNewItemObject* EquipObj)
{
	if (EquipObj)
	{
		/* EquipSlot::Ondrop에서 기존 SpawnActor의 루틴이 변경됨에 따라
		 * RemoveSingle시에 같지 않게 됨.
		 * 따라서 remove를 변경해야됨.
		 */

		//Backpack, Vest 체크하는 부분은 TenumRange로 변경하면서 사용하지 않는다.
		/*if (EquipObj->ItemInfo.DataAsset->EquipmentType == EEquipmentType::EET_Backpack)
		{
			bHasBackpack = false;
		}
		else if (EquipObj->ItemInfo.DataAsset->EquipmentType == EEquipmentType::EET_Vest)
		{
			bHasVest = false;
		}*/

		
		UNewItemObject* InnerEquipment = GetEquippedWeaponSameType(EquipObj->ItemInfo.DataAsset->EquipmentType, EquipObj);// ->ItemInfo);
		if (InnerEquipment)
		{
			//이름을 비교해서 같은 이름을 갖고있는 객체를 EquipmentItems Array에서 삭제하고
			// Equip Object를 Destory해준다.
			if (InnerEquipment->ItemInfo.DataAsset->ItemName.EqualTo(EquipObj->ItemInfo.DataAsset->ItemName))
			{
				EquipmentItems.RemoveSingle(InnerEquipment);

				if (EquipObj->SettedSlot)
				{
					EquipObj->RemoveLinkSlot();
					//EquipObj->SettedSlot->SettedObj = nullptr;
				}

				//Weapon::Remove에서 OwningPlayer를 사용하기 때문에 순서는 이렇게 한다.
				EquipObj->Equipment->Remove();
				EquipObj->Equipment->OwningPlayer = nullptr;

				EquipObj->Equipment->Destroy();

				//Equip->OwningPlayer = nullptr;
				//InnerEquipment->OwningPlayer = nullptr;
				//Equip->Destroy();
				//InnerEquipment->Destroy();

				OnEquipmentUpdated.Broadcast();

				return true;
			}
		}
	}
	return false;
}

/* EqiupType 또는 ItemSetting과 같은 Type의 EquipObj를 리턴한다.
*/
UNewItemObject* UEquipmentComponent::GetEquippedWeaponSameType(EEquipmentType EquipType, UNewItemObject* Object, ERifleSlot RifleSlot)
{
	for (auto& Equipped : EquipmentItems)
	{
		if (Equipped)
		{
			//Rifle인 경우에 RiffleAssign을 보고 가져온다. 또는 ItemSetting의 Type이 Rifle이면서 Rifle Slot이 지정된 경우.
			//Rifle 타입인 경우 Primary Rifle, Sub Rifle 이렇게 지정되어 있을 수 있기 때문임.
			if (EquipType == EEquipmentType::EET_Rifle)
			{
				if (Equipped->ItemInfo.DataAsset->EquipmentType != EEquipmentType::EET_Rifle) continue;

				if (Object != nullptr && Object->ItemInfo.DataAsset->EquipmentType == EEquipmentType::EET_Rifle)
				{
					if (Equipped->RifleAssign == Object->RifleAssign)
					{
						return Equipped;
					}
				}
				else if(RifleSlot != ERifleSlot::ERS_MAX)
				{
					if (Equipped->RifleAssign == RifleSlot)
					{
						return Equipped;
					}
				}				
			}
			//Rifle이 아닌 나머지 Equipment
			else
			{
				//World에 스폰된 Equipment인 경우 여길 사용한다.
				if (EquipType == EEquipmentType::EET_MAX)
				{
					if (Object != nullptr && Equipped->ItemInfo.DataAsset->EquipmentType == Object->ItemInfo.DataAsset->EquipmentType)
					{
						return Equipped;
					}
				}
				else //Equip없이 EquipType으로만 넘겨줬을때
				{
					if (Equipped->ItemInfo.DataAsset->EquipmentType == EquipType)
					{
						return Equipped;
						/*ReturnEquipObj = Equipped;
						return Cast<AEquipment>(Equipped->item);*/
					}
				}
			}
		}
	}
	//ReturnEquipObj = nullptr;
	return nullptr;
}

bool UEquipmentComponent::IsSameTypeExist(AEquipment* Equip, ERifleSlot RifleSlot)
{
	
	int32 RifleCnt = 0;

	for (auto& Equipped : EquipmentItems)
	{
		if (Equipped)
		{
			//장착하려는 EquipType이 Weapon이고 장착된 Type이 Weapon인 경우 서로 비교한다.
			if (Equipped->ItemInfo.DataAsset->EquipmentType == EEquipmentType::EET_Rifle && Equip->ItemSetting.DataAsset->EquipmentType == EEquipmentType::EET_Rifle)
			{
				//Rifle Slot이 지정되지 않은 경우 (WorldSpawn 상태) 카운트 한다.
				//Rifle Type은 2개까지 허용된다.
				if (RifleSlot == ERifleSlot::ERS_MAX)
				{
					RifleCnt++;
				}
				//Rifle Slot이 지정된 경우 Rifle이 지정된 슬롯에 해당하는게 있는지 확인한다.
				else
				{
					if (Equipped->RifleAssign == RifleSlot)
					{
						return true;
					}
				}
			}
			else if(Equipped->ItemInfo.DataAsset->EquipmentType == Equip->ItemSetting.DataAsset->EquipmentType) //파라미터 Weapon의 Type이 이미 있으면 true
			{
				return true;
			}

			if (RifleCnt >= 2) //RifleType의 무기가 2개이상일 경우 true
			{
				return true;
			}
		}
	}
	//없으면 false리턴
	return false;
}