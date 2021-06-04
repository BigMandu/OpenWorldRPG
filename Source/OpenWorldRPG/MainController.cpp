// Fill out your copyright notice in the Description page of Project Settings.


#include "MainController.h"
#include "MainCharacter.h"
#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"


AMainController::AMainController()
{
	bIsInventoryVisible = false;

}

void AMainController::BeginPlay()
{
	Super::BeginPlay();
	Main = Cast<AMainCharacter>(GetCharacter());

	if (WInteractText)
	{
		InteractText = CreateWidget<UUserWidget>(this, WInteractText);
		if (InteractText)
		{
			InteractText->AddToViewport();
			InteractText->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	if (WInventory)
	{
		Inventory = CreateWidget<UUserWidget>(this, WInventory);
		if (Inventory)
		{
			Inventory->AddToViewport();
			Inventory->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AMainController::ShowInteractText_Implementation()
{
	if (InteractText) //텍스트 및 Outline 설정.
	{
		InteractText->SetVisibility(ESlateVisibility::Visible);
	}
}

void AMainController::HideInteractText_Implementation()
{
	if (InteractText)
	{
		InteractText->SetVisibility(ESlateVisibility::Hidden);
	}
	
}


void AMainController::ToggleInventory()
{
	if (bIsInventoryVisible)
	{
		HideInventory();
	}
	else
	{
		ShowInventory();
	}
}

void AMainController::ShowInventory_Implementation()
{
	if (Inventory)
	{
		Inventory->SetVisibility(ESlateVisibility::Visible);
		
		FInputModeGameAndUI Mode; ///TabKey로 HIde를 하기위해 Game And UI mode로 했는데도 키가 안먹힌다. 
		Mode.SetWidgetToFocus(Inventory->TakeWidget()); 

		SetInputMode(Mode);
		bShowMouseCursor = true;

		if (Main)
		{
			//Main->DisableInput(this); //Player의 움직임 제한. -> 입력자체를 막아버리기 때문에 Toggle을 할 수 없음.
			Main->bDisableInput = true;
		}
		SetIgnoreMoveInput(true);
		SetIgnoreLookInput(true);

		bIsInventoryVisible = true;
	}
}


void AMainController::HideInventory_Implementation()
{
	if (Inventory)
	{
		Inventory->SetVisibility(ESlateVisibility::Hidden);
		FInputModeGameOnly Mode;
		SetInputMode(FInputModeGameOnly());
		bShowMouseCursor = false;

		if (Main)
		{
			//Main->DisableInput(this); //Player의 움직임 제한. -> 입력자체를 막아버리기 때문에 Toggle을 할 수 없음.
			Main->bDisableInput = false;
		}
		SetIgnoreMoveInput(false);
		SetIgnoreLookInput(false);

		bIsInventoryVisible = false;
	}
}

