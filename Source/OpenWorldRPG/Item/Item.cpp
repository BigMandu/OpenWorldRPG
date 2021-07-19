// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
//#include "InventoryComponent.h"
#include "OpenWorldRPG/MainCharacter.h"


void AItem::Pickup(class AActor* Actor)
{
	//UE_LOG(LogTemp, Warning, TEXT("AItem::Pickup"));
	AMainCharacter* Main = Cast<AMainCharacter>(Actor);
	if (Main)
	{
		//UE_LOG(LogTemp, Warning, TEXT("AItem::Add To Inventory"));

		Main->Inventory->AddItem(this); //OwningInventory세팅 및 Tarray에 넣어줌.
		
		Mesh->SetHiddenInGame(true);
		//Mesh->bHiddenInGame = true;
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
}

void AItem::Drop()
{
	if (OwningInventory)
	{
		OwningInventory->RemoveItem(this);
		Mesh->SetHiddenInGame(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void AItem::Use(class AMainChrarcter* Main)
{
	//UE_LOG(LogTemp, Warning, TEXT("AItem::Use"));
}
