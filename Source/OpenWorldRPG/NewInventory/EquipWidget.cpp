// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/EquipWidget.h"
#include "OpenWorldRPG/NewInventory/EquipmentSlot.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/NewItemwidget.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/EquipmentComponent.h"

#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/CanvasPanel.h"
#include "Blueprint/DragDropOperation.h"

void UEquipWidget::EquipInitialize(UEquipmentComponent* p_EquipComp)
{
	EquipComp = p_EquipComp;
	if (EquipComp)
	{
		EquipComp->OnEquipmentUpdated.AddUFunction(this, FName("RefreshEquipWidget"));
		UE_LOG(LogTemp, Warning, TEXT("UEquipWidget:: Bind Success"));
	}

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
				UE_LOG(LogTemp, Warning, TEXT("UEquipWidget:: RefreshEquip, Set Slot call"));
				SetSlot(ele, VestSlot);
				break;
			case EEquipmentType::EET_Backpack:
				break;

			}
		}
	}
}

void UEquipWidget::SetSlot(AEquipment* Equip, UEquipmentSlot* EquipSlot)
{
	if (WNewItemWidget)
	{
		UNewItemwidget* ItemWidget = CreateWidget<UNewItemwidget>(this, WNewItemWidget);

		if (ItemWidget)
		{
			//ItemWidget->OnRemoved.AddUFunction(this, FName("OnItemRemove"));
			EquipSlot->BGBorder->ClearChildren();

			ItemWidget->Tilesize = EquipSlot->GetDesiredSize().X;
			ItemWidget->ItemObj = Equip->ItemObj; // ele.Key;
			ItemWidget->Refresh();

			EquipSlot->BGBorder->AddChild(ItemWidget);
			EquipSlot->PaintBGBorder();
			/*UPanelSlot* PanelSlot = EquipSlot->BGBorder->AddChild(ItemWidget);

			UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(PanelSlot);
			if (CanvasSlot)
			{
				CanvasSlot->SetAutoSize(true);
			}*/

		}
	}
	//EquipSlot->ItemIcon->SetBrushFromMaterial(Equip->Icon);
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