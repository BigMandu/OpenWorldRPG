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

bool UEquipmentComponent::AddEquipment(FItemSetting ItemSetting, AEquipment* WantToEquip)
{
	UE_LOG(LogTemp, Warning, TEXT("EquipComp : AddEquip"));
	bool bCreated = false;
	bool bReturn = false;

	UNewItemObject* EquipObj = nullptr;

	//Equip�� ItemObj�� ���ٸ� �������ش�.
	if (WantToEquip->ItemObj == nullptr)
	{
		EquipObj = UCustomInventoryLibrary::CreateObject(ItemSetting, bCreated); //CreateObject(ItemSetting, bCreated);
		if (bCreated == false) return false;
	}
	else
	{
		EquipObj = WantToEquip->ItemObj;
	}
	
	UCustomPDA* CPDA = Cast<UCustomPDA>(EquipObj->ItemInfo.DataAsset);

	if (EquipObj && CPDA)
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

		//�����Ҷ��� ������ ItemRotate�� false��Ų��.
		EquipObj->bTempRotate = false;
		EquipObj->bRotated = false;

		EquipObj->Equipment = WantToEquip;
		EquipObj->MotherEquipComp = this;

		//RifleType�̸� RifleAssign�� �������ش�. //for QuickSlot & EquipWidget
		if (CPDA->EquipmentType == EEquipmentType::EET_Rifle || CPDA->EquipmentType == EEquipmentType::EET_Pistol)
		{	
			EquipObj->RifleAssign = Cast<AWeapon>(WantToEquip)->RifleAssign;
			SetWeaponPartsManager(WantToEquip, EquipObj);
			OnWeaponSetSlot.Broadcast(EquipObj);
		}
		
		//Backpack, Vest�� ���� ���θ� Ȯ���Ѵ�., Enum Range iter�� ���� Obj�� �����صд�.
		if(CPDA->EquipmentType == EEquipmentType::EET_Backpack)
		{
			BackpackObj = EquipObj;
		}
		else if(CPDA->EquipmentType == EEquipmentType::EET_Vest)
		{
			VestObj = EquipObj;
		}

		EquipmentItems.Add(EquipObj);

		bReturn = true;
		

		//EquipWidget::RefreshEquipWidget�� bind������.
		OnEquipmentUpdated.Broadcast();
		UE_LOG(LogTemp, Warning, TEXT("EquipComp : AddSuccess"));

	}
	return bReturn;
}

