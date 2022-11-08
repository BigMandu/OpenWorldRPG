// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/MainHud.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventory.h"
#include "OpenWorldRPG/MainController.h"

void UMainHud::NativeConstruct()
{
	if (WInteractText)
	{
		InteractText = CreateWidget<UUserWidget>(this, WInteractText);
		if (InteractText)
		{
			InteractText->AddToViewport();
			InteractText->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

void UMainHud::ShowInventoryWindow()
{
	if (NewInventoryWidget)
	{

		FInputModeGameAndUI Mode; ///TabKey로 HIde를 하기위해 Game And UI mode로 했는데도 키가 안먹힌다. 
		Mode.SetWidgetToFocus(NewInventoryWidget->TakeWidget());
		MainCon->SetInputMode(Mode);

		bIsInventoryWindowShow = true;
		NewInventoryWidget->ChangeRightSwitcher();
		NewInventoryWidget->SetVisibility(ESlateVisibility::Visible);
	}
}
void UMainHud::HideInventoryWindow()
{
	if (NewInventoryWidget)
	{
		bIsInventoryWindowShow = false;		
		NewInventoryWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMainHud::ShowInteractText()
{
	if (InteractText)
	{
		InteractText->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMainHud::HideInteractText()
{
	if (InteractText)
	{
		InteractText->SetVisibility(ESlateVisibility::Collapsed);
	}
}