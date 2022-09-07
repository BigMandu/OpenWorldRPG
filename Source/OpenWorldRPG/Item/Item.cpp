// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/BaseCharacter.h"
#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"

#include "OpenWorldRPG/NewInventory/Widget/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "DrawDebugHelpers.h"
#include "Equipment.h"


AItem::AItem()
{
	ItemState = EItemState::EIS_Spawn;

	StimuliComp = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("StimuliComp"));
	StimuliComp->bAutoRegister = true;
	
}

void AItem::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	StimuliComp->RegisterForSense(UAISense_Sight::StaticClass());
	StimuliComp->RegisterWithPerceptionSystem();
}

void AItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	/*MeshComponent->SetSimulatePhysics(true);
	MeshComponent->SetEnableGravity(true);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);

	MeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);

	MeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	MeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_EngineTraceChannel2, ECollisionResponse::ECR_Block);*/

	//Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);	
	//Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	//Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	//Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel2, ECollisionResponse::ECR_Overlap);
	if (ItemSetting.DataAsset)
	{
		//ItemSetting.DataAsset->ItemSize = FIntPoint(1, 1);
		//ItemSetting.DataAsset->bCanRotate = true;
	}
	
}

void AItem::BeginPlay()
{
	Super::BeginPlay();
	
	/*if (ItemObj == nullptr)
	{
		ItemObj = GetDefaultItemObj();
	}*/
}

