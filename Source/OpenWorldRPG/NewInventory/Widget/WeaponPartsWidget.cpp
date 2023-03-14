// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/Widget/WeaponPartsWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/NewItemwidget.h"
#include "OpenWorldRPG/NewInventory/Widget/EquipmentSlot.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"

#include "Components/Border.h"

void UWeaponPartsWidget::WidgetInit(UNewItemObject* Var_WeaponObj)
{
	if(!Var_WeaponObj) return;

	WeaponObj = Var_WeaponObj;
	
	MuzzleSlot->Initialize();
	TacticalSlot->Initialize();	
	ScopeSlot->Initialize();

	MuzzleSlot->OwnerWeaponObj = Var_WeaponObj;
	TacticalSlot->OwnerWeaponObj = Var_WeaponObj;
	ScopeSlot->OwnerWeaponObj = Var_WeaponObj;
	
	//MuzzleSlot->OnUnEquipWeaponParts.AddDynamic(this,)

	WeaponObj->WeaponPartsManager->OnChangeParts.AddDynamic(this, &UWeaponPartsWidget::RefreshWidget);

	/*MuzzleSlot->OnEquipWeaponParts.AddDynamic(this, &UWeaponPartsWidget::EquipWeaponParts);
	TacticalSlot->OnEquipWeaponParts.AddDynamic(this, &UWeaponPartsWidget::EquipWeaponParts);
	ScopeSlot->OnEquipWeaponParts.AddDynamic(this, &UWeaponPartsWidget::EquipWeaponParts);*/

	/*MuzzleSlot->OnRefreshWidget.AddDynamic(this, &UWeaponPartsWidget::RefreshWidget);
	TacticalSlot->OnRefreshWidget.AddDynamic(this, &UWeaponPartsWidget::RefreshWidget);
	ScopeSlot->OnRefreshWidget.AddDynamic(this, &UWeaponPartsWidget::RefreshWidget);*/

	//RefreshWidget();
}


void UWeaponPartsWidget::RefreshWidget()
{
	WeaponImage->SetColorAndOpacity(FLinearColor(0.f, 0.f, 0.f, 0.f));
	if (WeaponObj == nullptr) return;

	WeaponImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
	WeaponImage->SetBrushFromTexture(Cast<UWeaponPDA>(WeaponObj->ItemInfo.DataAsset)->WeaponVectorIMG);

	for (auto PartsType : TEnumRange<EWeaponPartsType>())
	{
		UNewItemObject* PartsObj = nullptr;
		switch (PartsType)
		{
			case EWeaponPartsType::EWPT_Muzzle:
				PartsObj = WeaponObj->WeaponPartsManager->MuzzleParts.IsValid() ? WeaponObj->WeaponPartsManager->MuzzleParts.Get() : nullptr;
				UpdatePartsSlotWidget(MuzzleSlot,PartsObj);
				break;
			case EWeaponPartsType::EWPT_Scope:
				PartsObj = WeaponObj->WeaponPartsManager->ScopeParts.IsValid() ? WeaponObj->WeaponPartsManager->ScopeParts.Get() : nullptr;
				UpdatePartsSlotWidget(ScopeSlot, PartsObj);
				break;
			case EWeaponPartsType::EWPT_Tactical:
				PartsObj = WeaponObj->WeaponPartsManager->TacticalParts.IsValid() ? WeaponObj->WeaponPartsManager->TacticalParts.Get() : nullptr;
				UpdatePartsSlotWidget(TacticalSlot, PartsObj);
				break;
		}
	}
}


/**
 * RefreshWidget???몄??? 留??WeaponPDA? ?? Parts ?蹂대? ??濡
 * WeaponPartsWidget? 媛 Slot? 媛깆???
 * PartsObj媛 nullptr濡 ??댁? 寃쎌곗 ClearChildren?쇰? ??留 ?怨 由ы댄??
 */
void UWeaponPartsWidget::UpdatePartsSlotWidget(UEquipmentSlot* PartsSlot, UNewItemObject* PartsObj)
{
	if(PartsSlot == nullptr) return;
	PartsSlot->BGBorder->ClearChildren();
	PartsSlot->SettedObj = nullptr;

	if (PartsObj == nullptr) return;

	UNewItemwidget* ItemWidget = CreateWidget<UNewItemwidget>(this, WItemWidget);
	if (ItemWidget)
	{
		PartsSlot->SettedObj = PartsObj;
		ItemWidget->Initialize();
		ItemWidget->Tilesize = 40.f;
		ItemWidget->ItemObj = PartsObj;
		//ItemWidget->Refresh();

		PartsSlot->BGBorder->AddChild(ItemWidget);
	}
}




