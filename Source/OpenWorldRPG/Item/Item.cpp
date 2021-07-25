// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
//#include "InventoryComponent.h"
#include "OpenWorldRPG/MainCharacter.h"

AItem::AItem()
{
	ItemState = EItemState::EIS_Spawn;
}

void AItem::Pickup(class AActor* Actor)
{
	//UE_LOG(LogTemp, Warning, TEXT("AItem::Pickup"));
	AMainCharacter* Main = Cast<AMainCharacter>(Actor);
	if (Main)
	{
		//UE_LOG(LogTemp, Warning, TEXT("AItem::Add To Inventory"));
		
		Mesh->SetHiddenInGame(true);	//Mesh->bHiddenInGame = true;
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		Main->Inventory->AddItem(this); //OwningInventory세팅 및 Tarray에 넣어줌.
		SetItemState(EItemState::EIS_Pickup);
	}
	
}

void AItem::Drop()
{
	if (OwningInventory)
	{
		SetItemState(EItemState::EIS_Drop);

		if (OwningInventory->RemoveItem(this))
		{

			Mesh->SetHiddenInGame(false);
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			UE_LOG(LogTemp, Warning, TEXT("AItem::Drop success"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("AItem::Drop fail"));
		}
	}
}

void AItem::Use(class AMainChrarcter* Main)
{
	//UE_LOG(LogTemp, Warning, TEXT("AItem::Use"));
}
