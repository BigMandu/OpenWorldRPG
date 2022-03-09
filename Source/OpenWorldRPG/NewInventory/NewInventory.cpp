// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/NewInventory.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/DropWidget.h"
#include "OpenWorldRPG/NewInventory/LootBoxWidget.h"

#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"

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
		//GridWidget->GridInitialize(InventoryComp, InventoryComp->TileSize);
	}
	return bResult;
}

void UNewInventory::SetRightWidget(UUserWidget* Widget)
{
	if (Widget)
	{
		ContentBorder->AddChild(Widget);
		RightWidgetSwitcher->SetActiveWidgetIndex(2);
		//LootWidget->SetVisibility(ESlateVisibility::Visible);
		//RightWidgetSwitcher->SetActiveWidget(Widget);
	}
}

void UNewInventory::ChangeRightSwitcher()
{

}