/************************************************************************/
/*                      ??? ?? ??                                 */
/************************************************************************/ 
  
 /*

void UWeaponPartsWidget::EquipWeaponParts(UNewItemObject* PartsObj)
{
	if(WeaponObj.IsValid() == false) return;
	CheckBeforeEquip(PartsObj);

	UCustomPDA* WeaponPartsPDA = Cast<UCustomPDA>(PartsObj->ItemInfo.DataAsset);
	UWeaponPDA* WeaponPDA = Cast<UWeaponPDA>(WeaponObj->ItemInfo.DataAsset);
	
	switch (WeaponPartsPDA->WeaponPartsType)
	{
	case EWeaponPartsType::EWPT_Muzzle:
		WeaponPDA->MuzzleParts = PartsObj;
		break;
	case EWeaponPartsType::EWPT_Scope:
		WeaponPDA->ScopeParts = PartsObj;
		break;
	case EWeaponPartsType::EWPT_Tactical:
		WeaponPDA->TacticalParts = PartsObj;
		break;
	}
	PartsObj->MotherWeaponPartsWidget = this;
	PartsObj->OwnerWeaponPDA = WeaponPDA;
}

void UWeaponPartsWidget::CheckBeforeEquip(UNewItemObject* PartsObj)
{
	if (PartsObj->MotherWeaponPartsWidget)
	{
		PartsObj->MotherWeaponPartsWidget->RemoveWeaponParts(PartsObj);
	}
	else if (PartsObj->MotherStorage)
	{
		//PartsObj->GetMotherStorage()->RemoveItem(PartsObj);
		PartsObj->MotherStorage->RemoveItem(PartsObj);
		PartsObj->MotherStorage = nullptr;
	}
}

void UWeaponPartsWidget::RemoveWeaponParts(UNewItemObject* PartsObj)
{
	UCustomPDA* WeaponPartsPDA = Cast<UCustomPDA>(PartsObj->ItemInfo.DataAsset);

	if (PartsObj->MotherWeaponPartsWidget == this)
	{
		PartsObj->MotherWeaponPartsWidget = nullptr;
	}

	switch (WeaponPartsPDA->WeaponPartsType)
	{
	case EWeaponPartsType::EWPT_Muzzle:
		PartsObj->OwnerWeaponPDA->MuzzleParts = nullptr;
		MuzzleSlot->RemoveParts(PartsObj);
		//MuzzleSlot->SettedObj = nullptr;
		break;
	case EWeaponPartsType::EWPT_Scope:
		PartsObj->OwnerWeaponPDA->ScopeParts = nullptr;
		ScopeSlot->RemoveParts(PartsObj);
		//ScopeSlot->SettedObj = nullptr;
		break;
	case EWeaponPartsType::EWPT_Tactical:
		PartsObj->OwnerWeaponPDA->TacticalParts = nullptr;
		TacticalSlot->RemoveParts(PartsObj);
		//TacticalSlot->SettedObj = nullptr;
		break;
	}

	PartsObj->OwnerWeaponPDA = nullptr;
}


*/

//OldVersion
/* EquipmentSlot?? Data媛 Insert, Delete?? 留???몄???? */
/*
void UWeaponPartsWidget::RefreshWidget()
{
	if(WeaponObj.IsValid() == false) return;
	UWeaponPDA* WeaponPDA = Cast<UWeaponPDA>(WeaponObj->ItemInfo.DataAsset);

	if (WeaponPDA)
	{
		for (auto PartsType : TEnumRange<EWeaponPartsType>())
		{
			UNewItemObject* PartsObj = nullptr;
			switch (PartsType)
			{
			case EWeaponPartsType::EWPT_Muzzle:
				MuzzleSlot->BGBorder->ClearChildren();
				if (WeaponPDA->MuzzleParts.IsValid())
				{
					
					PartsObj = WeaponPDA->MuzzleParts.Get();
					UpdatePartsSlotWidget(MuzzleSlot, PartsObj);
				}
				break;
			case EWeaponPartsType::EWPT_Scope:
				ScopeSlot->BGBorder->ClearChildren();
				if (WeaponPDA->ScopeParts.IsValid())
				{
					PartsObj = WeaponPDA->ScopeParts.Get();
					UpdatePartsSlotWidget(ScopeSlot, PartsObj);
				}
				break;
			case EWeaponPartsType::EWPT_Tactical:
				TacticalSlot->BGBorder->ClearChildren();
				if (WeaponPDA->TacticalParts.IsValid())
				{
					PartsObj = WeaponPDA->TacticalParts.Get();
					UpdatePartsSlotWidget(TacticalSlot, PartsObj);
				}
				break;
			}
		}

		//Live?, Spawned Item?대㈃ WeaponClass? ?⑥瑜??몄????
		if (WeaponObj->bIsDestoryed == false)
		{
			Cast<AWeapon>(WeaponObj->Equipment)->CheckWeaponParts();
		}

	}
}
*/


