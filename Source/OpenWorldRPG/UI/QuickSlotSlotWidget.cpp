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

/**등록 여부를 체크한 뒤에
* 등록을 진행한다.
* QuickSlot 특성상, 동일 슬롯에 등록을 요청하면 기존에 있던 Item정보는 삭제 되도록 한다.
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

//OnDrop시에 QuickSlotWidget에서 등록 process를 진행한다.
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


