// Fill out your copyright notice in the Description page of Project Settings.


#include "LootBoxWidget.h"
#include "Blueprint/UserWidget.h"
#include "OpenWorldRPG/Item/Item.h"
#include "OpenWorldRPG/Item/LootBox.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"


bool ULootBoxWidget::Initialize()
{
	//UE_LOG(LogTemp, Warning, TEXT("LootBoxWidget::Initialize LootBox widget"));
	bool bReturn = Super::Initialize();
	
	return bReturn;
}

void ULootBoxWidget::InitLootBoxWidget(AActor* actor)
{
	LootBox = Cast<ALootBox>(actor);
	if (LootBox)
	{
		LootBoxGridwidget->GridInitialize(LootBox->BoxInventoryComp, LootBox->BoxInventoryComp->TileSize);

	}
}