// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "OpenWorldRPG/BaseCharacter.h"
#include "OpenWorldRPG/Item/EquipmentComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "DrawDebugHelpers.h"
#include "Equipment.h"


AItem::AItem()
{
	ItemState = EItemState::EIS_Spawn;

	
	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

	Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_EngineTraceChannel2, ECollisionResponse::ECR_Block);

	//Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);	
	//Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	
	//Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
	
	ItemSize = FIntPoint(1, 1);
	bCanRotate = true;

	StimuliComp = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliComp"));
	StimuliComp->bAutoRegister = true;
	
}

void AItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StimuliComp->RegisterForSense(UAISense_Sight::StaticClass());
	StimuliComp->RegisterWithPerceptionSystem();
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
		
		Obj->bCanEquip = bCanEquip;
		Obj->EquipmentType = EquipmentType;
		Obj->InteractType = InteractType;
		
		//UE_LOG(LogTemp, Warning, TEXT("AItem::Create object"));
	}
	else
	{
		//UE_LOG(LogTemp, Warning, TEXT("AItem::fail to Create object"));
	}
	return Obj;
}

/** Pickup도 손봐야함.
*  Actor의 InventoryComponent를 우선 봐야할게 아니라, 
* Actor의 Equipment에서 Storage를 갖고있는 Backpack이나 Vest를 우선 탐색해서
* 있다면 그곳에 넣어야됨.
*/
bool AItem::Pickup(class AActor* Actor)
{
	bool bReturn = false;
	//UE_LOG(LogTemp, Warning, TEXT("AItem::Pickup"));
	bool bFlag = false;

	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	if (BChar)
	{
		//UE_LOG(LogTemp, Warning, TEXT("AItem::Add To Inventory"));
		
		if (BChar->Equipment)
		{
			//백팩이 있으면 추가
			if(BChar->Equipment->bHasBackpack)
			{
				bFlag = AddAtBackPack(BChar);
			}
			//조끼가 있거나 , 백팩 추가 실패 && 조끼 있으면  추가
			else if(BChar->Equipment->bHasVest || (bFlag == false && BChar->Equipment->bHasVest))
			{
				bFlag = AddAtVest(BChar);
			}
			//위 전부 실패시 포켓, 시큐어 박스에 추가
			else if(bFlag == false)
			{
				bFlag = AddAtPocket(BChar);
				if(bFlag == false)
				{
					bFlag = AddAtSecureBox(BChar);
					if(bFlag == false)
					{
						UE_LOG(LogTemp, Warning, TEXT("Item Add fail."));
					}
				}
			}

			
		}
		
		/*if (BChar->InventoryComp)
		{
			if (BChar->InventoryComp->TryAddItem(ItemObj))
			{
				SetItemState(EItemState::EIS_Pickup);
				Destroy();
				UE_LOG(LogTemp, Warning, TEXT("success Add item"));
				
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("fail to Add item"));
			}
		}*/
	}
	bReturn = bFlag;
	return bReturn;
}

bool AItem::AddAtBackPack(ABaseCharacter* BChar)
{
	AEquipment* Equipped = BChar->Equipment->GetEquippedWeaponSameType(EEquipmentType::EET_Backpack);
	if (Equipped->EquipInventoryComp->TryAddItem(ItemObj))
	{
		SetItemState(EItemState::EIS_Pickup);
		Destroy();
		return true;
	}
	return false;
}
bool AItem::AddAtVest(ABaseCharacter* BChar)
{
	AEquipment* Equipped = BChar->Equipment->GetEquippedWeaponSameType(EEquipmentType::EET_Vest);
	if (Equipped->EquipInventoryComp->TryAddItem(ItemObj))
	{
		SetItemState(EItemState::EIS_Pickup);
		Destroy();
		return true;
	}
	return false;
}
bool AItem::AddAtPocket(ABaseCharacter* BChar)
{
	if(BChar->PocketInventoryComp->TryAddItem(ItemObj))
	{
		SetItemState(EItemState::EIS_Pickup);
		Destroy();
		return true;
	}
	return false;
}
bool AItem::AddAtSecureBox(ABaseCharacter* BChar)
{
	if (BChar->SecureBoxInventoryComp->TryAddItem(ItemObj))
	{
		SetItemState(EItemState::EIS_Pickup);
		Destroy();
		return true;
	}
	return false;
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

			SetItemState(EItemState::EIS_Spawn);
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
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	if (BChar)
	{
		
		/* debug */
		//UE_LOG(LogTemp, Warning, TEXT("AItem::Use"));
	}
	
	

}
