// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/Widget/CharacterInventoryWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/BaseCharacter.h"

bool UCharacterInventoryWidget::Initialize()
{
	bool bReturn = Super::Initialize();
	return bReturn;
}

void UCharacterInventoryWidget::InitializeInventory(ABaseCharacter* BChar)
{
	PocketWidget->StorageObj = BChar->PocketStorage;
	SecureBoxWidget->StorageObj = BChar->SecureBoxStorage;

	PocketWidget->GridInit();
	SecureBoxWidget->GridInit();
	/*if (BChar->PocketInventoryComp)
	{
		PocketInvComp = BChar->PocketInventoryComp;
		PocketWidget->GridInitialize(PocketInvComp, PocketInvComp->TileSize);
	}

	if (BChar->SecureBoxInventoryComp)
	{
		SecureBoxInvComp = BChar->SecureBoxInventoryComp;
		SecureBoxWidget->GridInitialize(SecureBoxInvComp, SecureBoxInvComp->TileSize);
	}*/

}