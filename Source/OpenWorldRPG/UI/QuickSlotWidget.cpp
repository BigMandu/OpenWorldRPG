// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/QuickSlotWidget.h"
#include "OpenWorldRPG/UI/QuickSlotSlotWidget.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"


void UQuickSlotWidget::BindSlotWidget(class AMainCharacter* Player)
{
	
	Player->OnQuickSlotUse.AddDynamic(this, &UQuickSlotWidget::UseItemInQuickSlot);
	
}


void UQuickSlotWidget::SetItemInQuickSlot(EQuickSlotNumber QuickSlotNum, UNewItemObject* ItemObj)
{
	if (ItemObj)
	{
		if(CheckCanBeRegister(ItemObj))
		{
			CheckAlreadyRegistered(ItemObj);
			switch (QuickSlotNum)
			{
			case EQuickSlotNumber::EQSN_N4:
				QSlot_4->RegisterQuickSlot(ItemObj);
				break;
			case EQuickSlotNumber::EQSN_N5:
				QSlot_5->RegisterQuickSlot(ItemObj);
				break;
			case EQuickSlotNumber::EQSN_N6:
				QSlot_6->RegisterQuickSlot(ItemObj);
				break;
			case EQuickSlotNumber::EQSN_N7:
				QSlot_7->RegisterQuickSlot(ItemObj);
				break;
			case EQuickSlotNumber::EQSN_N8:
				QSlot_8->RegisterQuickSlot(ItemObj);
				break;
			case EQuickSlotNumber::EQSN_N9:
				QSlot_9->RegisterQuickSlot(ItemObj);
				break;
			}
		}
	}
}

UQuickSlotSlotWidget* UQuickSlotWidget::GetQuickSlotSlot(EQuickSlotNumber QuickSlotNum)
{
	//UNewItemObject* ReturnObj = nullptr;
	switch (QuickSlotNum)
	{
	case EQuickSlotNumber::EQSN_N4:
		return QSlot_4;
	break;
	case EQuickSlotNumber::EQSN_N5:
		return QSlot_5;
	break;
	case EQuickSlotNumber::EQSN_N6:
		return QSlot_6;
	break;
	case EQuickSlotNumber::EQSN_N7:
		return QSlot_7;
	break;
	case EQuickSlotNumber::EQSN_N8:
		return QSlot_8;
	break;
	case EQuickSlotNumber::EQSN_N9:
		return QSlot_9;
	break;
	}

	return nullptr;
}


/** QuickSlot�� ��� �� �� �ִ��� �Ǵ��Ѵ�.
 * ItemObj�� ��� Storage (�θ� storage)��
 * Pocket�̳�, Vest Storage�϶� �� Quick Slot�� ����� �����ϴ�.
 */

bool UQuickSlotWidget::CheckCanBeRegister(UNewItemObject* WantToSlot)
{

	AMainCharacter* Player = Cast<AMainCharacter>(GetOwningPlayerPawn());

	auto VestObj = Player->Equipment->GetEquipStorage(EEquipmentType::EET_Vest);

	bool bCanbeSet = false;
	if (WantToSlot->MotherStorage == Player->PocketStorage)
	{
		bCanbeSet = true;
	}
	else if (VestObj)
	{
		auto VestStorageObj = Cast<UItemStorageObject>(VestObj);
		if (WantToSlot->MotherStorage == VestStorageObj)
		{
			bCanbeSet = true;
		}
	}

	return bCanbeSet;
}


/* 
* UNewInventoryGrid::RefreshInventory���� 
* ItemObject�� bIsRegQuickSlot�� True�� �Ʒ� �Լ��� ȣ��ȴ�.
* 
* 
*/
void UQuickSlotWidget::JudgeCanbeInQuickSlotOrUpdate(UNewItemObject* ItemObj)
{
	for (EQuickSlotNumber QuickNum : TEnumRange<EQuickSlotNumber>())
	{
		UQuickSlotSlotWidget* QuickSlot = GetQuickSlotSlot(QuickNum);
		if (ItemObj == QuickSlot->GetMountedObject())
		{
			//��� �� �� ���� (Pocket || Vest�� ���� �ʴ�) �����
			//QuickSlot���� ��� �����Ѵ�.
			if (CheckCanBeRegister(ItemObj) == false)
			{
				QuickSlot->RemoveMountedObj();
				return;
			}
			else
			{
				//��� ������ ���¸� Item��Ȳ�� Update�Ѵ�. (������ ����� ����)
				SetItemInQuickSlot(QuickSlot->QuickSlotNumber,ItemObj);
			}
		}
	}
}

/*
* �̹� ��� �Ǿ��ִ��� Ȯ���ϰ�
* �̹� ����� �Ǿ� ������, ��ϵ� Slot���� ���� ��Ų��.
*/
void UQuickSlotWidget::CheckAlreadyRegistered(UNewItemObject* ItemObj)
{
	for (EQuickSlotNumber QuickNum : TEnumRange<EQuickSlotNumber>())
	{
		UQuickSlotSlotWidget* QuickSlot = GetQuickSlotSlot(QuickNum);
		if (QuickSlot && ItemObj == QuickSlot->GetMountedObject())
		{
			QuickSlot->RemoveMountedObj();
		}
	}
}

void UQuickSlotWidget::UseItemInQuickSlot(EQuickSlotNumber QuickSlotNum)
{
	UNewItemObject* WantToUse = GetQuickSlotSlot(QuickSlotNum)->GetMountedObject();
	if(WantToUse)
	{
		UWorld* World = GetWorld();
		if (World)
		{
			WantToUse->UseItem(World);
		}
		
	}
}