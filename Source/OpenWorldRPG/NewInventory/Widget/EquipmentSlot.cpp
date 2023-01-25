// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/Widget/EquipmentSlot.h"
#include "OpenWorldRPG/NewInventory/Widget/NewItemwidget.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/CustomDDOperation.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"

#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"
#include "OpenWorldRPG/NewInventory/Library/InventoryStruct.h"


#include "OpenWorldRPG/Item/BasePDA.h"
#include "OpenWorldRPG/Item/CustomPDA.h"
//#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"
//#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"

#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/BaseCharacter.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"


void UEquipmentSlot::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);

	UCustomDDOperation* DDOper = Cast<UCustomDDOperation>(InOperation);
	if (DDOper)
	{
		PaintBGBorder(DDOper->ItemObj);
	}

	/*UNewItemObject* ItemObj = Cast< UNewItemObject>(InOperation->Payload);
	if (ItemObj)
	{
		PaintBGBorder(ItemObj);
	}*/

}
void UEquipmentSlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	PaintBGBorder();
	
}



void UEquipmentSlot::PaintBGBorder(UNewItemObject* ItemObj)
{
	FLinearColor Red = FLinearColor(1.f, 0.f, 0.f, 0.25f);
	FLinearColor Green = FLinearColor(0.f, 1.f, 0.f, 0.25f);
	FLinearColor Black = FLinearColor(0.f, 0.f, 0.f, 0.25f);
	if (ItemObj != nullptr)
	{
		if (BGBorder)
		{
			if (IsSupportedEquip(ItemObj))
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
		BGBorder->SetBrushColor(Black);
		bCanDrop = false;
	}

	//UE_LOG(LogTemp, Warning, TEXT("UEquipmentSlot bCanDrop = %d"), bCanDrop ? 1 : 0); //���ڷ� ���� ���̷�?
}

bool UEquipmentSlot::IsSupportedEquip(UNewItemObject* ItemObj)
{
	bool bReturn = false;

	ABaseCharacter* TempChar = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	

	//�������̸鼭  �������� Type�� �� Slot�� Type�� ���ٸ� true�� �����Ѵ�.
	UCustomPDA* CPDA = Cast<UCustomPDA>(ItemObj->ItemInfo.DataAsset);
	
	if(CPDA == nullptr) return false;

	if (bIsforWeaponParts)
	{
		if (WeaponPartsType == CPDA->WeaponPartsType)
		{
			if (IsEmpty())
			{
				bReturn = true;
			}
		}
	}
	else if (CPDA->InteractType == EInteractType::EIT_Equipment &&
		CPDA->EquipmentType == SlotType)
	{
		//������ ������ ����ִ��� Ȯ���Ѵ�.
		if (IsEmpty())
		{
			//UE_LOG(LogTemp,Warning,TEXT("EquipSlot::SupportedEquip / Empty"));
			bReturn = true;
		}
	}
	//else if (CPDA->InteractType == EInteractType::EIT_Equipment &&
	//	bIsforWeaponParts && WeaponPartsType == CPDA->WeaponPartsType)
	//{
	//	if (IsEmpty())
	//	{
	//		bReturn = true;
	//	}
	//}


	return bReturn;
}

bool UEquipmentSlot::IsEmpty()
{
	if (SettedObj)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipSlot::SupportedEquip / Not empty"));
		return false;
	}
	return true;
}

bool UEquipmentSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bReturn = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	
	PaintBGBorder();
	UCustomDDOperation* DDOper = Cast<UCustomDDOperation>(InOperation);
	if (DDOper)
	{
		if (DDOper->ItemObj == false) return false;
		

		if (DDOper->ItemObj->bIsDragging && !SettedObj)// != DDOper->ItemObj)
		{
			DDOper->ItemObj->bIsDragging = false;

			//EquipSlot���� �۵��� ��
			if (bIsforWeaponParts == false)
			{
				bReturn = TrySlotEquip(DDOper->ItemObj);
				//UE_LOG(LogTemp, Warning, TEXT("EquipSlot::OnDrop / TrySlotEquip func result is true. set link obj"));
			}
			//WeaponParts Slot���� �۵��� ��
			else if( bIsforWeaponParts)
			{
				bReturn = TrySlotParts(DDOper->ItemObj);
				//UE_LOG(LogTemp, Error, TEXT("EquipSlot::OnDrop / TrySlotEquip func result is false."));
			}
		}
		
	}
	return bReturn;
}

