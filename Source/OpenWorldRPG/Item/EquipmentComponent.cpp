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
	if (Equip)
	{
		if (Equip->OwningInventory != nullptr) //인벤토리에 있던 Weapon이면,
		{
			Equip->OwningInventory->RemoveItem(Cast<AItem>(this)); //인벤토리에서 지워준다.
		}

		Equip->OwningEquipment = this;
		EquipmentItems.Add(Equip);
		
		OnEquipmentUpdated.Broadcast();

		UE_LOG(LogTemp, Warning, TEXT("EquipComp : AddSuccess"));
		return true;
	}
	return false;
}

bool UEquipmentComponent::RemoveEquipment(AEquipment* Equip)
{
	if (Equip)
	{
		EquipmentItems.RemoveSingle(Equip);
		OnEquipmentUpdated.Broadcast();
		return true;
	}
	return false;
}

bool UEquipmentComponent::IsWeaponExist(AEquipment* Equip)
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

AEquipment* UEquipmentComponent::GetBeforeWeapon(AEquipment* Equip)
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