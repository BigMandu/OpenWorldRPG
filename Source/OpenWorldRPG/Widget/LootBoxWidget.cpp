// Fill out your copyright notice in the Description page of Project Settings.


#include "LootBoxWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/WrapBox.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"
#include "OpenWorldRPG/Item/Item.h"
#include "OpenWorldRPG/Item/LootBox.h"
#include "OpenWorldRPG/Item/InventoryComponent.h"

bool ULootBoxWidget::Initialize()
{
	UE_LOG(LogTemp, Warning, TEXT("Initialize LootBox widget"));
	bool bReturn = Super::Initialize();

	AMainCharacter* TMain = Cast<AMainCharacter>(GetOwningPlayerPawn());
	Main = (TMain == nullptr) ? nullptr : TMain;

	
	if (Main)
	{
		if (Main->InteractLootBox)//수정 해야함
		{
			UE_LOG(LogTemp, Warning, TEXT("LootBox valid"));
			InteractBox = Main->InteractLootBox;
			if (InteractBox)
			{
				InteractBox->BoxInvComp->OnInventoryUpdated.AddDynamic(this, &ULootBoxWidget::InitLootBox);
				UE_LOG(LogTemp, Warning, TEXT("call InitLootBox func"));
				InitLootBox();
			}
		}
	}
	
	return bReturn;
}


void ULootBoxWidget::InitLootBox()
{
	UE_LOG(LogTemp, Warning, TEXT("init lootbox"));
	UWrapBox* LootWrapBox = Cast<UWrapBox>(GetWidgetFromName(TEXT("LootWarpBox")));
	if (LootWrapBox)
	{
		LootWrapBox->ClearChildren();

		
		for (auto InvItem : InteractBox->BoxInvComp->InventoryItems)
		{
			AItem* Item = Cast<AItem>(InvItem);

			if (Main && Item)
			{
				AMainController* MainCon = Main->MainController;
				if (MainCon && MainCon->WInventoryItems)
				{
					//UE_LOG(LogTemp, Warning, TEXT("MainCon is valid"));
					InventoryItemsWidget = CreateWidget<UUserWidget>(this, MainCon->WInventoryItems);

					FObjectProperty* Obj = FindFProperty<FObjectProperty>(InventoryItemsWidget->GetClass(), FName("Item"));

					if (Obj != nullptr)
					{
						Obj->SetPropertyValue_InContainer(InventoryItemsWidget, Item);
						LootWrapBox->AddChildToWrapBox(InventoryItemsWidget);
					}
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Cast fail"));
			}
		}
	}
}