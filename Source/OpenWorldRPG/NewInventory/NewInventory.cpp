// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/NewInventory.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/DropWidget.h"
#include "OpenWorldRPG/NewInventory/LootBoxWidget.h"
#include "OpenWorldRPG/NewInventory/EquipWidget.h"
#include "OpenWorldRPG/NewInventory/CharacterInventory.h"

#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"

#include "Components/ScrollBox.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Border.h"

UNewInventory::UNewInventory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UNewInventory::NativeConstruct()
{
	Super::NativeConstruct();
	//if (PocketInventoryComp)
	//{
	//	PocketWidget->GridInitialize(PocketInventoryComp, PocketInventoryComp->TileSize);
	//	
	//}
	//if(SecureBoxInventoryComp)
	//{
	//	SecureBoxWidget->GridInitialize(SecureBoxInventoryComp, SecureBoxInventoryComp->TileSize);
	//	//GridWidget->BindDropWidget(DropWidget);
	//	
	//}

	if (EquipComp)
	{
		EquipmentWidget->EquipInitialize(EquipComp);
	}

	if (CharInvWidget)
	{
		CharInvWidget->InitializeInventory(Main);
	}
}

/* Navive Construct 보다 먼저 실행됨.*/
bool UNewInventory::Initialize()
{
	bool bResult = Super::Initialize();

	AMainCharacter* TMain = Cast<AMainCharacter>(GetOwningPlayerPawn());
	Main = (TMain == nullptr) ? nullptr : TMain;
	if (Main) 
	{
		/*if (Main->PocketInventoryComp)
		{
			PocketInventoryComp = Main->PocketInventoryComp;
		}
		if(Main->SecureBoxInventoryComp)
		{
			SecureBoxInventoryComp = Main->SecureBoxInventoryComp;
		}*/

		EquipComp = Main->Equipment;
		//GridWidget->GridInitialize(InventoryComp, InventoryComp->TileSize);
	}
	return bResult;
}

void UNewInventory::SetRightWidget(UUserWidget* Widget)
{
	ContentBorder->ClearChildren();
	if (Widget)
	{
		//RightWidgetScrollBox->ClearChildren();
		//RightWidgetScrollBox->AddChild(Widget);
		ContentBorder->AddChild(Widget);
		Widget->SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp, Warning, TEXT("Inventory::SetRightWidget, Widget name : %s"), *Widget->GetFName().ToString());
		//LootWidget->SetVisibility(ESlateVisibility::Visible);
		//RightWidgetSwitcher->SetActiveWidget(Widget);
	}
}

void UNewInventory::ChangeRightSwitcher()
{
	if (Main)
	{
		AMainController* MainCon = Main->MainController;
		if (MainCon)
		{
			if (MainCon->bIsInteractLootBox || MainCon->bIsInteractCharacterLoot)
			{
				RightWidgetSwitcher->SetActiveWidgetIndex(2);
			}
			else
			{
				SetRightWidget(nullptr);
				RightWidgetSwitcher->SetActiveWidgetIndex(1);
			}
		}
	}
}