// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "Item.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	Capacity = 20;
}


void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	//for (auto& item : DefaultItems) //�����Ҷ� Default Item�� �־��ش�.
	//{
	//	AItem* RefItem = Cast<AItem>(item);
	//	if (RefItem)
	//	{
	//		AddItem(RefItem);
	//	}
	//	
	//}
	
}

bool UInventoryComponent::AddItem(class AItem* Item)
{
	if (InventoryItems.Num() >= Capacity || Item == nullptr) //��밳���� �Ѱų�, item�� ������ 
	{
		return false;
	}

	Item->OwningInventory = this;
	InventoryItems.Add(Item);

	OnInventoryUpdated.Broadcast();

	return true;
}

bool UInventoryComponent::RemoveItem(class AItem* Item)
{
	if (Item)
	{
		Item->OwningInventory = nullptr;
		InventoryItems.RemoveSingle(Item);

		OnInventoryUpdated.Broadcast();
		return true;
	}
	return false;
}