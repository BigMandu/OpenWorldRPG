// Fill out your copyright notice in the Description page of Project Settings.


#include "MainController.h"
#include "MainCharacter.h"
#include "Item/Container.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventory.h"
#include "OpenWorldRPG/UI/QuickSlotWidget.h"
#include "OpenWorldRPG/MainHud.h"



AMainController::AMainController()
{
	bIsInventoryVisible = false;

}

void AMainController::BeginPlay()
{
	Super::BeginPlay();
	Main = Cast<AMainCharacter>(GetCharacter());

	if (WMainHud)
	{
		MainHud = CreateWidget<UMainHud>(this,WMainHud);
		MainHud->AddToViewport();
		MainHud->SetVisibility(ESlateVisibility::Visible);
		MainHud->MainCon = this;
	}


}

void AMainController::ToggleInventory()
{
	if (MainHud->bIsInventoryWindowShow)
	{
		/* Hide Inventory */
		//HideInventory();
		MainHud->HideInventoryWindow();
		SetInputAndFocus(false);
	}
	else
	{
		/* Show Inventory */
		//ShowInventory();
		MainHud->ShowInventoryWindow();
		SetInputAndFocus(true);

	}
}

void AMainController::ControlInteractText(bool bIsInteract)
{
	if (bIsInteract)
	{
		MainHud->ShowInteractText();
	}
	else
	{
		MainHud->HideInteractText();
	}
}

void AMainController::SetInputAndFocus(bool bIsShow)
{
	if (bIsShow) //Set To UI Mode
	{
		//Show �϶� InputMode�� MainHud���� �������ش�.

		bShowMouseCursor = true;
		//Main->DisableInput(this); //Player�� ������ ����. -> �Է���ü�� ���ƹ����� ������ Toggle�� �� �� ����.
		Main->bDisableInput = true;
		SetIgnoreMoveInput(true);
		SetIgnoreLookInput(true);
		bIsInventoryVisible = true;
	}
	else //Return To GameMode
	{
		Main->bDisableInput = false;

		FInputModeGameOnly Mode;
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;

		SetIgnoreMoveInput(false);
		SetIgnoreLookInput(false);
		bIsInventoryVisible = false;
		bIsInteractLootBox = false;
		bIsInteractCharacterLoot = false;
	}
}

//void AMainController::UseQuickSlotItem(EQuickSlotNumber QuickSlotNum)
//{
//	OnQuickSlotUse.Broadcast(QuickSlotNum);
//	//MainHud->QuickSlot->UseItemInQuickSlot(QuickSlotNum);
//}