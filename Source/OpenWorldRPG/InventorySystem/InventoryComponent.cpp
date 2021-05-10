// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Item.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	Capacity = 20;
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	for (auto& item : DefaultItems) //�����Ҷ� Default Item�� �־��ش�.
	{
		AddItem(item);
	}
	
}

bool UInventoryComponent::AddItem(class UItem* Item)
{
	if (InventoryItems.Num() >= Capacity || Item == nullptr) //��밳���� �Ѱų�, item�� ������ 
	{
		return false;
	}

	Item->OwningInventory = this;	
	Item->World = Item->GetWorld();
	InventoryItems.Add(Item);

	OnInventoryUpdated.Broadcast();

	return true;
}

bool UInventoryComponent::RemoveItem(class UItem* Item)
{
	if (Item)
	{
		Item->OwningInventory = nullptr;
		Item->World = nullptr;
		InventoryItems.RemoveSingle(Item);

		OnInventoryUpdated.Broadcast();
		return true;
	}
	return false;
}