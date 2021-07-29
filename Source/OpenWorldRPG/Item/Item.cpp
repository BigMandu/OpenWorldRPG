// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "DrawDebugHelpers.h"

AItem::AItem()
{
	ItemState = EItemState::EIS_Spawn;

	
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	
}

void AItem::Pickup(class AActor* Actor)
{
	//UE_LOG(LogTemp, Warning, TEXT("AItem::Pickup"));
	AMainCharacter* Main = Cast<AMainCharacter>(Actor);
	if (Main)
	{
		//UE_LOG(LogTemp, Warning, TEXT("AItem::Add To Inventory"));
		
		if (Main->Inventory->AddItem(this))//OwningInventory세팅 및 Tarray에 넣어줌.
		{
			SetItemState(EItemState::EIS_Pickup);

			Mesh->SetSimulatePhysics(false);
			Mesh->SetEnableGravity(false);

			Mesh->SetHiddenInGame(true);	//Mesh->bHiddenInGame = true;
			Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
		}
	}
	
}

void AItem::Drop()
{
	if (OwningInventory)
	{
		if (OwningInventory->RemoveItem(this))
		{
			SetItemState(EItemState::EIS_Drop);
			FVector OwnerLocation = OwningInventory->GetOwner()->GetActorLocation();
			FVector OwnerForwardLo = OwningInventory->GetOwner()->GetActorForwardVector();
			FVector DropLocation = OwnerLocation + OwnerForwardLo * 120.f;

			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			Mesh->SetSimulatePhysics(true);
			Mesh->SetEnableGravity(true);
			Mesh->SetHiddenInGame(false);
			
			
			/* Item을 Drop할때 캐릭터 앞으로 버리게 하는데, 힘을 랜덤으로 줘서 살짝씩 다르게 한다.*/
			//SetActorLocation(DropLocation);
			Mesh->SetWorldLocation(DropLocation);
			float Rand = FMath::RandRange(1.f, 100000.f);
			Mesh->AddForce(FVector(OwnerForwardLo * Rand));

			/* debug */
			DrawDebugSphere(GetWorld(), DropLocation, 10.f, 8, FColor::Green, false, 2.f, (uint8)nullptr, 2.f);
			
			

			UE_LOG(LogTemp, Warning, TEXT("AItem::Drop success"));
			return;
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("AItem::Drop fail"));
}

void AItem::Use(AActor* Actor)
{
	//UE_LOG(LogTemp, Warning, TEXT("AItem::Use"));
	AMainCharacter* Main = Cast<AMainCharacter>(Actor);
	if (Main)
	{
		UE_LOG(LogTemp, Warning, TEXT("AItem::Use"));
	}
	
	

}
