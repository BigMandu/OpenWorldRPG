// Fill out your copyright notice in the Description page of Project Settings.


#include "Item.h"
#include "OpenWorldRPG/Item/BasePDA.h"
#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/Item/ConsumePDA.h"
#include "OpenWorldRPG/Item/Weapon.h"

#include "OpenWorldRPG/BaseCharacter.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/ItemStorageObject.h"

#include "OpenWorldRPG/NewInventory/Widget/NewInventoryGrid.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"

#include "OpenWorldRPG/MainAnimInstance.h"


#include "Engine/SkeletalMeshSocket.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "DrawDebugHelpers.h"
#include "Equipment.h"


AItem::AItem() : Super()
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

	if (Mesh)
	{
		Mesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
		Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Block);

		
		//Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	}
	


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
	
	Cast<UMeshComponent>(GetRootComponent())->SetSimulatePhysics(true);
	if (Mesh)
	{
		Mesh->SetSimulatePhysics(true);
		//SetRootComponent(Mesh);
	}
	if (SKMesh)
	{
		SKMesh->SetSimulatePhysics(false);
		
	}
}

//UNewItemObject* AItem::GetDefaultItemObj()
//{
//	UNewItemObject* Obj = NewObject<UNewItemObject>(this, UNewItemObject::StaticClass(), FName(GetName()));
//	if (Obj && ItemSetting.DataAsset)
//	{
//		//Obj->thumbnail = Thumbnail;
//		Obj->item = this;
//		Obj->bCanRotated = ItemSetting.DataAsset->bCanRotate;
//		//Obj->itemsize = ItemSetting.DataAsset->ItemSize;
//		Obj->itemName = ItemSetting.DataAsset->ItemName;
//		Obj->itemDesc = ItemSetting.DataAsset->ItemDescription;
//
//		Obj->icon = ItemSetting.DataAsset->Icon;
//		Obj->iconRotated = ItemSetting.DataAsset->IconRotated;
//		Obj->InteractType = ItemSetting.DataAsset->InteractType;
//
//		UCustomPDA* CPDA = Cast<UCustomPDA>(ItemSetting.DataAsset);
//		if(CPDA)
//		{
//			Obj->bCanEquip = CPDA->bCanEquip;
//			Obj->EquipmentType = CPDA->EquipmentType;
//			
//		}
//		//UE_LOG(LogTemp, Warning, TEXT("AItem::Create object"));
//	}
//	else
//	{
//		//UE_LOG(LogTemp, Warning, TEXT("AItem::fail to Create object"));
//	}
//	return Obj;
//}


bool AItem::Pickup(class AActor* Actor, UNewItemObject* obj)
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
			//New Version. EnumRange�� �̿��� Backpack, Vest������ ���� �� �ִٸ� �ִ´�.
			
			for (EEquipmentType EquipTp : TEnumRange<EEquipmentType>())
			{
				UNewItemObject* TempObj = nullptr;
				UCustomPDA* Var_CPDA = nullptr;

				TempObj = BChar->Equipment->GetEquipStorage(EquipTp);
				if(TempObj)
				{
					Var_CPDA = Cast<UCustomPDA>(TempObj->ItemInfo.DataAsset);
					if (Var_CPDA == nullptr || (Var_CPDA && Var_CPDA->bHasStorage == false))
					{
						continue;
					}
				}
				else continue;

				UItemStorageObject* ItemStorage = Cast<UItemStorageObject>(TempObj);
				if(ItemStorage == nullptr) continue;

				bFlag = BChar->BaseInventoryComp->TryAddItem(ItemStorage,ItemSetting, obj);
				if (bFlag) break;
			}

			//�������� ��� AddItem�� �����ߴٸ� Pocket, Securebox������ AddItem�� �õ��Ѵ�.
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
						//DDOPer�� defaultVisual�� �����ϸ鼭 �ʿ� ������.
						//PickUP������ Item Add�� �����ߴٸ� CustomInventoryLibrary�� BackToItem�� �����Ѵ�.
						/*if (ItemState == EItemState::EIS_Pickup)
						{
							ItemObj->bIsDestoryed = true;
							Destroy();
							
							UCustomInventoryLibrary::DirectInToInventory(ItemObj, BChar);							
						}*/
						UE_LOG(LogTemp, Warning, TEXT("Item Add fail. No Space"));
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
			
			
			/* Item�� Drop�Ҷ� ĳ���� ������ ������ �ϴµ�, ���� �������� �༭ ��¦�� �ٸ��� �Ѵ�.*/
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


