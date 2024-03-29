// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/Inventory/EquipmentSlot.h"
#include "OpenWorldRPG/UI/Inventory/NewItemwidget.h"

#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/CustomDDOperation.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"

#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"

#include "OpenWorldRPG/Item/BasePDA.h"
#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/Item/WeaponPartsPDA.h"

#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/BaseCharacter.h"

#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#include "Components/Border.h"
#include "Components/Image.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


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
	FLinearColor Yellow = FLinearColor(1.f, 1.f, 0.f, 0.25f);

	if (ItemObj != nullptr)
	{
		if (BGBorder)
		{
			if (IsSupportedEquip(ItemObj))
			{
				UE_LOG(LogTemp, Warning, TEXT("EquipSlot::PaintBGBorder / Support Equip! GREEN"));
				BGBorder->SetBrushColor(Green);
				bCanDrop = true;
			}
			//지원되는 슬롯이 아니고, 장착된 슬롯이지만 backpack 또는 vest 슬롯인 경우 item을 넣을 수 있으니 Yellow로 한다.
			else if (IsEmpty() == false)
			{
				if (SlotType == EEquipmentType::EET_Backpack || SlotType == EEquipmentType::EET_Vest)
				{
					UE_LOG(LogTemp, Warning, TEXT("EquipSlot::PaintBGBorder / Can ADD ITEM!! YELLOW"));
					BGBorder->SetBrushColor(Yellow);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("EquipSlot::PaintBGBorder / NAH, RED!!"));
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

	//UE_LOG(LogTemp, Warning, TEXT("UEquipmentSlot bCanDrop = %d"), bCanDrop ? 1 : 0); //한자로 나옴 왜이럼?
}

bool UEquipmentSlot::IsSupportedEquip(UNewItemObject* ItemObj)
{
	bool bReturn = false;

	ABaseCharacter* TempChar = Cast<ABaseCharacter>(GetOwningPlayerPawn());
	

	//장착템이면서  장착템의 Type과 이 Slot의 Type이 같다면 true를 리턴한다. -> 장착이 가능함.
	UCustomPDA* CPDA = Cast<UCustomPDA>(ItemObj->ItemInfo.DataAsset);
	
	if(CPDA == nullptr) return false;

	//WeaponParts용
	if (bIsforWeaponParts)
	{
		UWeaponPartsPDA* WPPDA = Cast<UWeaponPartsPDA>(CPDA);

		if ( WPPDA && WeaponPartsType == WPPDA->WeaponPartsType)
		{
			if ( OwnerWeaponObj == nullptr ) return false;
			//Parts의 limit추가.
			//Parts의 limit이 제한없음 이거나 Parts의 limit와 weapon의 limit이 같다면 추가가능
			if ( ( WPPDA->WeaponPartsLimitationType == EWeaponPartsLimitationType::EWPT_UnLimited) ||
			( WPPDA->WeaponPartsLimitationType == Cast<UCustomPDA>(OwnerWeaponObj->ItemInfo.DataAsset)->WeaponPartsLimitationType/* OwnerWeaponPDA->WeaponPartsLimitationType*/) )
			{
				if ( IsEmpty() )
				{
					bReturn = true;
				}
			}
			
		}
	}
	//그냥 일반 장착 장비용
	else if (CPDA->InteractType == EInteractType::EIT_Equipment &&
		CPDA->EquipmentType == SlotType)
	{
		//슬롯이 같으면 비어있는지 확인한다.
		if (IsEmpty())
		{
			//UE_LOG(LogTemp,Warning,TEXT("EquipSlot::SupportedEquip / Empty"));
			bReturn = true;
		}
	}


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
			//EquipSlot으로 작동할 때
			if (bIsforWeaponParts == false)
			{
				bReturn = TrySlotEquip(DDOper->ItemObj);
				//UE_LOG(LogTemp, Warning, TEXT("EquipSlot::OnDrop / TrySlotEquip func result is true. set link obj"));
			}
			//WeaponParts Slot으로 작동할 때
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
	//Item이 Destroy된 상태면 (InventoryGrid에 있었던 Equipment들이 해당됨)
	//Item을 Spawn하고 기존에 저장된 정보를  새로 Spawn한 item에 이관한다.
	//대부분 bIsDestroyed에 분기됨. else문은 거의 사용되지 않음
	AEquipment* ReturnEquip = nullptr;

	if (Obj->bIsDestoryed)
	{
		ReturnEquip = UCustomInventoryLibrary::SpawnEquipment(GetWorld(), Obj);
	}
	else
	{
		//ReturnEquip = Obj->Equipment;
		if (Obj->MotherEquipComp)
		{
			Obj->MotherEquipComp->RemoveEquipment(Obj);
			Obj->MotherEquipComp = nullptr;

			//이거 왜했지?
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


			//AI와 플레이어를 나눈다.
			ABaseCharacter* BChar;
			if (LootedChar_Owner != nullptr)
			{
				BChar = LootedChar_Owner;
			}
			else
			{
				BChar = Cast<ABaseCharacter>(GetOwningPlayerPawn());
			}

			////같은 EquipmentComp면 장착이 불가능하도록 한다. (Weapon Swap 방지 차원)
			//if(Var_ItemObj->MotherEquipComp == BChar->Equipment) return false;

			//BChar가 정상이면 장착을 시도한다.
			if (BChar != nullptr)
			{
				AEquipment* Equipment = nullptr;

				Equipment = SpawnEquipment(Var_ItemObj);

				if (Equipment != nullptr)
				{
					//Drop시에 DropSound를 재생한다.
					if ( Var_ItemObj->ItemInfo.DataAsset->SlotDropSound )
					{
						UE_LOG(LogTemp, Warning, TEXT("EqiupSlot::OnDrop// Play UI sound"));
						UGameplayStatics::PlaySound2D(GetWorld(), Var_ItemObj->ItemInfo.DataAsset->SlotDropSound);
					}

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

	//Drop시에 DropSound를 재생한다.
	if ( PartsObj->ItemInfo.DataAsset->SlotDropSound )
	{
		UE_LOG(LogTemp, Warning, TEXT("EqiupSlot::OnDrop// Play UI sound"));
		UGameplayStatics::PlaySound2D(GetWorld(), PartsObj->ItemInfo.DataAsset->SlotDropSound);
	}
		
		
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
	//this 자체가 nullptr..
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