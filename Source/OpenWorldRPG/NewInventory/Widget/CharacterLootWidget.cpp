// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/Widget/CharacterLootWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/CharacterInventoryWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/EquipWidget.h"
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
	//ABaseCharacter* BChar = Cast<ABaseCharacter>(DeadChar);
	//AEnemyCharacter* AIChar = Cast<AEnemyCharacter>(DeadChar);
	if(DeadChar && EquipWidget && CharInvWidget)
	{
		if(DeadChar->Equipment)
		{
			EquipWidget->LootedChar_Owner = DeadChar;
			EquipWidget->EquipInitialize(DeadChar->Equipment);
			
			EquipWidget->RefreshEquipWidget();

			//Widget ¼ÕºÁ¾ßÇÔ. Pocket, SecureBoxÃß°¡ÇØ¾ßµÊ.
			//InvenWidget->GridInitialize(DeadChar->PocketInventoryComp, DeadChar->PocketInventoryComp->TileSize);
		}
		CharInvWidget->InitializeInventory(DeadChar);
		//EquipWidget->RefreshEquipWidget();
	}
}

