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
		if (Weapon->OwningInventory != nullptr) //�κ��丮�� �ִ� Weapon�̸�,
		{
			Weapon->OwningInventory->RemoveItem(Cast<AItem>(this)); //�κ��丮���� �����ش�.
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
			//Rifle Type�� �ֹ���, �ι��� �ΰ��� ������ �� �ִ�.
			if (Equipped->WeaponType == EWeaponType::EWT_Rifle && Weapon->WeaponType == EWeaponType::EWT_Rifle)
			{
				RifleCnt++;
			}
			else if(Equipped->WeaponType == Weapon->WeaponType) //�Ķ���� Weapon�� Type�� �̹� ������ true
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