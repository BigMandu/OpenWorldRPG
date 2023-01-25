// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/QuickSlotSlotWidget.h"
#include "OpenWorldRPG/UI/QuickSlotWidget.h"
#include "OpenWorldRPG/MainHud.h"
#include "OpenWorldRPG/MainController.h"
#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/CustomDDOperation.h"
#include "OpenWorldRPG/NewInventory/Widget/NewItemwidget.h"

#include "Components/Border.h"



void UQuickSlotSlotWidget::RemoveMountedObj()
{
	MountedItemObj = nullptr;
	SlotBorder->ClearChildren();
}


void UQuickSlotSlotWidget::SetWeaponQuickSlot(UNewItemObject* WantToSlot)
{
	if(WItemWidget == nullptr) return;
	UNewItemwidget* ItemWidget = CreateWidget<UNewItemwidget>(this, WItemWidget);
	if(ItemWidget == nullptr) return;

	SlotBorder->ClearChildren();
	ItemWidget->Tilesize = 40.f;
	ItemWidget->ItemObj = WantToSlot;
	ItemWidget->Refresh();

	SlotBorder->AddChild(ItemWidget);
}

/**��� ���θ� üũ�� �ڿ�
* ����� �����Ѵ�.
* QuickSlot Ư����, ���� ���Կ� ����� ��û�ϸ� ������ �ִ� Item������ ���� �ǵ��� �Ѵ�.
* 
*/
void UQuickSlotSlotWidget::RegisterQuickSlot(UNewItemObject* WantToSlot)
{
	EItemType ObjItemType = WantToSlot->ItemInfo.DataAsset->ItemType;	
	
	if(WItemWidget)
	{		
		UNewItemwidget* ItemWidget = CreateWidget<UNewItemwidget>(this, WItemWidget);

		Initialize();
		SlotBorder->ClearChildren();
			
		SetMountedObject(WantToSlot);
		WantToSlot->bIsRegQuickSlot = true;

		ItemWidget->Tilesize = 70.f;
		ItemWidget->ItemObj = WantToSlot;
		ItemWidget->Refresh();

		SlotBorder->AddChild(ItemWidget);
		
	}
}

//OnDrop�ÿ� QuickSlotWidget���� ��� process�� �����Ѵ�.
bool UQuickSlotSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bReturn = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	
	UCustomDDOperation* DDOper = Cast<UCustomDDOperation>(InOperation);
	if(DDOper && DDOper->ItemObj)
	{
		AMainController* PlayerCon = Cast<AMainController>(GetOwningPlayer());
		if(PlayerCon)
		{
			PlayerCon->MainHud->QuickSlot->SetItemInQuickSlot(QuickSlotNumber,DDOper->ItemObj);
		}
	}

	bReturn = false;
	return bReturn;
}


