// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/EquipmentSlot.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/MainCharacter.h"
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
	if (Obj != nullptr)
	{
		FLinearColor Green = FLinearColor(0.f, 1.f, 0.f, 0.25f);
		FLinearColor Red = FLinearColor(1.f, 0.f, 0.f, 0.25f);
		
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
		FLinearColor Black = FLinearColor(0.f, 0.f, 0.f, 0.25f);
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
			AMainCharacter* Main = Cast<AMainCharacter>(GetOwningPlayerPawn());
			AEquipment* Equipment = Cast<AEquipment>(ItemObj->item);
			AWeapon* Weapon = Cast<AWeapon>(Equipment);

			if (Equipment)// && ItemObj->GetMotherContainer() != nullptr) //-> 아이템을 뺏다가 바로 끼는 경우가 있을때가 있기 때문에 조건에서 뺌.
			{
				if(SlotType == EEquipmentType::EET_Rifle)
				{
					if (Weapon)
					{
						if (RifleSlotType == ERifleSlot::ERS_Primary)
						{
							if (Weapon)
							{
								Weapon->RifleAssign = ERifleAssign::ERA_Primary;
								Main->PrimaryWeapon = Weapon;
							}
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
					/* //여기를 함수화 해봤는데 안된다.
					 *
					 
					//AMainCharacter* Main = Cast<AMainCharacter>(GetOwningPlayerPawn());
					//AEquipment* T_Equipment = Equipment->SpawnEquip(ItemObj, Main);
					//if (T_Equipment != nullptr)
					//{

					//	ItemObj->MotherContainer = nullptr;

					//	Equipment = T_Equipment;
					//	//T_Equipment->StepEquip(Main); //Weapon으로 cast, equip함수 호출

					//	//return true;
					//}
					*/

					AEquipment* T_Equipment = Cast<AEquipment>(GetWorld()->SpawnActor<AActor>(ItemObj->GetItemClass()));
					if (T_Equipment)
					{
						ItemObj->bIsDestoryed = false;
						//T_Equipment = Equipment;
						if (Equipment->bHasStorage)
						{
							T_Equipment->EquipInventoryComp = Equipment->EquipInventoryComp;
						}

						T_Equipment->ReInitialize(ItemObj);


						Equipment = T_Equipment;
						ItemObj->item = Equipment;

						Equipment->SetItemState(EItemState::EIS_Pickup);
						
						
					}
					
				}
				
				ItemObj->MotherContainer = nullptr;

				AWeapon* T_Weapon = Cast<AWeapon>(Equipment);
				if (T_Weapon)
				{
					T_Weapon->RifleAssign = Weapon->RifleAssign;
					T_Weapon->StepEquip(Main);
				}
				else
				{
					Equipment->StepEquip(Main); //Weapon으로 cast, equip함수 호출
				}
			}			
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Incorrect Slot"));
		}
	}
	

	return true;
}
