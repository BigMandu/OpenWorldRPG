// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/CharacterLootWidget.h"

#include "NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/EquipWidget.h"
#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"

#include "OpenWorldRPG/BaseCharacter.h"
//#include "OpenWorldRPG/EnemyCharacter.h"

void UCharacterLootWidget::InitCharLootWidget(ABaseCharacter* DeadChar)
{
	//ABaseCharacter* BChar = Cast<ABaseCharacter>(DeadChar);
	//AEnemyCharacter* AIChar = Cast<AEnemyCharacter>(DeadChar);
	if(DeadChar && EquipWidget)
	{
		if(DeadChar->Equipment)
		{
			EquipWidget->EquipInitialize(DeadChar->Equipment);
			EquipWidget->RefreshEquipWidget();

			//Widget �պ�����. Pocket, SecureBox�߰��ؾߵ�.
			InvenWidget->GridInitialize(DeadChar->PocketInventoryComp, DeadChar->PocketInventoryComp->TileSize);
		}
	}
}

