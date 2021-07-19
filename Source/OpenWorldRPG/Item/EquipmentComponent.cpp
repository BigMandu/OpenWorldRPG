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

