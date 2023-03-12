// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/QuickSlotWidget.h"
#include "OpenWorldRPG/UI/QuickSlotSlotWidget.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"


void UQuickSlotWidget::BindSlotWidget(class AMainCharacter* Player)
{
	Player->OnQuickSlotUse.AddDynamic(this, &UQuickSlotWidget::UseItemInQuickSlot);
	Player->Equipment->OnWeaponSetSlot.AddDynamic(this,&UQuickSlotWidget::SetWeaponQuickSlot);
	Player->Equipment->OnWeaponRemoveSlot.AddDynamic(this,&UQuickSlotWidget::RemoveWeaponQuickSlot);
}

/* Ư�� Slot �ϳ��� Register�ϴ� ����̴�.  (Inventory, Equipwidget ���� �ٸ� ������ <- ����� ��ü refresh) */
void UQuickSlotWidget::SetItemInQuickSlot(EQuickSlotNumber QuickSlotNum, UNewItemObject* ItemObj, bool bIsNeedOnlyDeleted)
{
	if (ItemObj)
	{
		if(CheckCanBeRegister(ItemObj))
		{
			//���� Quickslot�� ���� item�� ��쿡�� false�� �����Ѵ�.
			if(CheckAlreadyRegistered(ItemObj, bIsNeedOnlyDeleted, QuickSlotNum))
			{
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

	bool bIsInPocketOrVest = false;
	bool bCanRegist = false;


	if (WantToSlot->MotherStorage == Player->PocketStorage)
	{
		bIsInPocketOrVest = true;
	}
	else if (VestObj)
	{
		auto VestStorageObj = Cast<UItemStorageObject>(VestObj);
		if (WantToSlot->MotherStorage == VestStorageObj)
		{
			bIsInPocketOrVest = true;
		}
	}

	//Pocket�̳� Vest�� �ִ� ��� ItemType�� �����Ѵ�.
	if (bIsInPocketOrVest)
	{
		EItemType ObjItemType = WantToSlot->ItemInfo.DataAsset->ItemType;
		UGrenadePDA* GrenadeDA = Cast<UGrenadePDA>(WantToSlot->ItemInfo.DataAsset);
		if (ObjItemType == EItemType::EIT_Food || ObjItemType == EItemType::EIT_Medical || ObjItemType == EItemType::EIT_Usable)
		{
			bCanRegist = true;
		}
		else if (GrenadeDA)
		{
			bCanRegist = true;
		}
	}


	return bCanRegist;
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
				SetItemInQuickSlot(QuickSlot->QuickSlotNumber,ItemObj, false);
			}
		}
	}
}

/*
* �̹� ��� �Ǿ��ִ��� Ȯ���ϰ�
* �̹� ����� �Ǿ� ������, ��ϵ� Slot���� ���� ��Ų��.
*/
bool UQuickSlotWidget::CheckAlreadyRegistered(UNewItemObject* ItemObj, bool bIsOnlyDeleted, EQuickSlotNumber WantQuickNum)
{
	for (EQuickSlotNumber QuickNum : TEnumRange<EQuickSlotNumber>())
	{
		UQuickSlotSlotWidget* QuickSlot = GetQuickSlotSlot(QuickNum);

		//QuickSlot�� ��ϵ� Obj�� ���ٸ� ��ϵǾ��ִ� QuickSlot���� �����Ѵ�.
		if (QuickSlot && ItemObj == QuickSlot->GetMountedObject())
		{
			QuickSlot->RemoveMountedObj();

			/**�ٵ� ����, �ش� QuickSlot Number�� ��ϵ� Item�� �ߺ� �Է��ϸ� 
			* ���� ���μ����� ���� �ϱ����� false�� �����ϰ� ��ģ��.
			*/
			if (bIsOnlyDeleted && WantQuickNum != EQuickSlotNumber::EQSN_MAX && QuickNum == WantQuickNum)
			{
				return false;
			}
		}
	}
	return true;
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


void UQuickSlotWidget::SetWeaponQuickSlot(UNewItemObject* WantToSet)
{
	switch (WantToSet->RifleAssign)
	{
	case ERifleSlot::ERS_Primary:
		QSlot_Primary->SetWeaponQuickSlot(WantToSet);
	break;
	case ERifleSlot::ERS_Sub:
		QSlot_Sub->SetWeaponQuickSlot(WantToSet);
	break;
	case ERifleSlot::ERS_Pistol:
		QSlot_Pistol->SetWeaponQuickSlot(WantToSet);
	break;
	}
}

void UQuickSlotWidget::RemoveWeaponQuickSlot(ERifleSlot WantToRemoveSlot)
{
	switch (WantToRemoveSlot)
	{
	case ERifleSlot::ERS_Primary:
		QSlot_Primary->RemoveMountedObj();
	break;
	case ERifleSlot::ERS_Sub:
		QSlot_Sub->RemoveMountedObj();
	break;
	case ERifleSlot::ERS_Pistol:
		QSlot_Pistol->RemoveMountedObj();
	break;
	}
}