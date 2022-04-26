// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/EquipWidget.h"
#include "OpenWorldRPG/NewInventory/EquipmentSlot.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/NewItemwidget.h"
#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/Item/EquipmentComponent.h"

#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
//#include "Components/Border.h"
//#include "Components/CanvasPanelSlot.h"
//#include "Components/CanvasPanel.h"
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
	RemoveSlot();
	if (EquipComp)
	{
		for (auto ele : EquipComp->EquipmentItems)
		{
			if (ele != nullptr)
			{
				switch (ele->EquipmentType)
				{
				case EEquipmentType::EET_Helmet:
					SetSlot(ele, HelmetSlot);
					break;
				case EEquipmentType::EET_Plate:
					SetSlot(ele, PlateSlot);
					break;
				case EEquipmentType::EET_Pistol:
					SetSlot(ele, PistolSlot);
					break;
				case EEquipmentType::EET_Rifle:
					{
					AWeapon* Weapon = Cast<AWeapon>(ele);
						if(Weapon)
						{
							switch(Weapon->RifleAssign)
							{
							case ERifleAssign::ERA_Primary:
								SetSlot(ele, PrimarySlot);
								break;
							case ERifleAssign::ERA_Sub:
								SetSlot(ele, SubSlot);
								break;
							}
						}
					}
					
					break;
				case EEquipmentType::EET_Vest:
					SetSlot(ele, VestSlot);
					if (VestOverlay && ele->bHasStorage && ele->EquipGridWidget)
					{ 
						VestOverlay->AddChild(ele->EquipGridWidget);//���� error
					}
					break;
				case EEquipmentType::EET_Backpack:
					SetSlot(ele, BackpackSlot);
					if (BackpackOverlay && ele->bHasStorage && ele->EquipGridWidget)
					{
						BackpackOverlay->AddChild(ele->EquipGridWidget);
					}
					break;

				}
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
			ItemWidget->OnRemoved.AddUFunction(this, FName("RemoveEquipment"));
			EquipSlot->BGBorder->ClearChildren();
			//VestOverlay->ClearChildren();
			

			ItemWidget->Tilesize = EquipSlot->GetDesiredSize().X;
			ItemWidget->ItemObj = Equip->ItemObj; // ele.Key;
			ItemWidget->Refresh();
			
			EquipSlot->BGBorder->AddChild(ItemWidget);
			EquipSlot->PaintBGBorder();

		}
	}
}

void UEquipWidget::RemoveEquipment(UObject* T_ItemObj)
{
	UNewItemObject* ItemObj = Cast<UNewItemObject>(T_ItemObj);
	if (ItemObj && EquipComp)
	{
		AEquipment* Equipment = Cast<AEquipment>(ItemObj->item);
		if(Equipment)
		{
			
			Equipment->Remove();
			EquipComp->RemoveEquipment(Equipment);
			
			ItemObj->bIsDestoryed = true;
			//Equipment->Destroy();
		}
	}
}

void UEquipWidget::RemoveSlot()
{
	VestOverlay->ClearChildren();
	BackpackOverlay->ClearChildren();
	PrimarySlot->BGBorder->ClearChildren();
	SubSlot->BGBorder->ClearChildren();
}
