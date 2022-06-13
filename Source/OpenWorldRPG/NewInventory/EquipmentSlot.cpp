// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/EquipmentSlot.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/BaseCharacter.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Blueprint/DragDropOperation.h"

bool UEquipmentSlot::IsSupportedEquip(UNewItemObject* Obj)
{
	bool bReturn = false;

	if (Obj->InteractType == EInteractType::EIT_Equipment && Obj->EquipmentType == SlotType)
	{
		bReturn = true;	
	}
	return bReturn;
}

void UEquipmentSlot::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	UNewItemObject* ItemObj = Cast< UNewItemObject>(InOperation->Payload);
	if (ItemObj)
	{
		PaintBGBorder(ItemObj);
		
	}

}
void UEquipmentSlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	PaintBGBorder();
	
}


void UEquipmentSlot::PaintBGBorder(UNewItemObject* Obj)
{
	FLinearColor Red = FLinearColor(1.f, 0.f, 0.f, 0.25f);
	FLinearColor Green = FLinearColor(0.f, 1.f, 0.f, 0.25f);
	FLinearColor Black = FLinearColor(0.f, 0.f, 0.f, 0.25f);
	if (Obj != nullptr)
	{
		if (BGBorder)
		{
			if (IsSupportedEquip(Obj))
			{
				BGBorder->SetBrushColor(Green);
				bCanDrop = true;
			}
			else
			{
				BGBorder->SetBrushColor(Red);
				bCanDrop = false;
			}
		}
	}
	else
	{	
		BGBorder->SetBrushColor(Black);
		bCanDrop = false;
	}

	UE_LOG(LogTemp, Warning, TEXT("UEquipmentSlot bCanDrop = %s"), bCanDrop ? "true" : "false");
}

bool UEquipmentSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	
	UNewItemObject* ItemObj = Cast< UNewItemObject>(InOperation->Payload);
	if (ItemObj)
	{
		if (IsSupportedEquip(ItemObj))
		{
			UE_LOG(LogTemp, Warning, TEXT("Correct Slot"));

			check(ItemObj->item);
			
			//GetOwningPlayerPawn으로 가져오면, Player가 된다. Widget의 소유권을 확실히 해야됨
			ABaseCharacter* BChar;
			if (LootedChar_Owner != nullptr)
			{
				BChar = LootedChar_Owner;
			}
			else
			{
				BChar = Cast<ABaseCharacter>(GetOwningPlayerPawn());
			}

			if (BChar != nullptr)
			{
				if (ItemObj->bIsDestoryed)
				{
					AEquipment* Equipment = Cast<AEquipment>(GetWorld()->SpawnActor<AActor>(ItemObj->GetItemClass()));
					if (Equipment)
					{
						ItemObj->bIsDestoryed = false;
						Equipment->ReInitialize(ItemObj);
						Equipment->SetItemState(EItemState::EIS_Pickup);
						ItemObj->MotherContainer = nullptr;
						Equipment->StepEquip(BChar);
					}
				}
				else
				{
					AEquipment* Equipment = Cast<AEquipment>(ItemObj->item);
					if (Equipment)
					{
						Equipment->StepEquip(BChar);
					}
				}
			}

			//old version

			/*
			AMainCharacter* Main = Cast<AMainCharacter>(GetOwningPlayerPawn());
			AEquipment* Equipment = Cast<AEquipment>(ItemObj->item);
			
			if (Equipment)// && ItemObj->GetMotherContainer() != nullptr) //-> 아이템을 뺏다가 바로 끼는 경우가 있을때가 있기 때문에 조건에서 뺌.
			{
				if(SlotType == EEquipmentType::EET_Rifle)
				{
					AWeapon* Weapon = Cast<AWeapon>(Equipment);
					if (Weapon)
					{
						if (RifleSlotType == ERifleSlot::ERS_Primary)
						{
							Weapon->RifleAssign = ERifleAssign::ERA_Primary;
							Main->PrimaryWeapon = Weapon;	
						}
						else if (RifleSlotType == ERifleSlot::ERS_Sub)
						{
							Weapon->RifleAssign = ERifleAssign::ERA_Sub;
							Main->SubWeapon = Weapon;
						}
					}
				}

				if(ItemObj->bIsDestoryed)
				{
					AEquipment* T_Equipment = Cast<AEquipment>(GetWorld()->SpawnActor<AActor>(ItemObj->GetItemClass()));
					if (T_Equipment)
					{
						if(T_Equipment->EquipmentType == EEquipmentType::EET_Rifle)
						{
							AWeapon* T_Weapon = Cast<AWeapon>(T_Equipment);
							if(T_Weapon)
							{
								T_Weapon->RifleAssign = Weapon->RifleAssign;
							}
						}


						ItemObj->bIsDestoryed = false;
						//T_Equipment = Equipment;
						if (Equipment->bHasStorage)
						{
							T_Equipment->EquipInventoryComp = Equipment->EquipInventoryComp;
						}
						T_Equipment->ReInitialize(ItemObj);

						Equipment = T_Equipment; //error
						ItemObj->item = Equipment;

						Equipment->SetItemState(EItemState::EIS_Pickup);						
					}
				}
				
				ItemObj->MotherContainer = nullptr;
				*/

				//아래 /**/ 주석은 안쓰는부분.
				/*AWeapon* T_Weapon = Cast<AWeapon>(Equipment);
				if (T_Weapon)
				{
					T_Weapon->RifleAssign = Weapon->RifleAssign;
					T_Weapon->StepEquip(Main);
				}*/
				//else
				//{
					 //Weapon으로 cast, equip함수 호출
				//}
			//}			
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Incorrect Slot"));
		}
	}
	

	return true;
}
