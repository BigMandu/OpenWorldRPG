// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/Item/EquipmentComponent.h"
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
	if (Equip)
	{
		//if (Equip->OwningInventory != nullptr) //인벤토리에 있던 Weapon이면,
		//{
		//	Equip->OwningInventory->RemoveItem(Cast<AItem>(this)); //인벤토리에서 지워준다.
		//}

		/* 동일한 Equip이 이미 있는지 확인한다. Swap일때 AddEquip을 하기 때문에*/
		for(auto& EquippedItem : EquipmentItems)
		{
			if(EquippedItem == Equip)
			{
				bAlreadyHave = true;
				break;
			}
		}

		/* 동일 Equip이 없다면 추가시킨다. */
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

	}
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
		if (Equip->EquipmentType == EEquipmentType::EET_Backpack)
		{
			bHasBackpack = false;
		}
		else if (Equip->EquipmentType == EEquipmentType::EET_Vest)
		{
			bHasVest = false;
		}

		AEquipment* InnerEquipment = GetEquippedWeaponSameType(Equip->EquipmentType);
		if (InnerEquipment)
		{
			//이름을 비교해서 같은 이름을 갖고있는 객체를 EquipmentItems Array에서 삭제하고
			// Equip Object를 Destory해준다.
			if (InnerEquipment->ItemName.EqualTo(Equip->ItemName))
			{
				EquipmentItems.RemoveSingle(InnerEquipment);

				Equip->OwningPlayer = nullptr;
				InnerEquipment->OwningPlayer = nullptr;
				Equip->Destroy();
				InnerEquipment->Destroy();

				OnEquipmentUpdated.Broadcast();

				return true;
			}
		}
	}
	return false;
}

bool UEquipmentComponent::IsSameTypeExist(AEquipment* Equip, ERifleSlot RifleSlot)
{
	int32 RifleCnt = 0;

	for (auto& EquipItem : EquipmentItems)
	{
		AEquipment* Equipped = Cast<AEquipment>(EquipItem);
		if (Equipped)
		{
			//Rifle Type은 주무기, 부무기 두개를 장착할 수 있다.
			if (Equipped->EquipmentType == EEquipmentType::EET_Rifle && Equip->EquipmentType == EEquipmentType::EET_Rifle)
			{
				if (RifleSlot == ERifleSlot::ERS_MAX)
				{
					RifleCnt++;
				}
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
			else if(Equipped->EquipmentType == Equip->EquipmentType) //파라미터 Weapon의 Type이 이미 있으면 true
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

AEquipment* UEquipmentComponent::GetEquippedWeaponSameType(EEquipmentType EquipType, AEquipment* Equip, ERifleSlot RifleSlot)
{
	for (auto& EquipItem : EquipmentItems)
	{
		AEquipment* Equipped = Cast<AEquipment>(EquipItem);
		if (Equipped)
		{
			if ((EquipType == EEquipmentType::EET_Rifle || Equip->EquipmentType == EEquipmentType::EET_Rifle)
				&& RifleSlot != ERifleSlot::ERS_MAX)
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

bool UEquipmentComponent::SwapEquipment(AEquipment* Before, AEquipment* Next)
{
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
}