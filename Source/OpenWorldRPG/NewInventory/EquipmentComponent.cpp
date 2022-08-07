// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"

#include "OpenWorldRPG/NewInventory/Library/InventoryStruct.h"

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
		//if (Equip->OwningInventory != nullptr) //�κ��丮�� �ִ� Weapon�̸�,
		//{
		//	Equip->OwningInventory->RemoveItem(Cast<AItem>(this)); //�κ��丮���� �����ش�.
		//}

		// ������ Equip�� �̹� �ִ��� Ȯ���Ѵ�. Swap�϶� AddEquip�� �ϱ� ������
		for(auto& EquippedItem : EquipmentItems)
		{
			if(EquippedItem == Equip)
			{
				bAlreadyHave = true;
				break;
			}
		}

		// ���� Equip�� ���ٸ� �߰���Ų��. 
		if (bAlreadyHave == false)
		{
			Equip->OwningEquipment = this;
			EquipmentItems.Add(Equip);
			Equip->SKMesh->SetHiddenInGame(false); //�ӽ÷� �صа���.

			//Backpack, Vest�� ���� ���θ� Ȯ���Ѵ�.
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
		//EquipWidget::RefreshEquipWidget�� bind��Ŵ.
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
		* EquipSlot::Ondrop���� ���� SpawnActor�� ��ƾ�� ����ʿ� ����
		* RemoveSingle�ÿ� ���� �ʰ� ��.
		* ���� remove�� �����ؾߵ�.
		*/
		//if (Equip->EquipmentType == EEquipmentType::EET_Backpack)
		{
			bHasBackpack = false;
		}
		//else if (Equip->EquipmentType == EEquipmentType::EET_Vest)
		{
			bHasVest = false;
		}

		/*
		AEquipment* InnerEquipment = GetEquippedWeaponSameType(Equip->EquipmentType);
		if (InnerEquipment)
		{
			//�̸��� ���ؼ� ���� �̸��� �����ִ� ��ü�� EquipmentItems Array���� �����ϰ�
			// Equip Object�� Destory���ش�.
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
			//Rifle�� ��쿡 RiffleAssign�� ���� �����´�.
			//Rifle Ÿ���� ��� Primary Rifle, Sub Rifle �̷��� �����Ǿ� ���� �� �ֱ� ������.
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
				//World�� ������ Equipment�� ��� ���� ����Ѵ�.
				if (Equip != nullptr && EquipType == EEquipmentType::EET_MAX)
				{
					if (Equipped->EquipmentType == Equip->EquipmentType)
					{
						return Equipped;
					}
				}
				else //Equip���� EquipType���θ� �Ѱ�������
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

bool UEquipmentComponent::AddEquipment(UNewItemObject* EquipObj)
{
	UE_LOG(LogTemp, Warning, TEXT("EquipComp : AddEquip"));
	bool bAlreadyHave = false;
	bool bReturn = false;

	if (EquipObj)
	{
		// ������ Equip�� �̹� �ִ��� Ȯ���Ѵ�. Swap�϶� AddEquip�� �ϱ� ������
		for(auto& EquippedItem : EquipmentItems)
		{
			if(EquippedItem == EquipObj)
			{
				bAlreadyHave = true;
				break;
			}
		}

		// ���� Equip�� ���ٸ� �߰���Ų��.
		if (bAlreadyHave == false)
		{
			//Equip->OwningEquipment = this;

			EquipmentItems.Add(EquipObj);

			//Equip->SKMesh->SetHiddenInGame(false); //�ӽ÷� �صа���.

			//Backpack, Vest�� ���� ���θ� Ȯ���Ѵ�.
			
			if(EquipObj->ItemInfo.DataAsset->EquipmentType == EEquipmentType::EET_Backpack)
			{
				bHasBackpack = true;
			}
			else if(EquipObj->ItemInfo.DataAsset->EquipmentType == EEquipmentType::EET_Vest)
			{
				bHasVest = true;
			}
			bReturn = true;
		}
		//EquipWidget::RefreshEquipWidget�� bind��Ŵ.
		OnEquipmentUpdated.Broadcast();

		UE_LOG(LogTemp, Warning, TEXT("EquipComp : AddSuccess"));

	}
	return bReturn;
}

bool UEquipmentComponent::RemoveEquipment(UNewItemObject* EquipObj)
{
	if (EquipObj)
	{
		/*
		* EquipSlot::Ondrop���� ���� SpawnActor�� ��ƾ�� ����ʿ� ����
		* RemoveSingle�ÿ� ���� �ʰ� ��.
		* ���� remove�� �����ؾߵ�.
		*/
		if (EquipObj->ItemInfo.DataAsset->EquipmentType == EEquipmentType::EET_Backpack)
		{
			bHasBackpack = false;
		}
		else if (EquipObj->ItemInfo.DataAsset->EquipmentType == EEquipmentType::EET_Vest)
		{
			bHasVest = false;
		}

		
		UNewItemObject* InnerEquipment = GetEquippedWeaponSameType(EquipObj->ItemInfo.DataAsset->EquipmentType, EquipObj->ItemInfo);
		if (InnerEquipment)
		{
			//�̸��� ���ؼ� ���� �̸��� �����ִ� ��ü�� EquipmentItems Array���� �����ϰ�
			// Equip Object�� Destory���ش�.
			//if (InnerEquipment->ItemName.EqualTo(Equip->ItemName))
			{
				EquipmentItems.RemoveSingle(InnerEquipment);

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

/* EqiupType �Ǵ� ItemSetting�� ���� Type�� EquipObj�� �����Ѵ�.
*/
UNewItemObject* UEquipmentComponent::GetEquippedWeaponSameType(EEquipmentType EquipType, FItemSetting ItemSetting, ERifleSlot RifleSlot)
{
	for (auto& Equipped: EquipmentItems)
	{
		if (Equipped)
		{
			//Rifle�� ��쿡 RiffleAssign�� ���� �����´�. �Ǵ� ItemSetting�� Type�� Rifle�̸鼭 Rifle Slot�� ������ ���.
			//Rifle Ÿ���� ��� Primary Rifle, Sub Rifle �̷��� �����Ǿ� ���� �� �ֱ� ������.
			if ((EquipType == EEquipmentType::EET_Rifle || (ItemSetting.DataAsset->EquipmentType == EEquipmentType::EET_Rifle)
				&& RifleSlot != ERifleSlot::ERS_MAX))
			{
				AWeapon* EquippedWeapon = Cast<AWeapon>(Equipped->item);
				if (EquippedWeapon)
				{
					if (EquippedWeapon->RifleAssign == RifleSlot)
					{
						return Equipped;
						/*ReturnEquipObj = Equipped;
						return EquippedWeapon;*/
					}
				}
			}
			//Rifle�� �ƴ� ������ Equipment
			else
			{
				//World�� ������ Equipment�� ��� ���� ����Ѵ�.
				if (EquipType == EEquipmentType::EET_MAX)
				{
					if (Equipped->EquipmentType == ItemSetting.DataAsset->EquipmentType)
					{
						return Equipped;
						/*ReturnEquipObj = Equipped;
						return Cast<AEquipment>(Equipped->item);*/
					}
				}
				else //Equip���� EquipType���θ� �Ѱ�������
				{
					if (Equipped->EquipmentType == EquipType)
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
			//�����Ϸ��� EquipType�� Weapon�̰� ������ Type�� Weapon�� ��� ���� ���Ѵ�.
			if (Equipped->EquipmentType == EEquipmentType::EET_Rifle && Equip->ItemSetting.DataAsset->EquipmentType == EEquipmentType::EET_Rifle)
			{
				//Rifle Slot�� �������� ���� ��� (WorldSpawn ����) ī��Ʈ �Ѵ�.
				//Rifle Type�� 2������ ���ȴ�.
				if (RifleSlot == ERifleSlot::ERS_MAX)
				{
					RifleCnt++;
				}
				//Rifle Slot�� ������ ��� Rifle�� ������ ���Կ� �ش��ϴ°� �ִ��� Ȯ���Ѵ�.
				else
				{
					AWeapon* EquippedWeapon = Cast<AWeapon>(Equipped);
					if (EquippedWeapon)
					{
						if (EquippedWeapon->RifleAssign == RifleSlot)
						{
							return true;
						}
					}
				}
			}
			else if(Equipped->EquipmentType == Equip->ItemSetting.DataAsset->EquipmentType) //�Ķ���� Weapon�� Type�� �̹� ������ true
			{
				return true;
			}

			if (RifleCnt >= 2) //RifleType�� ���Ⱑ 2���̻��� ��� true
			{
				return true;
			}
		}
	}
	//������ false����
	return false;
}