UNewItemObject* AItem::GetDefaultItemObj()
{
	UNewItemObject* Obj = NewObject<UNewItemObject>(this, UNewItemObject::StaticClass(), FName(GetName()));
	if (Obj && ItemSetting.DataAsset)
	{
		//Obj->thumbnail = Thumbnail;
		Obj->item = this;
		Obj->bCanRotated = ItemSetting.DataAsset->bCanRotate;
		//Obj->itemsize = ItemSetting.DataAsset->ItemSize;
		Obj->itemName = ItemSetting.DataAsset->ItemName;
		Obj->itemDesc = ItemSetting.DataAsset->ItemDescription;

		Obj->icon = ItemSetting.DataAsset->Icon;
		Obj->iconRotated = ItemSetting.DataAsset->IconRotated;
		
		Obj->bCanEquip = ItemSetting.DataAsset->bCanEquip;
		Obj->EquipmentType = ItemSetting.DataAsset->EquipmentType;
		Obj->InteractType = ItemSetting.DataAsset->InteractType;
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
			//New Version. EnumRange를 이용해 Backpack, Vest순으로 넣을 수 있다면 넣는다.
			for (EEquipmentType EquipTp : TEnumRange<EEquipmentType>())
			{
				UNewItemObject* TempObj = nullptr;
				TempObj = BChar->Equipment->GetEquipStorage(EquipTp);
				if(TempObj == nullptr || TempObj->ItemInfo.DataAsset == nullptr || TempObj->ItemInfo.DataAsset->bHasStorage == false) continue;

				UItemStorageObject* ItemStorage = Cast<UItemStorageObject>(TempObj);
				if(ItemStorage == nullptr) continue;
				bFlag = BChar->BaseInventoryComp->TryAddItem(ItemStorage,ItemSetting);
				if (bFlag) break;
			}

			//OldVersion
			/*
			if(BChar->Equipment->bHasBackpack)
			{
				//AEquipment* Equipped = BChar->Equipment->GetEquippedWeaponSameType(EEquipmentType::EET_Backpack);
				//bFlag = AddAtEquip(Equipped);

				UNewItemObject* Obj = BChar->Equipment->GetEquipStorage(EEquipmentType::EET_Backpack);
				UItemStorageObject* ItemStorage = Cast<UItemStorageObject>(Obj);
				check(ItemStorage)

				bFlag = AddAtCharInv(ItemStorage);
			}
			//if have vest not backpack || fail add at backpack && have vest
			if(bFlag == false && BChar->Equipment->bHasVest)
			{
				UNewItemObject* Obj = BChar->Equipment->GetEquipStorage(EEquipmentType::EET_Vest);
				UItemStorageObject* ItemStorage = Cast<UItemStorageObject>(Obj);
				check(ItemStorage)

				bFlag = AddAtCharInv(ItemStorage);

				AEquipment Equipped = BChar->Equipment->GetEquippedWeaponSameType(EEquipmentType::EET_Vest);
				bFlag = AddAtEquip(Equipped);
			}
			*/

			//pocket, secure box
			if(bFlag == false)
			{
				//bFlag = AddAtCharInv(BChar->PocketInventoryComp);
				bFlag = AddAtCharInv(BChar, BChar->PocketStorage);
				if(bFlag == false)
				{
					bFlag = AddAtCharInv(BChar, BChar->SecureBoxStorage);
					if(bFlag == false)
					{
						//PickUP상태의 Item Add에 실패했다면 CustomInventoryLibrary의 BackToItem을 실행한다.
						if (ItemState == EItemState::EIS_Pickup)
						{
							ItemObj->bIsDestoryed = true;
							Destroy();
							
							UCustomInventoryLibrary::DirectInToInventory(ItemObj, BChar);
							
						}
						UE_LOG(LogTemp, Warning, TEXT("Item Add fail."));
					}
				}
			}

			if (bFlag)
			{
				SetItemState(EItemState::EIS_Pickup);
				//ItemObj->bIsDestoryed = true;
				Destroy();
			}

			
		}
		
		//old version
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

bool AItem::AddAtEquip(AEquipment* Equipped)
{
	if (Equipped)
	{
		//if (Equipped->EquipInventoryComp->TryAddItem(ItemObj))
		{
			SetItemState(EItemState::EIS_Pickup);
			//ItemObj->bIsDestoryed = true;
			Destroy();
			return true;
		}
	}
	return false;
}

bool AItem::AddAtCharInv(ABaseCharacter* Character, UItemStorageObject* Storage)
{
	if (Storage)
	{
		if (Character->BaseInventoryComp->TryAddItem(Storage, ItemSetting))
		{
			return true;
		}

		/*if (InvComp->TryAddItem(ItemObj))
		{
			SetItemState(EItemState::EIS_Pickup);
			ItemObj->bIsDestoryed = true;
			Destroy();
			return true;
		}*/
	}
	return false;
}

void AItem::Drop()
{
	if (OwningInventory)
	{
		if (OwningInventory->RemoveItem(this->ItemObj))
		{
			/*Mesh->SetHiddenInGame(false);
			Mesh->SetSimulatePhysics(true);
			Mesh->SetEnableGravity(true);
			Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);*/

			SetItemState(EItemState::EIS_Spawn);
			FVector OwnerLocation = OwningInventory->GetOwner()->GetActorLocation();
			FVector OwnerForwardLo = OwningInventory->GetOwner()->GetActorForwardVector();
			FVector DropLocation = OwnerLocation + OwnerForwardLo * 120.f;			
			
			
			/* Item을 Drop할때 캐릭터 앞으로 버리게 하는데, 힘을 랜덤으로 줘서 살짝씩 다르게 한다.*/
			//SetActorLocation(DropLocation);
			//Mesh->SetWorldLocation(DropLocation);
			
			float Rand = FMath::RandRange(1.f, 100000.f);
			//Mesh->AddForce(FVector(OwnerForwardLo * Rand));

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

//void AItem::SettingStorage()
//{
//	//EquipGridWidget이 없을때만 생성한다.
//	if (ItemInvComponent == nullptr)
//	{
//		if (MainCon == nullptr)
//		{
//			UE_LOG(LogTemp, Warning, TEXT("SettingStorage::MainCon is nullptr try Get Controller"));
//			if (OwningPlayer)
//			{
//				//MainCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
//				MainCon = Cast<AMainController>(OwningPlayer->GetController());
//			}
//		}
//
//		if (WEquipGridWidget && MainCon)
//		{
//			ItemInvComponent = CreateWidget<UNewInventoryGrid>(MainCon, WEquipGridWidget);
//			if (ItemInvComponent && ItemObj->GetItemInvComp() != nullptr)// && EquipInventoryComp)
//			{
//				UE_LOG(LogTemp, Warning, TEXT("SettingStorage::Try Initialize EquipGridWidget "));
//				//ItemObj->SetItemInvComp(EquipInventoryComp);
//				//EquipInventoryComp = ItemObj->GetItemInvComp(); //Item Swap시에 문제 발생 (ItemObj의 InvComp가 null만 가지고 있음)
//				ItemInvComponent->GridInitialize(ItemObj->GetItemInvComp(), ItemObj->GetItemInvComp()->TileSize);
//
//			}
//		}
//	}
//}
