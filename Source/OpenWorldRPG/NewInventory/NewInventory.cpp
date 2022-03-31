// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/NewInventory.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/DropWidget.h"
#include "OpenWorldRPG/NewInventory/LootBoxWidget.h"
#include "OpenWorldRPG/NewInventory/EquipWidget.h"

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
	if (InventoryComp)
	{
		GridWidget->GridInitialize(InventoryComp, InventoryComp->TileSize);
		GridWidget->BindDropWidget(DropWidget);
		EquipWidget->EquipInitialize(EquipComp);
	}
}

/* Navive Construct 보다 먼저 실행됨.*/
bool UNewInventory::Initialize()
{
	bool bResult = Super::Initialize();

	AMainCharacter* TMain = Cast<AMainCharacter>(GetOwningPlayerPawn());
	Main = (TMain == nullptr) ? nullptr : TMain;
	if (Main && Main->InventoryComp)
	{
		InventoryComp = Main->InventoryComp;
		EquipComp = Main->Equipment;
		//GridWidget->GridInitialize(InventoryComp, InventoryComp->TileSize);
	}
	return bResult;
}

void UNewInventory::SetRightWidget(UUserWidget* Widget)
{
	if (Widget)
	{
		//RightWidgetScrollBox->ClearChildren();
		//RightWidgetScrollBox->AddChild(Widget);
		
		ContentBorder->ClearChildren();
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
			if (MainCon->bIsInteractLootBox)
			{
				RightWidgetSwitcher->SetActiveWidgetIndex(2);
			}
			else
			{
				RightWidgetSwitcher->SetActiveWidgetIndex(1);
			}
		}
	}
}