// Fill out your copyright notice in the Description page of Project Settings.


#include "ContainerWidget.h"
#include "Blueprint/UserWidget.h"
#include "OpenWorldRPG/Vehicles/NiceCar.h"
#include "OpenWorldRPG/Item/Item.h"
#include "OpenWorldRPG/Item/Container.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/UI/Inventory/NewInventoryGrid.h"


bool UContainerWidget::Initialize()
{
	//UE_LOG(LogTemp, Warning, TEXT("LootBoxWidget::Initialize LootBox widget"));
	bool bReturn = Super::Initialize();
	
	return bReturn;
}

void UContainerWidget::InitContainerWidget(AActor* actor)
{
	ANiceCar* VehicleType = Cast<ANiceCar>(actor);
	Container = Cast<AContainer>(actor);
	if (Container)
	{
		ContainerGridwidget->StorageObj = Container->ContainerStorage;
		ContainerGridwidget->GridInit();

		//ContainerGridwidget->GridInitialize(Container->ContainerInventoryComp, Container->ContainerInventoryComp->TileSize);

	}
	else if (VehicleType)
	{
		ContainerGridwidget->StorageObj = VehicleType->StorageObj;
		ContainerGridwidget->GridInit();
	}
}