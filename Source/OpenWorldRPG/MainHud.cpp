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
		HideOtherUI();

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
		ShowOtherUI();
		//닫을때 마다 HighestZ를 2로 초기화 한다.
		NewInventoryWidget->HighestZ = 2;
		
		NewInventoryWidget->CloseAllAdditionalWidget(NewInventoryWidget->OpenedWindowArray.Num());
		bIsInventoryWindowShow = false;		
		NewInventoryWidget->SetVisibility(ESlateVisibility::Collapsed);				
	}
}

void UMainHud::ShowOtherUI()
{
	ResourceWidget->SetVisibility(ESlateVisibility::Visible);
	WeaponStatus->SetVisibility(ESlateVisibility::Visible);
	//QuickSlot->SetVisibility(ESlateVisibility::Visible);
}

void UMainHud::HideOtherUI()
{
	ResourceWidget->SetVisibility(ESlateVisibility::Collapsed);
	WeaponStatus->SetVisibility(ESlateVisibility::Collapsed);
	//QuickSlot->SetVisibility(ESlateVisibility::Collapsed);
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