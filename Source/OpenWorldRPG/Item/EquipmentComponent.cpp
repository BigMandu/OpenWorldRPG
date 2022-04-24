// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/Item/EquipmentComponent.h"
#include "OpenWorldRPG/Item/Equipment.h"

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

			//EquipWidget::RefreshEquipWidget과 bind시킴.

			bReturn = true;
		}
			OnEquipmentUpdated.Broadcast();

			UE_LOG(LogTemp, Warning, TEXT("EquipComp : AddSuccess"));

	}
	return bReturn;
}

bool UEquipmentComponent::RemoveEquipment(AEquipment* Equip)
{
	if (Equip)
	{
		/*if (Equip->SKMesh)
		{
			Equip->SKMesh->SetHiddenInGame(true);
		}*/
		EquipmentItems.RemoveSingle(Equip);
		Equip->OwningPlayer = nullptr;
		Equip->Destroy();
		
		OnEquipmentUpdated.Broadcast();
		return true;
	}
	return false;
}

bool UEquipmentComponent::IsSameTypeExist(AEquipment* Equip)
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
				RifleCnt++;
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

AEquipment* UEquipmentComponent::GetEquippedWeaponSameType(AEquipment* Equip)
{
	for (auto& EquipItem : EquipmentItems)
	{
		AEquipment* Equipped = Cast<AEquipment>(EquipItem);
		if (Equipped)
		{
			if (Equipped->EquipmentType == Equip->EquipmentType)
			{
				return Equipped;
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