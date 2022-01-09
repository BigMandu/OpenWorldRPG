// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/NewInventory.h"
#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"

UNewInventory::UNewInventory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UNewInventory::NativeConstruct()
{
	Super::NativeConstruct();

	GridWidget->GridInitialize(InventoryComp, TileSize);
}

bool UNewInventory::Initialize()
{
	bool bResult = Super::Initialize();

	AMainCharacter* TMain = Cast<AMainCharacter>(GetOwningPlayerPawn());
	Main = (TMain == nullptr) ? nullptr : TMain;
	if (Main)
	{
		InventoryComp = Main->NewInventory;
		InventoryComp->Columns = 10;
		InventoryComp->Rows = 10;
		TileSize = 50.f;


	}
	
	/*if (bResult)
	{
		AMainCharacter* TMain = Cast<AMainCharacter>(GetOwningPlayerPawn());
		Main = (TMain == nullptr) ? nullptr : TMain;
		if(Main)
		{
			InventoryComp = Main->NewInventory;
			InventoryComp->Columns = 10;
			InventoryComp->Rows = 10;
			TileSize = 50.f;
			
			GridWidget->GridInitialize(InventoryComp, TileSize);
			return bResult;
		}
	}*/
	return bResult;
}