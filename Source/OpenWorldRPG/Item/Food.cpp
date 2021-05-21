// Fill out your copyright notice in the Description page of Project Settings.


#include "Food.h"

AFood::AFood()
{
	UseActionText = FText::FromString("eat");
	ItemName = FText::FromString("bread");

	Heal = 10.f;
}

void AFood::Interact()//_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("Food:: Interact"));

	Pickup();
}

void AFood::Pickup()
{
	ItemSMesh->SetVisibility(false);
	ItemSMesh->SetSimulatePhysics(false);
	ItemSMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AFood::Use(class ACharacter* Character)
{
	if (Character)
	{
		UE_LOG(LogTemp, Warning, TEXT("Food::Use, Heal is : %f"), Heal);

		if (OwningInventory)
		{
			OwningInventory->RemoveItem(this);
		}

	}
}