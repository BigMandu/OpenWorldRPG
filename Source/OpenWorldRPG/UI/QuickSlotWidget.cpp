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

/* 특정 Slot 하나씩 Register하는 방식이다.  (Inventory, Equipwidget 과는 다른 형식임 <- 얘들은 전체 refresh) */
void UQuickSlotWidget::SetItemInQuickSlot(EQuickSlotNumber QuickSlotNum, UNewItemObject* ItemObj, bool bIsNeedOnlyDeleted)
{
	if (ItemObj)
	{
		if(CheckCanBeRegister(ItemObj))
		{
			//동일 Quickslot에 동일 item인 경우에만 false를 리턴한다.
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


/** QuickSlot에 등록 할 수 있는지 판단한다.
 * ItemObj가 담긴 Storage (부모 storage)가
 * Pocket이나, Vest Storage일때 만 Quick Slot에 등록이 가능하다.
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

	//Pocket이나 Vest에 있는 경우 ItemType을 검증한다.
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
* UNewInventoryGrid::RefreshInventory에서 
* ItemObject에 bIsRegQuickSlot이 True면 아래 함수가 호출된다.
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
			//등록 할 수 없는 (Pocket || Vest에 있지 않는) 경우라면
			//QuickSlot에서 등록 해제한다.
			if (CheckCanBeRegister(ItemObj) == false)
			{
				QuickSlot->RemoveMountedObj();
				return;
			}
			else
			{
				//등록 가능한 상태면 Item상황을 Update한다. (삭제후 재삽입 ㅇㅇ)
				SetItemInQuickSlot(QuickSlot->QuickSlotNumber,ItemObj, false);
			}
		}
	}
}

/*
* 이미 등록 되어있는지 확인하고
* 이미 등록이 되어 있으면, 등록된 Slot에서 삭제 시킨다.
*/
bool UQuickSlotWidget::CheckAlreadyRegistered(UNewItemObject* ItemObj, bool bIsOnlyDeleted, EQuickSlotNumber WantQuickNum)
{
	for (EQuickSlotNumber QuickNum : TEnumRange<EQuickSlotNumber>())
	{
		UQuickSlotSlotWidget* QuickSlot = GetQuickSlotSlot(QuickNum);

		//QuickSlot에 등록된 Obj와 같다면 등록되어있던 QuickSlot에서 삭제한다.
		if (QuickSlot && ItemObj == QuickSlot->GetMountedObject())
		{
			QuickSlot->RemoveMountedObj();

			/**근데 만약, 해당 QuickSlot Number에 등록된 Item을 중복 입력하면 
			* 삭제 프로세스만 진행 하기위해 false를 리턴하고 마친다.
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