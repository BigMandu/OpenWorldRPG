// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "Materials/MaterialInterface.h"
#include "DrawDebugHelpers.h"

AItem::AItem()
{
	ItemState = EItemState::EIS_Spawn;

	
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	ItemSize = FIntPoint(1, 1);
	bCanRotate = true;
	
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	if (ItemObj == nullptr)
	{
		ItemObj = GetDefaultItemObj();
	}
}

UNewItemObject* AItem::GetDefaultItemObj()
{
	UNewItemObject* Obj = NewObject<UNewItemObject>(this, UNewItemObject::StaticClass(), FName(GetName()));
	if (Obj)
	{
		//Obj->thumbnail = Thumbnail;
		Obj->item = this;
		Obj->bCanRotated = bCanRotate;
		Obj->itemsize = ItemSize;
		Obj->itemName = ItemName;
		Obj->itemDesc = ItemDescription;

		Obj->icon = Icon;
		Obj->iconRotated = IconRotated;
		
		
		UE_LOG(LogTemp, Warning, TEXT("AItem::Create object"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AItem::fail to Create object"));
	}
	return Obj;
}

void AItem::Pickup(class AActor* Actor)
{
	//UE_LOG(LogTemp, Warning, TEXT("AItem::Pickup"));
	AMainCharacter* Main = Cast<AMainCharacter>(Actor);
	if (Main)
	{
		//UE_LOG(LogTemp, Warning, TEXT("AItem::Add To Inventory"));
	
		if (Main->InventoryComp)
		{
			if (Main->InventoryComp->TryAddItem(ItemObj))
			{
				UE_LOG(LogTemp, Warning, TEXT("success Add item"));
				Destroy();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("fail to Add item"));
			}
		}
		/*
		if (Main->Inventory->AddItem(this))//OwningInventory세팅 및 Tarray에 넣어줌.
		{
			SetItemState(EItemState::EIS_Pickup);

			Mesh->SetSimulatePhysics(false);
			Mesh->SetEnableGravity(false);

			Mesh->SetHiddenInGame(true);	//Mesh->bHiddenInGame = true;
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
		*/
	}
	
}

void AItem::Drop()
{
	if (OwningInventory)
	{
		if (OwningInventory->RemoveItem(this))
		{
			Mesh->SetHiddenInGame(false);
			Mesh->SetSimulatePhysics(true);
			Mesh->SetEnableGravity(true);
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

			SetItemState(EItemState::EIS_Drop);
			FVector OwnerLocation = OwningInventory->GetOwner()->GetActorLocation();
			FVector OwnerForwardLo = OwningInventory->GetOwner()->GetActorForwardVector();
			FVector DropLocation = OwnerLocation + OwnerForwardLo * 120.f;			
			
			
			/* Item을 Drop할때 캐릭터 앞으로 버리게 하는데, 힘을 랜덤으로 줘서 살짝씩 다르게 한다.*/
			//SetActorLocation(DropLocation);
			Mesh->SetWorldLocation(DropLocation);
			
			float Rand = FMath::RandRange(1.f, 100000.f);
			Mesh->AddForce(FVector(OwnerForwardLo * Rand));

			/* debug */
			//DrawDebugSphere(GetWorld(), DropLocation, 10.f, 8, FColor::Green, false, 2.f, (uint8)nullptr, 2.f);
			//UE_LOG(LogTemp, Warning, TEXT("AItem::Drop success"));

			return;
		}
	}
	/* debug */
	//UE_LOG(LogTemp, Warning, TEXT("AItem::Drop fail"));
}

void AItem::Use(AActor* Actor)
{
	//UE_LOG(LogTemp, Warning, TEXT("AItem::Use"));
	AMainCharacter* Main = Cast<AMainCharacter>(Actor);
	if (Main)
	{
		
		/* debug */
		//UE_LOG(LogTemp, Warning, TEXT("AItem::Use"));
	}
	
	

}