bool UEquipmentComponent::RemoveEquipment(UNewItemObject* EquipObj)
{
	if (EquipObj)
	{
		/* EquipSlot::Ondrop���� ���� SpawnActor�� ��ƾ�� ����ʿ� ����
		 * RemoveSingle�ÿ� ���� �ʰ� ��.
		 * ���� remove�� �����ؾߵ�.
		 */

		UCustomPDA* OuterCPDA = Cast<UCustomPDA>(EquipObj->ItemInfo.DataAsset);
		UNewItemObject* InnerEquipment = GetEquippedWeaponSameType(OuterCPDA->EquipmentType, EquipObj);// ->ItemInfo);

		if (InnerEquipment)
		{
			UCustomPDA* InnerCPDA = Cast<UCustomPDA>(InnerEquipment->ItemInfo.DataAsset);
			//�̸��� ���ؼ� ���� �̸��� �����ִ� ��ü�� EquipmentItems Array���� �����ϰ�
			// Equip Object�� Destory���ش�.

			if (InnerCPDA->ItemName.EqualTo(OuterCPDA->ItemName))
			{
				//BackPack,Vest�� ��� Set�� Obj�� �����ش�.
				if (InnerCPDA->EquipmentType == EEquipmentType::EET_Backpack)
				{
					BackpackObj = nullptr;			
				}
				else if (InnerCPDA->EquipmentType == EEquipmentType::EET_Vest)
				{
					VestObj = nullptr;
				}

				//for QuickSlot
				if (InnerCPDA->EquipmentType == EEquipmentType::EET_Rifle || InnerCPDA->EquipmentType == EEquipmentType::EET_Pistol)
				{
					OnWeaponRemoveSlot.Broadcast(EquipObj->RifleAssign);
					EquipObj->RifleAssign = ERifleSlot::ERS_MAX;
					
				}

				EquipmentItems.RemoveSingle(InnerEquipment);

				if (EquipObj->SettedSlot)
				{
					EquipObj->RemoveLinkSlot();				
				}

				//Weapon::Remove���� OwningPlayer�� ����ϱ� ������ ������ �̷��� �Ѵ�.
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

/* EqiupType �Ǵ� Obj�� ���� Type�� EquipObj�� �����Ѵ�.
*/
UNewItemObject* UEquipmentComponent::GetEquippedWeaponSameType(EEquipmentType EquipType, UNewItemObject* Object, ERifleSlot RifleSlot)
{
	for (auto& Equipped : EquipmentItems)
	{
		if (Equipped)
		{
			UCustomPDA* CPDA = Cast<UCustomPDA>(Equipped->ItemInfo.DataAsset);
			UCustomPDA* ObjCPDA = Cast<UCustomPDA>(Object->ItemInfo.DataAsset);
			
			if(!CPDA || !ObjCPDA) continue;


			//Rifle�� ��쿡 RiffleAssign�� ���� �����´�. �Ǵ� ItemSetting�� Type�� Rifle�̸鼭 Rifle Slot�� ������ ���.
			//Rifle Ÿ���� ��� Primary Rifle, Sub Rifle �̷��� �����Ǿ� ���� �� �ֱ� ������.
			if (EquipType == EEquipmentType::EET_Rifle)
			{
				
				if (CPDA && CPDA->EquipmentType != EEquipmentType::EET_Rifle) continue;

				if (Object != nullptr && ObjCPDA && ObjCPDA->EquipmentType == EEquipmentType::EET_Rifle)
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
			else
			{
				//World�� ������ Equipment�� ��� ���� ����Ѵ�.
				if (EquipType == EEquipmentType::EET_MAX && CPDA)
				{
					if (Object != nullptr && CPDA->EquipmentType == ObjCPDA->EquipmentType)
					{
						return Equipped;
					}
				}
				else //Equip���� EquipType���θ� �Ѱ�������
				{
					if (CPDA->EquipmentType == EquipType)
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

	for (auto& Equipped : EquipmentItems)
	{
		if (Equipped)
		{
			UCustomPDA* Var_CPDA = Cast<UCustomPDA>(Equip->ItemSetting.DataAsset);
			UCustomPDA* Inner_CPDA = Cast<UCustomPDA>(Equipped->ItemInfo.DataAsset);

			//�����Ϸ��� EquipType�� Weapon�̰� ������ Type�� Weapon�� ��� ���� ���Ѵ�.
			if (Inner_CPDA->EquipmentType == EEquipmentType::EET_Rifle && Var_CPDA->EquipmentType == EEquipmentType::EET_Rifle)
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
					if (Equipped->RifleAssign == RifleSlot)
					{
						return true;
					}
				}
			}
			else if(Inner_CPDA->EquipmentType == Var_CPDA->EquipmentType) //�Ķ���� Weapon�� Type�� �̹� ������ true
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


///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////

void UEquipmentComponent::SetWeaponPartsManager(AEquipment* WantToEquip, UNewItemObject* WeaponObj)
{
	if (UWeaponPDA* WeaponPDA = Cast<UWeaponPDA>(WeaponObj->ItemInfo.DataAsset))
	{
		if (WeaponObj->WeaponPartsManager.IsValid() == false)
		{
			WeaponObj->WeaponPartsManager = NewObject<UWeaponPartsManagerObject>(UWeaponPartsManagerObject::StaticClass());
		}
		WeaponObj->WeaponPartsManager = Cast<AWeapon>(WantToEquip)->WeaponPartsManager;
	}	
}