// Fill out your copyright notice in the Description page of Project Settings.


#include "MainController.h"
#include "MainCharacter.h"
#include "Item/LootBox.h"
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

	/* Loot Box Widget TEST */
	/*
	if (WLootBoxInvWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("WLootBoxWidget is valid"));
		LootBoxInvWidget = CreateWidget<UUserWidget>(this, WLootBoxInvWidget);
		if (LootBoxInvWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("LootBoxWidget is valid"));

			
			//LootBoxInvWidget->SetPositionInViewport(FVector2D(500.f, 300.f));
			LootBoxInvWidget->SetAnchorsInViewport(FAnchors(1000.f, 500.f));
			LootBoxInvWidget->SetDesiredSizeInViewport(FVector2D(500.f));
			LootBoxInvWidget->AddToViewport();
			LootBoxInvWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	*/
}

void AMainController::ShowInteractText_Implementation()
{
	if (InteractText) //�ؽ�Ʈ �� Outline ����.
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
		
		FInputModeGameAndUI Mode; ///TabKey�� HIde�� �ϱ����� Game And UI mode�� �ߴµ��� Ű�� �ȸ�����. 
		Mode.SetWidgetToFocus(Inventory->TakeWidget()); 

		SetInputMode(Mode);
		bShowMouseCursor = true;

		if (Main)
		{
			//Main->DisableInput(this); //Player�� ������ ����. -> �Է���ü�� ���ƹ����� ������ Toggle�� �� �� ����.
			Main->bDisableInput = true;

			if (Main->InteractLootBox)
			{
				ShowLootBoxWidget();
			}
			
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
			/* 
				LootBox�� ��ȣ�ۿ� �߾��ٸ� 
				��ȣ�ۿ� ���� LootBox���� Close Box�� ȣ���Ѵ�.
			*/
			if (Main->InteractLootBox)
			{
				HideLootBoxWidget();
				Main->InteractLootBox->CloseBox(Main);
			}

			//Main->DisableInput(this); //Player�� ������ ����. -> �Է���ü�� ���ƹ����� ������ Toggle�� �� �� ����.
			Main->bDisableInput = false;

		
		}
		SetIgnoreMoveInput(false);
		SetIgnoreLookInput(false);

		bIsInventoryVisible = false;
	}
}

void AMainController::CreateLootWidget()
{
	if (WLootBoxWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("WLootBoxWidget is valid"));
		LootBoxWidget = CreateWidget<UUserWidget>(this, WLootBoxWidget);
		if (LootBoxWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("LootBoxWidget is valid"));

			UE_LOG(LogTemp, Warning, TEXT("MainCon::CreateLootWidget() is success"));
			//LootBoxInvWidget->SetPositionInViewport(FVector2D(500.f, 300.f));
			//LootBoxInvWidget->SetAnchorsInViewport(FAnchors(1000.f, 500.f));
			//LootBoxInvWidget->SetDesiredSizeInViewport(FVector2D(500.f));
			//LootBoxInvWidget->AddToViewport();
			LootBoxWidget->SetVisibility(ESlateVisibility::Hidden);
			//LootBoxWidget->Initialize();
		}
	}
}

void AMainController::ShowLootBoxWidget()
{
	if (LootBoxWidget)
	{
		LootBoxWidget->SetVisibility(ESlateVisibility::Visible);
	}
}
void AMainController::HideLootBoxWidget()
{
	if (LootBoxWidget)
	{
		LootBoxWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	
}