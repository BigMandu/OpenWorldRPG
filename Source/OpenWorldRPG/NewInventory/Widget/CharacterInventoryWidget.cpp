// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/Widget/CharacterInventoryWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventory.h"
#include "OpenWorldRPG/NewInventory/Widget/AdditionalWidget.h"

#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"

#include "OpenWorldRPG/BaseCharacter.h"

bool UCharacterInventoryWidget::Initialize()
{
	bool bReturn = Super::Initialize();
	return bReturn;
}

void UCharacterInventoryWidget::InitializeInventory(ABaseCharacter* BChar)//, UNewInventory* MainWidget)
{
	PocketWidget->StorageObj = BChar->PocketStorage;
	SecureBoxWidget->StorageObj = BChar->SecureBoxStorage;

	PocketWidget->GridInit();
	SecureBoxWidget->GridInit();


	
	
	//PocketWidget->OpenAdditionalWidget.AddDynamic(AdditionalWidget, &UAdditionalWidget::BindingOpenWidgetFunction);
	//SecureBoxWidget->OpenAdditionalWidget.AddDynamic(AdditionalWidget, &UAdditionalWidget::BindingOpenWidgetFunction);
	
	
	//PocketWidget->OpenAdditionalWidget.AddDynamic(this,&UAddi)
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

void UCharacterInventoryWidget::BindingAdditional(class UNewInventory* MainWidget)
{
	PocketWidget->OpenAdditionalWidget.AddDynamic(MainWidget, &UNewInventory::BindingOpenWidgetFunc);
	SecureBoxWidget->OpenAdditionalWidget.AddDynamic(MainWidget, &UNewInventory::BindingOpenWidgetFunc);
}