// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/Widget/EquipmentSlot.h"
#include "OpenWorldRPG/NewInventory/Widget/NewItemwidget.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"
#include "OpenWorldRPG/NewInventory/Library/InventoryStruct.h"
#include "OpenWorldRPG/NewInventory/CustomDDOperation.h"

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

	//UE_LOG(LogTemp, Warning, TEXT("UEquipmentSlot bCanDrop = %d"), bCanDrop ? 1 : 0); //한자로 나옴 왜이럼?
}

bool UEquipmentSlot::IsSupportedEquip(UNewItemObject* ItemObj)
{
	bool bReturn = false;

	//장착템이면서  장착템의 Type과 이 Slot의 Type이 같다면 true를 리턴한다.
	if (ItemObj->ItemInfo.DataAsset->InteractType == EInteractType::EIT_Equipment &&
		ItemObj->ItemInfo.DataAsset->EquipmentType == SlotType)
	{
		bReturn = true;
	}
	return bReturn;
}

//다른곳에서 사용하기 위해 함수로 뺌 (CustomInventoryLibrary)
bool UEquipmentSlot::TrySlotEquip(UNewItemObject* Var_ItemObj)
{
	if (Var_ItemObj)
	{
		if (IsSupportedEquip(Var_ItemObj))
		{
			UE_LOG(LogTemp, Warning, TEXT("Correct Slot"));

			
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

			//BChar가 정상이면 장착을 시도한다.
			if (BChar != nullptr)
			{
				AEquipment* Equipment = nullptr;
				//Item이 Destroy된 상태면 (InventoryGrid에 있었던 Equipment들이 해당됨)
				//Item을 Spawn하고 기존에 저장된 정보를  새로 Spawn한 item에 이관한다.
				//대부분 bIsDestroyed에 분기됨. else문은 거의 사용되지 않음
				if (Var_ItemObj->bIsDestoryed)
				{
					//여기 함수로 바꿔서 Equip에서도 사용할 수 있도록 하자.
					Equipment = UCustomInventoryLibrary::SpawnEquipment(GetWorld(), Var_ItemObj); //함수화 함.
					/*
					Equipment = Cast<AEquipment>(GetWorld()->SpawnActor<AActor>(Var_ItemObj->GetItemClass()));
					if (Equipment)
					{
						Var_ItemObj->bIsDestoryed = false;
						
						Equipment->ReInitialize(Var_ItemObj);
						Equipment->SetItemState(EItemState::EIS_Pickup);
						if (Equipment->bHasStorage)
						{
							Equipment->EquipInventoryComp = Var_ItemObj->GetItemInvComp();
						}
						
						//Var_ItemObj->SetMotherContainer(nullptr);

					}
					*/
				}
				else
				{
					Equipment = Var_ItemObj->Equipment;
				}


				if (Equipment != nullptr)
				{
					Var_ItemObj->bIsDestoryed = false;
					Equipment->ItemObj = Var_ItemObj;
					if (Equipment->ItemSetting.DataAsset->EquipmentType == EEquipmentType::EET_Rifle)
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
		//Slot의 Type과 Equipment의 Type이 일치하지 않는 경우 다시 원래 있던곳으로 옮긴다.
		/*else
		{
			UCustomInventoryLibrary::BackToItem(Var_ItemObj);
		}*/
	}
	return false;
}

bool UEquipmentSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bReturn = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	

	PaintBGBorder();
	UCustomDDOperation* DDOper = Cast<UCustomDDOperation>(InOperation);
	if (DDOper)
	{
		if (DDOper->ItemObj == false) return false;

		if (DDOper->ItemObj->bIsDragging && SettedObj != DDOper->ItemObj)
		{
			DDOper->ItemObj->bIsDragging = false;
			
			//동일한 Equipment를 드래그 했다가 놓을때 재장착을 방지하기위함.
			//EquipComp의 RemoveEquip에서 Slot의 SettedObj를 null로 바꿔준다.
			//DDOper->ItemObj->SettedSlot = this;			

			TrySlotEquip(DDOper->ItemObj);

			DDOper->ItemObj->RemoveLinkSlot();
			DDOper->ItemObj->SetLinkSlot(this);
			SettedObj = DDOper->ItemObj;
			
		}
	}

	/*UNewItemObject* ItemObj = Cast< UNewItemObject>(InOperation->Payload);
	if (ItemObj)
	{
		if (ItemObj->bIsDragging)
		{

			ItemObj->bIsDragging = false;

			TrySlotEquip(ItemObj);
		}*/
		/*
		if (IsSupportedEquip(ItemObj))
		{
			UE_LOG(LogTemp, Warning, TEXT("Correct Slot"));

			check(ItemObj->item);
			
			ABaseCharacter* BChar;
			if (LootedChar_Owner != nullptr)
			{
				BChar = LootedChar_Owner;
			}
			else
			{
				BChar = Cast<ABaseCharacter>(GetOwningPlayerPawn());
			}

			if (BChar != nullptr)
			{
				AEquipment* Equipment = nullptr;
				if (ItemObj->bIsDestoryed)
				{
					Equipment = Cast<AEquipment>(GetWorld()->SpawnActor<AActor>(ItemObj->GetItemClass()));
					if (Equipment)
					{
						ItemObj->bIsDestoryed = false;
						Equipment->ReInitialize(ItemObj);
						Equipment->SetItemState(EItemState::EIS_Pickup);
						ItemObj->SetMotherContainer(nullptr);
						
					}
				}
				else
				{
					Equipment = Cast<AEquipment>(ItemObj->item);
				}


				if (Equipment != nullptr)
				{
					Equipment->StepEquip(BChar);
				}
			}
		*/

			//old version
			/*
			AMainCharacter* Main = Cast<AMainCharacter>(GetOwningPlayerPawn());
			AEquipment* Equipment = Cast<AEquipment>(ItemObj->item);
			
			if (Equipment)// && ItemObj->GetMotherContainer() != nullptr) //-> 아이템을 뺏다가 바로 끼는 경우가 있을때가 있기 때문에 조건에서 뺌.
			{
				if(SlotType == EEquipmentType::EET_Rifle)
				{
					AWeapon* Weapon = Cast<AWeapon>(Equipment);
					if (Weapon)
					{
						if (RifleSlotType == ERifleSlot::ERS_Primary)
						{
							Weapon->RifleAssign = ERifleAssign::ERA_Primary;
							Main->PrimaryWeapon = Weapon;	
						}
						else if (RifleSlotType == ERifleSlot::ERS_Sub)
						{
							Weapon->RifleAssign = ERifleAssign::ERA_Sub;
							Main->SubWeapon = Weapon;
						}
					}
				}

				if(ItemObj->bIsDestoryed)
				{
					AEquipment* T_Equipment = Cast<AEquipment>(GetWorld()->SpawnActor<AActor>(ItemObj->GetItemClass()));
					if (T_Equipment)
					{
						if(T_Equipment->EquipmentType == EEquipmentType::EET_Rifle)
						{
							AWeapon* T_Weapon = Cast<AWeapon>(T_Equipment);
							if(T_Weapon)
							{
								T_Weapon->RifleAssign = Weapon->RifleAssign;
							}
						}


						ItemObj->bIsDestoryed = false;
						//T_Equipment = Equipment;
						if (Equipment->bHasStorage)
						{
							T_Equipment->EquipInventoryComp = Equipment->EquipInventoryComp;
						}
						T_Equipment->ReInitialize(ItemObj);

						Equipment = T_Equipment; //error
						ItemObj->item = Equipment;

						Equipment->SetItemState(EItemState::EIS_Pickup);						
					}
				}
				
				ItemObj->MotherContainer = nullptr;
				*/

				//아래 /**/ 주석은 안쓰는부분.
				/*AWeapon* T_Weapon = Cast<AWeapon>(Equipment);
				if (T_Weapon)
				{
					T_Weapon->RifleAssign = Weapon->RifleAssign;
					T_Weapon->StepEquip(Main
				}*/
				//else
				//{
					 //Weapon으로 cast, equip함수 호출
				//}
			//}			
		//}
		
	/*}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Item obj is null"));
	}
	*/

	return bReturn;
}

FReply UEquipmentSlot::NativeOnPreviewKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	//UE_LOG(LogTemp, Warning, TEXT("NewInventoryGrid::OnPreviewKeyDown"));
	FReply Reply = Super::NativeOnPreviewKeyDown(InGeometry, InKeyEvent);

	bool bDragging = UWidgetBlueprintLibrary::IsDragDropping();
	if (bDragging)
	{
		UNewItemObject* Obj = Cast<UNewItemObject>(UWidgetBlueprintLibrary::GetDragDroppingContent()->Payload);
		if (Obj && Obj->bCanRotated)
		{
			if (InKeyEvent.GetKey() == EKeys::R)
			{
				Obj->ItemRotate();
				UNewItemwidget* Itemwidget = Cast<UNewItemwidget>(UWidgetBlueprintLibrary::GetDragDroppingContent()->DefaultDragVisual);
				if (Itemwidget)
				{
					Itemwidget->Refresh();
				}
			}
		}
	}
	return Reply;
}