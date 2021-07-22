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

		Main->Inventory->AddItem(this); //OwningInventory���� �� Tarray�� �־���.
		SetItemState(EItemState::EIS_Pickup);
	}
	
}

void AItem::Drop()
{
	if (OwningInventory)
	{
		OwningInventory->RemoveItem(this);
		SetItemState(EItemState::EIS_Spawn);

		Mesh->SetHiddenInGame(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void AItem::Use(class AMainChrarcter* Main)
{
	//UE_LOG(LogTemp, Warning, TEXT("AItem::Use"));
}
