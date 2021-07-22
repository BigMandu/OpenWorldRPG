// Fill out your copyright notice in the Description page of Project Settings.


#include "EquipmentComponent.h"
#include "Weapon.h"

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

bool UEquipmentComponent::AddEquipment(AWeapon* Weapon)
{
	UE_LOG(LogTemp, Warning, TEXT("EquipComp : AddEquip"));
	if (Weapon)
	{
		if (Weapon->OwningInventory != nullptr) //인벤토리에 있던 Weapon이면,
		{
			Weapon->OwningInventory->RemoveItem(Cast<AItem>(this)); //인벤토리에서 지워준다.
		}

		Weapon->OwningEquipment = this;
		EquipmentItems.Add(Weapon);
		
		OnEquipmentUpdated.Broadcast();

		UE_LOG(LogTemp, Warning, TEXT("EquipComp : AddSuccess"));
		return true;
	}
	return false;
}

bool UEquipmentComponent::RemoveEquipment(AWeapon* Weapon)
{
	if (Weapon)
	{
		EquipmentItems.RemoveSingle(Weapon);
		OnEquipmentUpdated.Broadcast();
		return true;
	}
	return false;
}

bool UEquipmentComponent::IsWeaponExist(AWeapon* Weapon)
{
	int32 RifleCnt = 0;

	for (auto& EquipItem : EquipmentItems)
	{
		AWeapon* Equipped = Cast<AWeapon>(EquipItem);
		if (Equipped)
		{
			//Rifle Type은 주무기, 부무기 두개를 장착할 수 있다.
			if (Equipped->WeaponType == EWeaponType::EWT_Rifle && Weapon->WeaponType == EWeaponType::EWT_Rifle)
			{
				RifleCnt++;
			}
			else if(Equipped->WeaponType == Weapon->WeaponType) //파라미터 Weapon의 Type이 이미 있으면 true
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

AWeapon* UEquipmentComponent::GetBeforeWeapon(AWeapon* Weapon)
{
	for (auto& EquipItem : EquipmentItems)
	{
		AWeapon* Equipped = Cast<AWeapon>(EquipItem);
		if (Equipped)
		{
			if (Equipped->WeaponType == Weapon->WeaponType)
			{
				return Equipped;
			}
		}
	}
	return nullptr;
}