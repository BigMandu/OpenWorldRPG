// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/CharacterInventory.h"
#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/BaseCharacter.h"

bool UCharacterInventory::Initialize()
{
	bool bReturn = Super::Initialize();
	return bReturn;
}

void UCharacterInventory::InitializeInventory(ABaseCharacter* BChar)
{
	if (BChar->PocketInventoryComp)
	{
		PocketInvComp = BChar->PocketInventoryComp;
		PocketWidget->GridInitialize(PocketInvComp, PocketInvComp->TileSize);
	}

	if (BChar->SecureBoxInventoryComp)
	{
		SecureBoxInvComp = BChar->SecureBoxInventoryComp;
		PocketWidget->GridInitialize(SecureBoxInvComp, SecureBoxInvComp->TileSize);
	}

}