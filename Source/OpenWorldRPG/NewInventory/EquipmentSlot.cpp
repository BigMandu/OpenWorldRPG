// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/EquipmentSlot.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Blueprint/DragDropOperation.h"

void UEquipmentSlot::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	UNewItemObject* ItemObj = Cast< UNewItemObject>(InOperation->Payload);
	if (ItemObj)
	{
		PaintBGBorder(ItemObj->EquipmentType);
	}

}
void UEquipmentSlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	PaintBGBorder();
}

//bool UEquipmentSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
//{
//	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
//	
//	UNewItemObject* ItemObj = Cast< UNewItemObject>(InOperation->Payload);
//	if (ItemObj)
//	{
//		if (ItemObj->EquipmentType == SlotType)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("Correct Slot"));
//		}
//		else
//		{
//			UE_LOG(LogTemp, Warning, TEXT("Incorrect Slot"));
//		}
//	}
//	
//
//	return true;
//}


void UEquipmentSlot::PaintBGBorder(EEquipmentType EquipType)
{
	if (EquipType != EEquipmentType::EET_MAX)
	{
		FLinearColor Green = FLinearColor(0.f, 1.f, 0.f, 0.25f);
		FLinearColor Red = FLinearColor(1.f, 0.f, 0.f, 0.25f);
		
		if (BGBorder)
		{
			if (EquipType == SlotType)
			{
				BGBorder->SetBrushColor(Green);
			}
			else
			{
				BGBorder->SetBrushColor(Red);
			}
		}
	}
	else
	{
		FLinearColor Black = FLinearColor(0.f, 0.f, 0.f, 0.25f);
		BGBorder->SetBrushColor(Black);
	}
}