//Remove Weapon Or Item before Attached, And Change Holding Item.
//Attach�ϱ� �� ����
void AItem::AttachToHand(ABaseCharacter* Actor, UNewItemObject* Obj)
{
	if (Actor->EquippedWeapon)
	{
		Actor->EquippedWeapon->GunAttachToSubSocket(Actor);

		//�������� ���� �ӽ������Ѵ�.
		BeforeEquipppedWeapon = Actor->EquippedWeapon;
		Actor->EquippedWeapon = nullptr;
	}
	//�̹� �ٸ� HoldingItem�� �ִٸ� ���Ѵ�.
	else if (Actor->HoldingItem.IsValid())
	{
		//���� �̰Ŷ� �����ѰŸ� ���� ������ �� item�� destory�Ѵ�.
		if (Actor->HoldingItem.Get() == this)
		{
			this->Destroy();
			return;
		}
		//���� �ٸ� item�� ��� �־��ٸ�, �� ��ϵ� holding actor�� �����Ѵ�.
		else if (Actor->HoldingItem.Get() != this)
		{
			Actor->HoldingItem.Get()->Destroy();
			Actor->SetHoldingItem(nullptr);
		}
	}

	if (Obj)
	{
		ItemObj = Obj;
	}


	Mesh->SetSimulatePhysics(false);

	//�տ� �� �������� Interaction�� ���� �ʵ��� �Ѵ�.
	bCanNotInteractable = true;

	//MainChar�� LMBDown, Up�� ���� EquippedGrenade������ ����, �����Ѵ�.
	Actor->SetHoldingItem(this);//HoldingItem = this;
	
	AttachToHand_Step(Actor);
}

void AItem::AttachToHand_Step(ABaseCharacter* Actor)
{
	AMainCharacter* Player = Cast<AMainCharacter>(Actor);
	if (Player)
	{
		Player->ReAttachHoldingItem();
	}
	else
	{
		const USkeletalMeshSocket* AttachSocket = nullptr;
		FTransform AttachTransform;

		if (Actor)
		{
			const USkeletalMeshSocket* LeftHandSocket = Actor->GetMesh()->GetSocketByName(Actor->LeftHandGripSocketName);
			if (LeftHandSocket)
			{
				if (LeftHandSocket->AttachActor(this,Actor->GetMesh()))
				{
					if (ItemSetting.DataAsset)
					{
						SetActorRelativeTransform(ItemSetting.DataAsset->TPS_HandAttachTransform);
					}

					//SKMesh->SetHiddenInGame(false);
					//BChar->SetEquippedWeapon(this);
				}
			}

		}
	}
}

void AItem::DetachFromHand(ABaseCharacter* Actor, bool bIsNeedToEquipBeforeWeapon)
{
	//Detach�� �Ѵ�.
	FDetachmentTransformRules Rules = FDetachmentTransformRules(EDetachmentRule::KeepWorld, false);
	this->DetachFromActor(Rules);

	//������ PrimaryWeapon�� ������Ų��.
	Actor->SetHoldingItem(nullptr);// = nullptr;
	if(bIsNeedToEquipBeforeWeapon)
	{
		EquipBeforeWeapon(Actor);
	}

}

void AItem::Use(ABaseCharacter* Char, UNewItemObject* Obj)
{
	check(Char)
	UE_LOG(LogTemp, Warning, TEXT("AItem::Use"));
	
	
	Char->PlayUseItemAnim(this);

	if (ItemSetting.DataAsset->ItemType == EItemType::EIT_Food || ItemSetting.DataAsset->ItemType == EItemType::EIT_Medical)
	{			
		UConsumePDA* ConsumablePDA = Cast<UConsumePDA>(ItemSetting.DataAsset);		
		if (ConsumablePDA->bIsHPRecovery)
		{			
			Char->RecoveryHealthDelegate(ConsumablePDA->RecoveryTime, ConsumablePDA->RecoveryAmount,this);
			UE_LOG(LogTemp,Warning, TEXT("AItem::Use // HP Recovery "));
		}
		if (ConsumablePDA->bIsStaminaRecovery)
		{
			Char->RecoveryStaminaDelegate(ConsumablePDA->RecoveryTime, ConsumablePDA->RecoveryAmount, this);
			UE_LOG(LogTemp, Warning, TEXT("AItem::Use // Stamina Recovery "));
		}
	}
	
	
	RemoveCountAtIventory(Char,1);
	Destroy();
	
	
}


void AItem::RemoveCountAtIventory(ABaseCharacter* Char, int32 RemoveCount)
{
	if (ItemObj)// && ItemSetting.DataAsset->bCanStack)
	{
		Char->BaseInventoryComp->RemoveItemCount(ItemObj, RemoveCount);
	}
}

void AItem::EquipBeforeWeapon(ABaseCharacter* Actor)
{
	if (BeforeEquipppedWeapon.IsValid())
	{
		if (Actor->TPAnimInstance)
		{
			Actor->TPAnimInstance->WeaponTypeNumber = 0;
		}

		switch (BeforeEquipppedWeapon->RifleAssign)
		{
			case ERifleSlot::ERS_Primary:
				Actor->ChangePrimaryWeapon();
			break;
			case ERifleSlot::ERS_Sub:
				Actor->ChangeSubWeapon();
			break;
			case ERifleSlot::ERS_Pistol:
				Actor->ChangePistolWeapon();
			break;
		}
	}
}



//void AItem::SettingStorage()
//{
//	//EquipGridWidget�� �������� �����Ѵ�.
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
//				//EquipInventoryComp = ItemObj->GetItemInvComp(); //Item Swap�ÿ� ���� �߻� (ItemObj�� InvComp�� null�� ������ ����)
//				ItemInvComponent->GridInitialize(ItemObj->GetItemInvComp(), ItemObj->GetItemInvComp()->TileSize);
//
//			}
//		}
//	}
//}
