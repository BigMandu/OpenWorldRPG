// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/Inventory/EquipStorageWidget.h"
#include "OpenWorldRPG/UI/Inventory/EquipmentSlot.h"
#include "OpenWorldRPG/UI/Inventory/NewItemwidget.h"
#include "OpenWorldRPG/UI/Inventory/NewInventoryGrid.h"
#include "OpenWorldRPG/UI/Inventory/NewInventory.h"

#include "OpenWorldRPG/CustomLibrary/CustomStructLibrary.h"

#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"

#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/Item/Equipment.h"

#include "OpenWorldRPG/BaseCharacter.h"

#include "Components/Border.h"
#include "Components/Overlay.h"
#include "Components/VerticalBox.h"

#include "Components/CanvasPanelSlot.h"
#include "Blueprint/DragDropOperation.h"



void UEquipStorageWidget::EquipInitialize(UEquipmentComponent* p_EquipComp)
{
	EquipComp = p_EquipComp;
	if (EquipComp)
	{
		EquipComp->OnEquipmentUpdated.AddUFunction(this, FName("RefreshEquipWidget"));
		if (LootedChar_Owner)
		{
			VestSlot->LootedChar_Owner = LootedChar_Owner;
			BackpackSlot->LootedChar_Owner = LootedChar_Owner;
		}
	}
}

void UEquipStorageWidget::InitializeInventory(ABaseCharacter* BChar)
{
	bool bNoInitSecureBox = false;

	if (BChar->bIsDie)
	{
		bNoInitSecureBox = true;
		SBVB->SetVisibility(ESlateVisibility::Collapsed);
		//SecureBoxWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (bNoInitSecureBox == false)
	{
		SecureBoxWidget->StorageObj = BChar->SecureBoxStorage;
		SecureBoxWidget->GridInit();
	}

	PocketWidget->StorageObj = BChar->PocketStorage;
	PocketWidget->GridInit();
}

void UEquipStorageWidget::RefreshEquipWidget()
{
	RemoveSlot();
	if (EquipComp == nullptr) return;
	
	UCustomPDA* CPDA = nullptr;
	for (auto& ele : EquipComp->EquipmentItems)
	{
		if (ele != nullptr)
		{
			CPDA = Cast<UCustomPDA>(ele->ItemInfo.DataAsset);
			if (CPDA == nullptr) continue;

			switch (CPDA->EquipmentType)
			{
			case EEquipmentType::EET_Vest:
				SetSlot(ele, VestSlot);
				if (VestOverlay && CPDA->bHasStorage)
				{
					UItemStorageObject* StorageObj = Cast<UItemStorageObject>(ele);
					SettingStorageWidget(VestOverlay, StorageObj);
				}
				break;
			case EEquipmentType::EET_Backpack:
				SetSlot(ele, BackpackSlot);
				if (BackpackOverlay && CPDA->bHasStorage)// && ele->EquipGridWidget)
				{
					UItemStorageObject* StorageObj = Cast<UItemStorageObject>(ele);
					SettingStorageWidget(BackpackOverlay, StorageObj);
				}
				break;
			}
		}
	}
	
}



void UEquipStorageWidget::SetSlot(UNewItemObject* EquipObj, UEquipmentSlot* EquipSlot)
{
	if (WNewItemWidget)
	{
		UNewItemwidget* ItemWidget = CreateWidget<UNewItemwidget>(this, WNewItemWidget);

		if (ItemWidget)
		{
			EquipSlot->Initialize();
			//ItemWidget->OnRemoved.AddUFunction(this, FName("RemoveEquipment"));
			EquipSlot->BGBorder->ClearChildren();


			ItemWidget->Tilesize = 60.f;//Equip->EquipInventoryComp->TileSize; //임시로 이렇게 사이즈를 박아뒀다., GetDesiredSize는 widget이 화면에 출력되야 구할수 있는건데 ..  //EquipSlot->GetDesiredSize().X;
			ItemWidget->ItemObj = EquipObj;
			
			ItemWidget->Refresh();

			if (LootedChar_Owner != nullptr)
			{
				EquipSlot->LootedChar_Owner = LootedChar_Owner;
			}

			/**
			* Equip Slot과 EquipObj를 서로 Link한다.
			*/
			EquipObj->RemoveLinkSlot();
			EquipSlot->SettedObj = EquipObj;			
			EquipObj->SetLinkSlot(EquipSlot);


			EquipSlot->BGBorder->AddChild(ItemWidget);
			EquipSlot->PaintBGBorder();


		}
	}
}
void UEquipStorageWidget::SettingStorageWidget(UOverlay* EquipOverlay, UItemStorageObject* Var_StorageObj)
{
	if (WGridInvWidget)
	{
		if (Var_StorageObj->Inventory.Num() == 0)
		{
			UCustomPDA* CPDA = Cast<UCustomPDA>(Var_StorageObj->ItemInfo.DataAsset);
			Var_StorageObj->InitializeStorage(CPDA->StorageX, CPDA->StorageY, CPDA->TileSize);
		}
		EquipOverlay->ClearChildren();

		UNewInventoryGrid* GridInv = CreateWidget<UNewInventoryGrid>(this, WGridInvWidget);
		GridInv->StorageObj = Var_StorageObj;
		GridInv->GridInit();

		GridInv->OpenAdditionalWidget.Clear(); //새로 추가함.
		GridInv->OpenAdditionalWidget.AddDynamic(MainWidget, &UNewInventory::BindingOpenWidgetFunc);
		EquipOverlay->AddChild(GridInv);

	}
}
void UEquipStorageWidget::RemoveSlot()
{
	
	VestOverlay->ClearChildren();
	BackpackOverlay->ClearChildren();
	
	VestSlot->BGBorder->ClearChildren();
	BackpackSlot->BGBorder->ClearChildren();
}