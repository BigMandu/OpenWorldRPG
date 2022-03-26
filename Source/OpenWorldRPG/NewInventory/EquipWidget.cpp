// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/EquipWidget.h"
#include "OpenWorldRPG/NewInventory/EquipmentSlot.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/EquipmentComponent.h"
#include "Blueprint/DragDropOperation.h"

void UEquipWidget::EquipInitialize(UEquipmentComponent* p_EquipComp)
{
	EquipComp = p_EquipComp;

	RefreshEquipWidget();
}

void UEquipWidget::RefreshEquipWidget()
{
	if (EquipComp)
	{
		for (auto ele : EquipComp->EquipmentItems)
		{
			switch (ele->EquipmentType)
			{
			case EEquipmentType::EET_Helmet:
				
				break;
			case EEquipmentType::EET_Plate:
				break;
			case EEquipmentType::EET_Pistol:
				break;
			case EEquipmentType::EET_Rifle:
				break;
			case EEquipmentType::EET_Vest:
				break;
			case EEquipmentType::EET_Backpack:
				break;

			}
		}
	}
}

//bool UEquipWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
//{
//	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
//
//	UNewItemObject* ItemObj = Cast<UNewItemObject>(InOperation->Payload);
//	if (ItemObj)
//	{
//		if (ItemObj->bCanEquip)
//		{
//			if (ItemObj->EquipmentType == EEquipmentType::EET_Vest)
//			{
//				UE_LOG(LogTemp, Warning, TEXT(" Correct Slot, Can Equip "));
//			}
//			else
//			{
//				UE_LOG(LogTemp, Warning, TEXT(" Incorrect Slot "));
//			}
//		}
//	}
//	return true;
//}