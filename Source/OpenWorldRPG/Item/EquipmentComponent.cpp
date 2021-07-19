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

