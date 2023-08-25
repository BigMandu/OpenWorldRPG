// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/UI/Inventory/CharacterLootWidget.h"
#include "OpenWorldRPG/UI/Inventory/EquipWidget.h"
#include "OpenWorldRPG/UI/Inventory/EquipStorageWidget.h"
#include "OpenWorldRPG/UI/Inventory/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/BaseCharacter.h"
//#include "OpenWorldRPG/EnemyCharacter.h"



bool UCharacterLootWidget::Initialize()
{
	bool bReturn = Super::Initialize();

	return bReturn;
}

void UCharacterLootWidget::InitCharLootWidget(ABaseCharacter* DeadChar)
{
	if(DeadChar && EquipWidget)
	{
		if(DeadChar->Equipment)
		{
			EquipStorageWidget->LootedChar_Owner = DeadChar;
			EquipStorageWidget->EquipInitialize(DeadChar->Equipment);
			EquipStorageWidget->InitializeInventory(DeadChar);
			EquipStorageWidget->RefreshEquipWidget();
			

			EquipWidget->LootedChar_Owner = DeadChar;
			EquipWidget->EquipInitialize(DeadChar->Equipment);
			//EquipWidget->InitializeInventory(DeadChar);
			EquipWidget->RefreshEquipWidget();
		}
	}
}

