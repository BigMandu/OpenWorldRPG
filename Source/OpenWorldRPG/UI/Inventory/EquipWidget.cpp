// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/Inventory/EquipWidget.h"
#include "OpenWorldRPG/UI/Inventory/EquipmentSlot.h"
#include "OpenWorldRPG/UI/Inventory/NewItemwidget.h"
#include "OpenWorldRPG/UI/Inventory/NewInventoryGrid.h"
#include "OpenWorldRPG/UI/Inventory/NewInventory.h"

#include "OpenWorldRPG/CustomLibrary/CustomStructLibrary.h"

#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"

#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/Item/CustomPDA.h"

#include "OpenWorldRPG/BaseCharacter.h"

#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/Overlay.h"
//#include "Components/Border.h"
//#include "Components/CanvasPanelSlot.h"
//#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

#include "Blueprint/DragDropOperation.h"

void UEquipWidget::EquipInitialize(UEquipmentComponent* p_EquipComp)
{
	EquipComp = p_EquipComp;
	if (EquipComp)
	{
		EquipComp->OnEquipmentUpdated.AddUFunction(this, FName("RefreshEquipWidget"));

		//AI의 LootWidget에서 사용할때
		if (LootedChar_Owner)
		{
			HelmetSlot->LootedChar_Owner = LootedChar_Owner;
			PlateSlot->LootedChar_Owner = LootedChar_Owner;
			PistolSlot->LootedChar_Owner = LootedChar_Owner;
			PrimarySlot->LootedChar_Owner = LootedChar_Owner;
			SubSlot->LootedChar_Owner = LootedChar_Owner;
		}
	}

	if (LootedChar_Owner == nullptr)
	{
		RefreshEquipWidget();
	}
	
}
/*
void UEquipWidget::InitializeInventory(ABaseCharacter* BChar)
{
	bool bNoInitSecureBox = false;
	if (BChar->bIsDie)
	{
		bNoInitSecureBox = true;
		SecureBoxWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	
	if (bNoInitSecureBox == false)
	{
		SecureBoxWidget->StorageObj = BChar->SecureBoxStorage;
		SecureBoxWidget->GridInit();
	}

	PocketWidget->StorageObj = BChar->PocketStorage;
	PocketWidget->GridInit();
	
}
*/

void UEquipWidget::RefreshEquipWidget()
{
	RemoveSlot();
	if (EquipComp)
	{
		UCustomPDA* CPDA = nullptr;
		for (auto& ele : EquipComp->EquipmentItems)
		{
			if (ele != nullptr)
			{
				CPDA = Cast<UCustomPDA>(ele->ItemInfo.DataAsset);
				if(CPDA == nullptr) continue;

				switch (CPDA->EquipmentType)
				{
				case EEquipmentType::EET_Helmet:
					SetSlot(ele, HelmetSlot);
					break;
				case EEquipmentType::EET_Plate:
					SetSlot(ele, PlateSlot);
					break;
				case EEquipmentType::EET_Pistol:
					SetSlot(ele, PistolSlot);
					break;
				case EEquipmentType::EET_Rifle:
				{
					switch (ele->RifleAssign)
					{
					case ERifleSlot::ERS_Primary:
						SetSlot(ele, PrimarySlot);
						ele->SetLinkSlot(PrimarySlot);
						PrimarySlot->SettedObj = ele;
						break;
					case ERifleSlot::ERS_Sub:
						SetSlot(ele, SubSlot);
						ele->SetLinkSlot(SubSlot);
						SubSlot->SettedObj = ele;
						break;
					}					
				}

				//break;
				//case EEquipmentType::EET_Vest:
				//	SetSlot(ele, VestSlot);
				//	if (VestOverlay && CPDA->bHasStorage)
				//	{
				//		UItemStorageObject* StorageObj = Cast<UItemStorageObject>(ele);
				//		SettingStorageWidget(VestOverlay, StorageObj);
				//	}
				//	break;
				//case EEquipmentType::EET_Backpack:
				//	SetSlot(ele, BackpackSlot);
				//	if (BackpackOverlay && CPDA->bHasStorage)// && ele->EquipGridWidget)
				//	{
				//		UItemStorageObject* StorageObj = Cast<UItemStorageObject>(ele);
				//		SettingStorageWidget(BackpackOverlay, StorageObj);
				//	}
				//	break;
				}
			}
		}
	}
}

