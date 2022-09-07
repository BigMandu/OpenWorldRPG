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

	//UE_LOG(LogTemp, Warning, TEXT("UEquipmentSlot bCanDrop = %d"), bCanDrop ? 1 : 0); //���ڷ� ���� ���̷�?
}

bool UEquipmentSlot::IsSupportedEquip(UNewItemObject* ItemObj)
{
	bool bReturn = false;

	//�������̸鼭  �������� Type�� �� Slot�� Type�� ���ٸ� true�� �����Ѵ�.
	if (ItemObj->ItemInfo.DataAsset->InteractType == EInteractType::EIT_Equipment &&
		ItemObj->ItemInfo.DataAsset->EquipmentType == SlotType)
	{
		bReturn = true;
	}
	return bReturn;
}

//�ٸ������� ����ϱ� ���� �Լ��� �� (CustomInventoryLibrary)
bool UEquipmentSlot::TrySlotEquip(UNewItemObject* Var_ItemObj)
{
	if (Var_ItemObj)
	{
		if (IsSupportedEquip(Var_ItemObj))
		{
			UE_LOG(LogTemp, Warning, TEXT("Correct Slot"));

			
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

			//BChar�� �����̸� ������ �õ��Ѵ�.
			if (BChar != nullptr)
			{
				AEquipment* Equipment = nullptr;
				//Item�� Destroy�� ���¸� (InventoryGrid�� �־��� Equipment���� �ش��)
				//Item�� Spawn�ϰ� ������ ����� ������  ���� Spawn�� item�� �̰��Ѵ�.
				//��κ� bIsDestroyed�� �б��. else���� ���� ������ ����
				if (Var_ItemObj->bIsDestoryed)
				{
					//���� �Լ��� �ٲ㼭 Equip������ ����� �� �ֵ��� ����.
					Equipment = UCustomInventoryLibrary::SpawnEquipment(GetWorld(), Var_ItemObj); //�Լ�ȭ ��.
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
		//Slot�� Type�� Equipment�� Type�� ��ġ���� �ʴ� ��� �ٽ� ���� �ִ������� �ű��.
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
			
			//������ Equipment�� �巡�� �ߴٰ� ������ �������� �����ϱ�����.
			//EquipComp�� RemoveEquip���� Slot�� SettedObj�� null�� �ٲ��ش�.
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
			
			if (Equipment)// && ItemObj->GetMotherContainer() != nullptr) //-> �������� ���ٰ� �ٷ� ���� ��찡 �������� �ֱ� ������ ���ǿ��� ��.
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

				//�Ʒ� /**/ �ּ��� �Ⱦ��ºκ�.
				/*AWeapon* T_Weapon = Cast<AWeapon>(Equipment);
				if (T_Weapon)
				{
					T_Weapon->RifleAssign = Weapon->RifleAssign;
					T_Weapon->StepEquip(Main
				}*/
				//else
				//{
					 //Weapon���� cast, equip�Լ� ȣ��
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