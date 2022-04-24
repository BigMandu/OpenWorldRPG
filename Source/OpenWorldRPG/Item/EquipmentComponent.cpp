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
		//if (Equip->OwningInventory != nullptr) //�κ��丮�� �ִ� Weapon�̸�,
		//{
		//	Equip->OwningInventory->RemoveItem(Cast<AItem>(this)); //�κ��丮���� �����ش�.
		//}

		/* ������ Equip�� �̹� �ִ��� Ȯ���Ѵ�. Swap�϶� AddEquip�� �ϱ� ������*/
		for(auto& EquippedItem : EquipmentItems)
		{
			if(EquippedItem == Equip)
			{
				bAlreadyHave = true;
				break;
			}
		}

		/* ���� Equip�� ���ٸ� �߰���Ų��. */
		if (bAlreadyHave == false)
		{
			Equip->OwningEquipment = this;
			EquipmentItems.Add(Equip);
			Equip->SKMesh->SetHiddenInGame(false); //�ӽ÷� �صа���.

			//EquipWidget::RefreshEquipWidget�� bind��Ŵ.

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
			//Rifle Type�� �ֹ���, �ι��� �ΰ��� ������ �� �ִ�.
			if (Equipped->EquipmentType == EEquipmentType::EET_Rifle && Equip->EquipmentType == EEquipmentType::EET_Rifle)
			{
				RifleCnt++;
			}
			else if(Equipped->EquipmentType == Equip->EquipmentType) //�Ķ���� Weapon�� Type�� �̹� ������ true
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