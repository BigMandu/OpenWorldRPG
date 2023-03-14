// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/Widget/CharacterLootWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/EquipWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/EquipStorageWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventoryGrid.h"
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

