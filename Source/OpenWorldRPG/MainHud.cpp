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
	if(CompassWidget)
	{
		CompassWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UMainHud::ShowInventoryWindow()
{
	if (NewInventoryWidget)
	{
		//Inventory�� ���� ǥ�õ� widget�� hide�Ѵ�.
		HideOtherUI();

		FInputModeGameAndUI Mode; ///TabKey�� HIde�� �ϱ����� Game And UI mode�� �ߴµ��� Ű�� �ȸ�����. 
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
		//Inventory�� ������ ǥ���ؾ��� widget�� show�Ѵ�.
		ShowOtherUI();
		//������ ���� HighestZ�� 2�� �ʱ�ȭ �Ѵ�.
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

	if (bIsCompassWidgetVisible)
	{
		CompassWidget->SetVisibility(ESlateVisibility::Visible);
	}
	//QuickSlot->SetVisibility(ESlateVisibility::Visible);
}

void UMainHud::HideOtherUI()
{
	ResourceWidget->SetVisibility(ESlateVisibility::Collapsed);
	WeaponStatus->SetVisibility(ESlateVisibility::Collapsed);
	//QuickSlot->SetVisibility(ESlateVisibility::Collapsed);

	if (bIsCompassWidgetVisible)
	{
		CompassWidget->SetVisibility(ESlateVisibility::Collapsed);
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

//for Show/Hide OtherUI function
void UMainHud::SettingIsCompassWidgetVisible(bool bIsVisible)
{
	bIsCompassWidgetVisible = bIsVisible;
}