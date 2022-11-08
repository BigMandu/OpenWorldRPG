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
		//Show 일때 InputMode는 MainHud에서 직접해준다.

		bShowMouseCursor = true;
		//Main->DisableInput(this); //Player의 움직임 제한. -> 입력자체를 막아버리기 때문에 Toggle을 할 수 없음.
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