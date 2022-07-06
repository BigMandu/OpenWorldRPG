// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/EquipmentSlot.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/NewItemwidget.h"

#include "OpenWorldRPG/BaseCharacter.h"
#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Blueprint/DragDropOperation.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

bool UEquipmentSlot::IsSupportedEquip(UNewItemObject* Obj)
{
	bool bReturn = false;

	if (Obj->InteractType == EInteractType::EIT_Equipment && Obj->EquipmentType == SlotType)
	{
		bReturn = true;	
	}
	return bReturn;
}

void UEquipmentSlot::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	UNewItemObject* ItemObj = Cast< UNewItemObject>(InOperation->Payload);
	if (ItemObj)
	{
		PaintBGBorder(ItemObj);
		
	}

}
void UEquipmentSlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	PaintBGBorder();
	
}


void UEquipmentSlot::PaintBGBorder(UNewItemObject* Obj)
{
	FLinearColor Red = FLinearColor(1.f, 0.f, 0.f, 0.25f);
	FLinearColor Green = FLinearColor(0.f, 1.f, 0.f, 0.25f);
	FLinearColor Black = FLinearColor(0.f, 0.f, 0.f, 0.25f);
	if (Obj != nullptr)
	{
		if (BGBorder)
		{
			if (IsSupportedEquip(Obj))
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
bool UEquipmentSlot::TrySlotEquip(UNewItemObject* Var_ItemObj)
{
	if (Var_ItemObj)
	{
		if (IsSupportedEquip(Var_ItemObj))
		{
			UE_LOG(LogTemp, Warning, TEXT("Correct Slot"));

			check(Var_ItemObj->item);

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
				if (Var_ItemObj->bIsDestoryed)
				{
					Equipment = Cast<AEquipment>(GetWorld()->SpawnActor<AActor>(Var_ItemObj->GetItemClass()));
					if (Equipment)
					{
						Var_ItemObj->bIsDestoryed = false;
						Equipment->ReInitialize(Var_ItemObj);
						Equipment->SetItemState(EItemState::EIS_Pickup);
						//Var_ItemObj->SetMotherContainer(nullptr);

					}
				}
				else
				{
					Equipment = Cast<AEquipment>(Var_ItemObj->item);
				}


				if (Equipment != nullptr)
				{
					Equipment->StepEquip(BChar);
					//Var_ItemObj->SetMotherEquipSlot(this);
					return true;
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Incorrect Slot"));
		}
	}
	return false;
}

bool UEquipmentSlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bReturn = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	
	UNewItemObject* ItemObj = Cast< UNewItemObject>(InOperation->Payload);
	if (ItemObj)
	{
		TrySlotEquip(ItemObj);
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
		
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Item obj is null"));
	}
	

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