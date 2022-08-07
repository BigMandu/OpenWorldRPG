// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/NewInventory/Widget/NewInventory.h"
#include "OpenWorldRPG/NewInventory/Widget/EquipWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/CharacterInventoryWidget.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/Widget/DropWidget.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"
//#include "OpenWorldRPG/NewInventory/ContainerWidget.h"

#include "Blueprint/DragDropOperation.h"
#include "Components/ScrollBox.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Border.h"

UNewInventory::UNewInventory(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{

}

void UNewInventory::NativeConstruct()
{
	Super::NativeConstruct();
	//if (PocketInventoryComp)
	//{
	//	PocketWidget->GridInitialize(PocketInventoryComp, PocketInventoryComp->TileSize);
	//	
	//}
	//if(SecureBoxInventoryComp)
	//{
	//	SecureBoxWidget->GridInitialize(SecureBoxInventoryComp, SecureBoxInventoryComp->TileSize);
	//	//GridWidget->BindDropWidget(DropWidget);
	//	
	//}

	if (EquipComp)
	{
		EquipmentWidget->EquipInitialize(EquipComp);
	}

	//MainCharacter의 PostInit에서 호출하는것으로 변경함.
	/*if (CharInvWidget)
	{
		CharInvWidget->InitializeInventory(Main);
	}*/
}

/* Navive Construct 보다 먼저 실행됨.*/
bool UNewInventory::Initialize()
{
	bool bResult = Super::Initialize();

	AMainCharacter* TMain = Cast<AMainCharacter>(GetOwningPlayerPawn());
	Main = (TMain == nullptr) ? nullptr : TMain;
	if (Main)
	{
		/*if (Main->PocketInventoryComp)
		{
			PocketInventoryComp = Main->PocketInventoryComp;
		}
		if(Main->SecureBoxInventoryComp)
		{
			SecureBoxInventoryComp = Main->SecureBoxInventoryComp;
		}*/

		EquipComp = Main->Equipment;
		//GridWidget->GridInitialize(InventoryComp, InventoryComp->TileSize);
	}
	return bResult;
}

void UNewInventory::SetRightWidget(UUserWidget* Widget)
{
	ContentBorder->ClearChildren();
	if (Widget)
	{
		//RightWidgetScrollBox->ClearChildren();
		//RightWidgetScrollBox->AddChild(Widget);
		ContentBorder->AddChild(Widget);
		Widget->SetVisibility(ESlateVisibility::Visible);
		UE_LOG(LogTemp, Warning, TEXT("Inventory::SetRightWidget, Widget name : %s"), *Widget->GetFName().ToString());
		//LootWidget->SetVisibility(ESlateVisibility::Visible);
		//RightWidgetSwitcher->SetActiveWidget(Widget);
	}
}

void UNewInventory::ChangeRightSwitcher()
{
	if (Main)
	{
		AMainController* MainCon = Main->MainController;
		if (MainCon)
		{
			if (MainCon->bIsInteractLootBox || MainCon->bIsInteractCharacterLoot)
			{
				RightWidgetSwitcher->SetActiveWidgetIndex(2);
			}
			else
			{
				SetRightWidget(nullptr);
				RightWidgetSwitcher->SetActiveWidgetIndex(1);
			}
		}
	}
}

bool UNewInventory::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	bool bReturn = Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UNewItemObject* ItemObj = Cast< UNewItemObject>(InOperation->Payload);
	if (ItemObj)
	{
		if (ItemObj->bIsDragging)
		{
			ItemObj->bIsDragging = false;
			UCustomInventoryLibrary::BackToItem(ItemObj);
		}
	}
	return bReturn;
}