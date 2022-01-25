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

	//for (auto& item : DefaultItems) //시작할때 Default Item을 넣어준다.
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
	if (InventoryItems.Num() >= Capacity || Item == nullptr) //허용개수를 넘거나, item이 없으면 
	{
		UE_LOG(LogTemp, Warning, TEXT(" Item Add Failed "));
		return false;
	}

	UE_LOG(LogTemp, Warning, TEXT(" Item Add Successfully "));
	Item->OwningInventory = this;
	InventoryItems.Add(Item);

	
	//OnInventoryUpdated.Broadcast();

	return true;
}

bool UInventoryComponent::RemoveItem(class AItem* Item)
{
	if (Item)
	{
		//Item->OwningInventory = nullptr;
		InventoryItems.RemoveSingle(Item);

		//OnInventoryUpdated.Broadcast();

		/*FTransform form;
		form.SetLocation(GetOwner()->GetActorLocation() + GetOwner()->GetActorForwardVector() * 100.f);
		AActor* rmActor = GetWorld()->SpawnActor<AActor>(Item);*/
		



		return true;
	}
	return false;

	/*
	* index로 찾는 방법도 있다.
	if (int ItemIndex = InventoryItems.Find(Item))
	{
		InventoryItems.RemoveAt(ItemIndex);
	}
	*/
}