AEquipment* UEquipmentSlot::SpawnEquipment(UNewItemObject* Obj)
{
	//Item�� Destroy�� ���¸� (InventoryGrid�� �־��� Equipment���� �ش��)
	//Item�� Spawn�ϰ� ������ ����� ������  ���� Spawn�� item�� �̰��Ѵ�.
	//��κ� bIsDestroyed�� �б��. else���� ���� ������ ����
	AEquipment* ReturnEquip = nullptr;

	if (Obj->bIsDestoryed)
	{
		ReturnEquip = UCustomInventoryLibrary::SpawnEquipment(GetWorld(), Obj);
	}
	else
	{
		ReturnEquip = Obj->Equipment;
		if (Obj->MotherEquipComp)
		{
			Obj->MotherEquipComp->RemoveEquipment(Obj);
			Obj->MotherEquipComp = nullptr;
			ReturnEquip = UCustomInventoryLibrary::SpawnEquipment(GetWorld(), Obj);
		}
	}

	return ReturnEquip;
}


bool UEquipmentSlot::TrySlotEquip(UNewItemObject* Var_ItemObj)
{
	if (Var_ItemObj)
	{
		if (IsSupportedEquip(Var_ItemObj))
		{
			UE_LOG(LogTemp, Warning, TEXT("EquipSlot::Correct Slot"));


			//AI�� �÷��̾ ������.
			ABaseCharacter* BChar;
			if (LootedChar_Owner != nullptr)
			{
				BChar = LootedChar_Owner;
			}
			else
			{
				BChar = Cast<ABaseCharacter>(GetOwningPlayerPawn());
			}

			////���� EquipmentComp�� ������ �Ұ����ϵ��� �Ѵ�. (Weapon Swap ���� ����)
			//if(Var_ItemObj->MotherEquipComp == BChar->Equipment) return false;

			//BChar�� �����̸� ������ �õ��Ѵ�.
			if (BChar != nullptr)
			{
				AEquipment* Equipment = nullptr;

				Equipment = SpawnEquipment(Var_ItemObj);

				if (Equipment != nullptr)
				{
					Var_ItemObj->bIsDestoryed = false;
					Equipment->ItemObj = Var_ItemObj;
					UCustomPDA* CPDA = Cast<UCustomPDA>(Equipment->ItemSetting.DataAsset);
					if (CPDA->EquipmentType == EEquipmentType::EET_Rifle)
					{
						Equipment->Equip(BChar, RifleSlotType);
					}
					else
					{
						Equipment->Equip(BChar);
					}
					return true;
				}
			}
		}
	}
	return false;
}

bool UEquipmentSlot::TrySlotParts(UNewItemObject* PartsObj)
{
	if(IsSupportedEquip(PartsObj) == false) return false;
	UCustomPDA* WeaponPartsPDA = Cast<UCustomPDA>(PartsObj->ItemInfo.DataAsset);

	//if( OwnerWeaponObj == nullptr || WeaponPartsPDA == nullptr) return false;
	if( WeaponPartsPDA == nullptr) return false;
	if (OwnerWeaponObj.IsValid() == false) return false;
	
		
		
		/*OnEquipWeaponParts.Broadcast(PartsObj);
		OnRefreshWidget.Broadcast();*/
		//OnChangeWeaponParts.Broadcast();

		//OnEquipWeaponParts.Broadcast(PartsObj);
		OwnerWeaponObj.Get()->WeaponPartsManager->AddParts(PartsObj);
		return true;
	//}
	//return false;
}

void UEquipmentSlot::RemoveParts(UNewItemObject* PartsObj)
{
	//this ��ü�� nullptr..
	if(SettedObj == PartsObj)
	{

		SettedObj = nullptr;
		//OnChangeWeaponParts.Broadcast();
		OnRefreshWidget.Broadcast();
	}
}



FReply UEquipmentSlot::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	//UE_LOG(LogTemp, Warning, TEXT("NewInventoryGrid::OnPreviewKeyDown"));
	FReply Reply = Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);

	bool bDragging = UWidgetBlueprintLibrary::IsDragDropping();
	if (bDragging)
	{
		UCustomDDOperation* CusDDOper = Cast<UCustomDDOperation>(UWidgetBlueprintLibrary::GetDragDroppingContent());
		if(CusDDOper == nullptr) return Reply;

		UNewItemObject* Obj = CusDDOper->ItemObj;
		if (Obj && Obj->bCanRotated)
		{
			if (InKeyEvent.GetKey() == EKeys::R)
			{
				Obj->ItemRotate();
				UNewItemwidget* Itemwidget = Cast<UNewItemwidget>(CusDDOper->DefaultDragVisual);
				if (Itemwidget)
				{
					Itemwidget->Refresh();
				}
			}
		}
	}
	return Reply;
}