// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"
#include "OpenWorldRPG/Item/Item.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/Item/InventoryComponent.h"
#include "OpenWorldRPG/Item/EquipmentComponent.h"
#include "OpenWorldRPG/Item/LootBox.h"
#include "Blueprint/UserWidget.h"
#include "Components/Overlay.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"

UInventoryWidget::UInventoryWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	
}

bool UInventoryWidget::Initialize()
{
	bool bReturn = Super::Initialize();

	Primary = GetWidgetFromName(FName("PrimaryWeapon"));
	Sub = GetWidgetFromName(FName("SubWeapon"));
	Helmet = GetWidgetFromName(FName("Helmet"));
	Vest = GetWidgetFromName(FName("Vest")); 
	Pistol = GetWidgetFromName(FName("Pistol"));

	AMainCharacter* TMain = Cast<AMainCharacter>(GetOwningPlayerPawn());
	Main = (TMain == nullptr) ? nullptr : TMain;

	if (Main)
	{
		if (Main->Inventory && Main->Equipment)
		{
			InventoryComp = Main->Inventory;
			EquipmentComp = Main->Equipment;

			InventoryComp->OnInventoryUpdated.AddDynamic(this, &UInventoryWidget::InitInventory);
			EquipmentComp->OnEquipmentUpdated.AddDynamic(this, &UInventoryWidget::InitEquipment);
		}

		/*if (Main->InteractLootBox)
		{
			LootBox = Main->InteractLootBox;
			LootBox->LootBoxOpen.AddDynamic(this, &UInventoryWidget::SetLootBox);
		}*/
	}

	return bReturn;
}

void UInventoryWidget::InitInventory()
{
	//UE_LOG(LogTemp, Warning, TEXT("InitInventory"));
	UWrapBox* InventoryBox = Cast<UWrapBox>(GetWidgetFromName(TEXT("InventoryWarpBox")));
	if (InventoryBox)
	{
		InventoryBox->ClearChildren();

		for (auto InvItem : InventoryComp->InventoryItems)
		{
			AItem* Item = Cast<AItem>(InvItem);

			if (Main && Item)
			{
				AMainController* MainCon = Main->MainController;
				if (MainCon && MainCon->WInventoryItems)
				{
					//UE_LOG(LogTemp, Warning, TEXT("MainCon is valid"));
					InventoryItemsWidget = CreateWidget<UUserWidget>(this, MainCon->WInventoryItems);
					
					FObjectProperty* Obj = FindFProperty<FObjectProperty>(InventoryItemsWidget->GetClass(), FName("Item"));

					if (Obj != nullptr)
					{						
						Obj->SetPropertyValue_InContainer(InventoryItemsWidget, Item);
						InventoryBox->AddChildToWrapBox(InventoryItemsWidget);
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Cast fail"));
			}
		}
	}
}

void UInventoryWidget::InitEquipment()
{
	UE_LOG(LogTemp, Warning, TEXT("InitEquipment"));

	
	bool bIsValid = CheckSlotIsValid();

	if (!bIsValid)
	{
		UE_LOG(LogTemp, Warning, TEXT("Slot is invalid"));
		return;
	}
	

	for (auto EquipItem : EquipmentComp->EquipmentItems)
	{
		AWeapon* EquipWeap = Cast<AWeapon>(EquipItem);
		if (EquipWeap)
		{
			switch (EquipWeap->WeaponType)
			{
			case EWeaponType::EWT_Rifle:
				if (CheckEquipSlotIsSet(Primary))
				{
					SetEquipmentSlot(Sub, EquipWeap);
				}
				else
				{
					SetEquipmentSlot(Primary, EquipWeap);
				}
				break;
			case EWeaponType::EWT_Pistol:
				//CheckEquipSlotIsSet(Pistol);
				SetEquipmentSlot(Pistol, EquipWeap);
				break;
			case EWeaponType::EWT_Helmet:
				SetEquipmentSlot(Helmet, EquipWeap);
				break;
			case EWeaponType::EWT_Vest:
				SetEquipmentSlot(Vest, EquipWeap);
				break;
			}
		}
	}
}

bool UInventoryWidget::CheckSlotIsValid()
{
	if (Primary && Sub && Helmet && Vest && Pistol)
	{
		return true;
	}
	else return false;
}

bool UInventoryWidget::CheckEquipSlotIsSet(UWidget* EquipSlot)
{
	UE_LOG(LogTemp, Warning, TEXT("Slot name is %s"), *EquipSlot->GetFName().ToString());

	UUserWidget* EquipSlotWidget = Cast<UUserWidget>(EquipSlot);
	if (EquipSlotWidget)
	{
		UTextBlock* TBlock = Cast<UTextBlock>(EquipSlotWidget->GetWidgetFromName(FName("EquipItemName")));
		if (TBlock)
		{
			return !(TBlock->GetText().IsEmpty()); //TextBlock이 비어있으면 해당슬롯은 세팅 되어있는 상태임.
			//UE_LOG(LogTemp, Warning, TEXT("TBlock context is %s"), *TBlock->GetText().ToString());
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Error, InventoryWidget::CheckEquipSlotIsSet "));
	return false;
}

void UInventoryWidget::SetEquipmentSlot(UWidget* EquipSlot, AWeapon* Weap)
{
	if (EquipSlot && Weap)
	{
		UUserWidget* EquipSlotWidget = Cast<UUserWidget>(EquipSlot);
		if (EquipSlotWidget)
		{
			UImage* Thumb = Cast<UImage>(EquipSlotWidget->GetWidgetFromName(FName("EquipItemThumbnail")));
			UTextBlock* TBlock = Cast<UTextBlock>(EquipSlotWidget->GetWidgetFromName(FName("EquipItemName")));
			if (Thumb && TBlock)
			{
				//Thumb->SetBrushFromTexture(Weap->Thumbnail);
				TBlock->SetText(Weap->ItemName);
			}
		}
	}
}

void UInventoryWidget::SetLootBoxWidget()
{
	UE_LOG(LogTemp, Warning, TEXT("InvWidget::SetLootBoxWidget"));

	LootBoxTEXT = Cast<UTextBlock>(GetWidgetFromName(FName("NoInteractTEXT")));

	if (Main->InteractLootBox)
	{
		LootBox = Main->InteractLootBox;
		if (Main->MainController)
		{
			AMainController* MainCon = Main->MainController;
			MainCon->CreateLootWidget();

			if (MainCon->LootBoxWidget)
			{
				UE_LOG(LogTemp, Warning, TEXT("InvWidget::Get LootBox Overlay Widget..."));
				LootOverlay = Cast<UOverlay>(GetWidgetFromName(FName("LootBoxOverlay")));
				if (LootOverlay != nullptr && LootBoxTEXT)
				{
					UE_LOG(LogTemp, Warning, TEXT("InvWidget::Success, AddChild LootBox Widget To Overlay"));
					LootBoxTEXT->SetVisibility(ESlateVisibility::Hidden);
					LootOverlay->AddChildToOverlay(MainCon->LootBoxWidget);
				}
			}
		}
	}	
	else
	{
		LootBoxTEXT->SetVisibility(ESlateVisibility::Visible);
	}
}