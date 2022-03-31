// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/EquipmentSlot.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
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
	PaintBGBorder(nullptr);
	
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
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Incorrect Slot"));
		}
	}
	

	return true;
}
