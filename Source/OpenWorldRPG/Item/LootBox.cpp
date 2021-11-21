// Fill out your copyright notice in the Description page of Project Settings.


#include "LootBox.h"
#include "Item.h"
#include "InventoryComponent.h"
#include "Blueprint/UserWidget.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"

ALootBox::ALootBox()
{
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);

	BoxInvComp = CreateDefaultSubobject<UInventoryComponent>(TEXT("BoxInventory"));
	
	LootItemCount = 1;
}

void ALootBox::BeginPlay()
{
	Super::BeginPlay();


	if (LootItem_1 && LootItem_2)
	{
		BoxItem.Add(LootItem_1);
		BoxItem.Add(LootItem_2);
	}
	SelectAndStoreLootItem();
}

void ALootBox::SelectAndStoreLootItem()
{
	int32 BoxMaxNum = BoxItem.Num() - 1;
	if (BoxMaxNum > 0)
	{
		for (int32 i = 0; i < LootItemCount; ++i)
		{
			int32 Select = FMath::RandRange(0, BoxMaxNum);
			if (BoxItem[Select] != nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("LootBox::Select"));
				AItem* Item = Cast<AItem>(BoxItem[Select].GetDefaultObject());
				if (Item)
				{
					UE_LOG(LogTemp, Warning, TEXT("LootBox::Call AddItem"));
					BoxInvComp->AddItem(Item);
				}
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("LootBox::Select fail"));
			}
		}
	}
	
}

void ALootBox::OpenBox(AActor* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("LootBox::OpenBox"));
	
	Main = Cast<AMainCharacter>(Actor);
	if (Main)
	{
		Main->InteractLootBox = this;
		//Main->MainController->ShowInventory();

		//LootBoxOpen.Broadcast();
		if (Main->MainController)
		{
			UE_LOG(LogTemp, Warning, TEXT("LootBox::show Loot Box"));
			
			Main->MainController->ShowLootBox();
		}
	}
}

void ALootBox::CloseBox(AActor* Actor)
{
	//Call from MainController HideInventory();

	AMainCharacter* TMain = Cast<AMainCharacter>(Actor);
	if (Main == TMain)
	{
		Main->InteractLootBox = nullptr;
		if (Main->MainController)
		{
			UE_LOG(LogTemp, Warning, TEXT("LootBox::hide Loot Box"));
			Main->MainController->HideLootBox();
		}
	}
}