void UEquipWidget::RemoveSlot()
{
	//if (VestOverlay && BackpackOverlay && PrimarySlot && SubSlot)
	{
		/*VestOverlay->ClearChildren();
		BackpackOverlay->ClearChildren();*/
		PrimarySlot->BGBorder->ClearChildren();
		SubSlot->BGBorder->ClearChildren();
	}
	HelmetSlot->BGBorder->ClearChildren();
	PlateSlot->BGBorder->ClearChildren();
	PistolSlot->BGBorder->ClearChildren();
	PrimarySlot->BGBorder->ClearChildren();
	SubSlot->BGBorder->ClearChildren();
	/*VestSlot->BGBorder->ClearChildren();
	BackpackSlot->BGBorder->ClearChildren();*/

}

//NewVersion
void UEquipWidget::SetSlot(UNewItemObject* EquipObj, UEquipmentSlot* EquipSlot)
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
			ItemWidget->MotherEquipWidget = this;
			ItemWidget->Refresh();

			if (LootedChar_Owner != nullptr)
			{
				EquipSlot->LootedChar_Owner = LootedChar_Owner;
			}

			/**
			* Equip Slot과 EquipObj를 서로 Link한다.
			*/
			EquipSlot->SettedObj = EquipObj;
			EquipObj->RemoveLinkSlot();
			EquipObj->SetLinkSlot(EquipSlot);


			EquipSlot->BGBorder->AddChild(ItemWidget);
			EquipSlot->PaintBGBorder();


		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Eqiupwidget :: Item widget is null"));
	}
}

//OldVersion
//void UEquipWidget::SetSlot(AEquipment* Equip, UEquipmentSlot* EquipSlot)
//{
//	if (WNewItemWidget)
//	{
//		UNewItemwidget* ItemWidget = CreateWidget<UNewItemwidget>(this, WNewItemWidget);
//
//		if (ItemWidget)
//		{
//			EquipSlot->Initialize();
//			//ItemWidget->OnRemoved.AddUFunction(this, FName("RemoveEquipment"));
//			EquipSlot->BGBorder->ClearChildren();
//			//VestOverlay->ClearChildren();
//
//			
//			ItemWidget->Tilesize = 60.f;//Equip->EquipInventoryComp->TileSize; //임시로 이렇게 사이즈를 박아뒀다., GetDesiredSize는 widget이 화면에 출력되야 구할수 있는건데 ..  //EquipSlot->GetDesiredSize().X;
//			ItemWidget->ItemObj = Equip->ItemObj; // ele.Key;
//			ItemWidget->Refresh();// Equip->ItemObj, 60.f);
//			
//			if(LootedChar_Owner != nullptr)
//			{
//				EquipSlot->LootedChar_Owner = LootedChar_Owner;
//			}
//			
//			//Equip->ItemObj->SetMotherEquipSlot(EquipSlot);
//			//Equip->ItemObj->SetMotherContainer(nullptr);
//
//			EquipSlot->BGBorder->AddChild(ItemWidget);
//			EquipSlot->PaintBGBorder();
//			
//
//		}
//	}
//	else
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Eqiupwidget :: Item widget is null"));
//	}
//}

/*
void UEquipWidget::SettingStorageWidget(UOverlay* EquipOverlay, UItemStorageObject* Var_StorageObj)
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

		GridInv->OpenAdditionalWidget.AddDynamic(MainWidget,&UNewInventory::BindingOpenWidgetFunc);
		EquipOverlay->AddChild(GridInv);
		
	}
}*/

//void UEquipWidget::RemoveEquipment(UObject* T_ItemObj)
//{
//	UNewItemObject* ItemObj = Cast<UNewItemObject>(T_ItemObj);
//	if (ItemObj && EquipComp)
//	{
//		AEquipment* Equipment = Cast<AEquipment>(ItemObj->item);
//		if(Equipment)
//		{
//			Equipment->Remove();
//			EquipComp->RemoveEquipment(Equipment);
//			
//			ItemObj->bIsDestoryed = true;
//			//Equipment->Destroy();
//		}
//	}
//}