// Fill out your copyright notice in the Description page of Project Settings.



#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/Item/Weapon.h"

#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"

#include "OpenWorldRPG/NewInventory/Widget/EquipmentSlot.h"
#include "OpenWorldRPG/NewInventory/Widget/NewInventoryGrid.h"

#include "OpenWorldRPG/NewInventory/Library/InventoryStruct.h"
#include "OpenWorldRPG/NewInventory/Library/CustomInventoryLibrary.h"

#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"
#include "OpenWorldRPG/AI/EnemyAIController.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"

#include "Sound/SoundCue.h"


AEquipment::AEquipment() : Super()
{
	//SKMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	//EquipInventoryComp = CreateDefaultSubobject<UNewInventoryComponent>(TEXT("EquipInventoryComp"));

	//RootComponent = SKMesh;
	//Mesh->SetupAttachment(GetRootComponent());

	

}

void AEquipment::BeginPlay()
{
	Super::BeginPlay();

	/*if (bHasStorage)
	{
		SettingStorage();
	}*/
	

	if (ItemSetting.DataAsset)
	{
		CPDA = Cast<UCustomPDA>(ItemSetting.DataAsset);
		if(CPDA && CPDA->bHasStorage)
		{
			SettingStorage();
			if (bHasSpawnItem)
			{
				SpawnItem();
			}
		}
	}
}
void AEquipment::SettingStorage()
{
	StorageObj = NewObject<UItemStorageObject>();
	StorageObj->InitializeStorage(CPDA->StorageX, CPDA->StorageY, CPDA->TileSize);

	//OldVersion
	/*
	//EquipGridWidget�� �������� �����Ѵ�.
	//if (EquipGridWidget == nullptr)
	{
		if (MainCon == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("SettingStorage::MainCon is nullptr try Get Controller"));
			if (OwningPlayer)
			{
				//MainCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
				MainCon = Cast<AMainController>(OwningPlayer->GetController());
			}
		}

		if (WEquipGridWidget && MainCon)
		{
			EquipGridWidget = CreateWidget<UNewInventoryGrid>(MainCon, WEquipGridWidget);
			if (EquipGridWidget && EquipInventoryComp)//ItemObj->GetItemInvComp() != nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("SettingStorage::Try Initialize EquipGridWidget "));

				//EquipInventoryComp = ItemObj->GetItemInvComp(); //Item Swap�ÿ� ���� �߻� (ItemObj�� InvComp�� null�� ������ ����)
				//EquipGridWidget->GridInitialize(ItemObj->GetItemInvComp(), ItemObj->GetItemInvComp()->TileSize);
				EquipGridWidget->GridInitialize(EquipInventoryComp, EquipInventoryComp->TileSize);

			}
		}
	}
	*/
}

void AEquipment::SpawnItem()
{
	for (auto AddItem : SpawnItemList)
	{
		AItem* Item = GetWorld()->SpawnActor<AItem>(AddItem);
		if (Item)
		{

			if (EquipBaseComponent->TryAddItem(StorageObj, Item->ItemSetting))
			{
				Item->SetItemState(EItemState::EIS_Pickup);
				//Item->ItemObj->bIsDestoryed = true;
				Item->Destroy();
			}
		}
	}
}

void AEquipment::ReInitialize(UNewItemObject* Obj)
{
	if(ItemObj)// && EquipGridWidget == nullptr)
	{
		ItemObj = Obj;
		//if(bHasStorage) ����� �׳� �ٷ� SettingStorage�� ��������.
		{
			/*if (MainCon == nullptr) 
			{
				MainCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
			}*/
			//AMainController* MainCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
			//if (WEquipGridWidget)
			//{
			//	if(MainCon)
			//	{
			//		EquipGridWidget = CreateWidget<UNewInventoryGrid>(MainCon, WEquipGridWidget);
			//		EquipGridWidget->GridInitialize(EquipInventoryComp, EquipInventoryComp->TileSize);


			//	}
			//	//else if(AICon)
			//	//{
			//		EquipGridWidget = CreateWidget<UNewInventoryGrid>(AICon, WEquipGridWidget);
			//	//}
			//}
		}
	}
}

//�θ� Ŭ�������� Beginplay�� �ѹ��� ����.
UNewItemObject* AEquipment::GetDefaultItemObj()
{
	UNewItemObject* Obj = Super::GetDefaultItemObj();

	/*if(Obj && bHasStorage && EquipInventoryComp)
	{
		Obj->SetItemInvComp(EquipInventoryComp);
	}*/
	if (CPDA && CPDA->bHasStorage)
	{
		//Obj->bHasStorage = ItemSetting.DataAsset->bHasStorage;
		//ItemSetting.DataAsset->bHasStorage;
		/*Obj->GridTileSize = GridTileSize;
		Obj->WEquipGridWidget = WEquipGridWidget;
		Obj->ObjInvComp->Rows = Rows;
		Obj->ObjInvComp->Columns = Columns;*/
	}
	return Obj;
}

void AEquipment::SetOwningPlayer(AActor* Actor)
{
	if (OwningPlayer != Actor)
	{
		ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
		if (BChar)
		{
			AMainController* TMainCon = Cast<AMainController>(BChar->GetController());
			AEnemyAIController* TAICon = Cast<AEnemyAIController>(BChar->GetController());
			if(TMainCon)
			{
				MainCon = TMainCon;
			}
			else if(TAICon)
			{
				AICon = TAICon;
			}

			OwningPlayer = BChar;
			SetInstigator(BChar); //Instigator ����.
		}
	}
}

AActor* AEquipment::GetOwningPlayer()
{
	if (OwningPlayer)
	{
		return OwningPlayer;
	}
	return nullptr;
}

////Inventory�� Equipped���� ��� Swap
//void AEquipment::SwapBetweenInvAndEquipped(ABaseCharacter* BChar, UNewItemObject* ToInventory)
//{
//	if (ToInventory && ToInventory->Equipment)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Equipment::SwapEquip / need to same below obj"));
//		UE_LOG(LogTemp,Warning, TEXT("Equipment::SwapEquip / call RemoveEquip func. Remove Equip : %s"), *ToInventory->GetFName().ToString());
//		BChar->Equipment->RemoveEquipment(ToInventory);
//		//Obj�� �Ѱܹ�����. 
//		UE_LOG(LogTemp, Warning, TEXT("Equipment::SwapEquip / call SendToInv func. Send Equip : %s"),*ToInventory->GetFName().ToString());
//		ToInventory->Equipment->SendToInventory(BChar, ToInventory);
//	}
//}
//
////Equipped Weapon���� Slot swap (Rifle type�� �ش�)
//void AEquipment::SwapBetweenEquipped(ABaseCharacter* BChar, UNewItemObject* BeforeEquipped)
//{
//	if (ItemObj && BeforeEquipped)
//	{
//		ERifleSlot TempRifleSlot = BeforeEquipped->RifleAssign;
//
//		BeforeEquipped->SetLinkSlot(ItemObj->SettedSlot);
//		BeforeEquipped->SettedSlot->SettedObj = ItemObj;
//		BeforeEquipped->RifleAssign = ItemObj->RifleAssign;
//
//		AWeapon* Weapon = nullptr;
//		Weapon = Cast<AWeapon>(ItemObj->Equipment);
//		Weapon->SettingRifleAssign(BChar, TempRifleSlot);
//		Weapon->GunAttachToMesh(BChar);
//
//		Weapon = Cast<AWeapon>(BeforeEquipped->Equipment);		
//		Weapon->SettingRifleAssign(BChar, ItemObj->SettedSlot->RifleSlotType);
//		Weapon->GunAttachToMesh(BChar);
//		
//	}
//}

bool AEquipment::Equip(AActor* Actor, ERifleSlot RifleSlot)
{
	
	UE_LOG(LogTemp, Warning, TEXT("AEquipment::Equip func called"));
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	bool bReturn = false;
	UNewItemObject* BeforeEquippedObj = nullptr;

	if (BChar)
	{
		bool bIsInvEquipSwapState = false;
		/*�� ������ Ÿ�԰� ��ġ�ϴ� ���Ⱑ �̹� �ִٸ� �Ʒ� 2���� ���ǿ� ���� �б��Ѵ�.
		 * 1. �� ���Ⱑ ���忡 ������ ���¶��, Inventory�� �߰��� �õ��Ѵ�.
		 * 2. �� ���Ⱑ Inventory�� �ִ� ���¶�� ���� ������ ���� �ٲ� ���� �Ѵ�.
		 * ��ġ�ϴ� ���Ⱑ ���ٸ� Equip�Լ��� ȣ���Ѵ�.
		 */
		if (BChar->Equipment->IsSameTypeExist(this,RifleSlot))
		{
			if (GetItemState() == EItemState::EIS_Spawn)
			{
				////�̹� �������� Weapon�� ������ ���Կ� Equip�� �õ��Ѵٸ� slot�� ���� swap�Ѵ�.
				//AWeapon* Weapon = Cast<AWeapon>(this);
				//if (Weapon)
				//{
				//	BeforeEquippedObj = BChar->Equipment->GetEquippedWeaponSameType(EEquipmentType::EET_Rifle, nullptr, RifleSlot);
				//	SwapBetweenEquipped(BChar, BeforeEquippedObj);
				//}
				//else
				{
					//�Ϲ� ���� �κ��丮�� �� item�� ������ �Լ��� �����Ѵ�.
					SendToInventory(BChar,nullptr);
					return true;
				}
			}

			//�Ʒ� �ڵ���� Swap��� ���� �ڵ���̴�. Swap����� ���ֱ�� �ߴ�.

			//else if (GetItemState() == EItemState::EIS_Pickup)
			//{
			////��� Swap ����� ����.

			//	/* ���� �Ϸ��� ��� Pickup����(Inventory�� �ִ� ����)��
			//	 * ��� Swap�� �����Ѵ�.
			//	 */
			//	BeforeEquippedObj = BChar->Equipment->GetEquippedWeaponSameType(EEquipmentType::EET_MAX, ItemObj, RifleSlot);
			//	if (BeforeEquippedObj != nullptr)
			//	{
			//		bIsInvEquipSwapState = true;
			//	}
			//}
		}

		/*if (bIsInvEquipSwapState)
		{
			//Equip�� �ϰ��� ������ �����ߴ��� �ű��.. ���� ������ �� weapon�� attach socket�� �����Ǵ� ������ ������
			//Inventory ������ �� ����ϱ� ������ �̷��� �ߴ�.
			SwapBetweenInvAndEquipped(BChar, BeforeEquippedObj);
		}*/

		bReturn = StepEquip(Actor, RifleSlot);
		
		

	}

	return bReturn;
}

bool AEquipment::StepEquip(AActor* Actor, ERifleSlot RifleSlot)
{
	bool bReturn = false;
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	const USkeletalMeshSocket* Socket = nullptr;
	CPDA = Cast<UCustomPDA>(ItemSetting.DataAsset);
	switch (CPDA->EquipmentType)
	{
	case EEquipmentType::EET_Helmet:
		Socket = BChar->GetMesh()->GetSocketByName("headsocket");
		break;
	case EEquipmentType::EET_Vest:
		Socket = BChar->GetMesh()->GetSocketByName("VestSocket");
		break;
	case EEquipmentType::EET_Backpack:
		Socket = BChar->GetMesh()->GetSocketByName("BackpackSocket");
		break;
	}

	//Socket�� �ְų�, WeaponType�̸� �ش� (Weapon�� WeaponClass���� �����ϱ� ������ Socket�� ����)
	//if (Socket != nullptr || (ItemSetting.DataAsset->EquipmentType == EEquipmentType::EET_Rifle || ItemSetting.DataAsset->EquipmentType == EEquipmentType::EET_Pistol))
	{
		if (Socket != nullptr)
		{
			Socket->AttachActor(this, BChar->GetMesh());
			SetActorRelativeTransform(CPDA->MeshAttachTransform);
		}

		// Mesh Setting
		SKMesh->SetHiddenInGame(false);
		Mesh->SetHiddenInGame(true); //Static Mesh�� �Ⱥ��̰� �ϰ�, Collision�� ����.
		Mesh->SetSimulatePhysics(false);
		Mesh->SetEnableGravity(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SetOwningPlayer(BChar);
		
		//Main�� �ִ� Equipment�� Add���ش�.
		//BChar->Equipment->AddEquipment(this);
		BChar->Equipment->AddEquipment(ItemSetting, this);

		bReturn = true;
	}

	if (CPDA->EquippedSound)
	{
		UGameplayStatics::SpawnSoundAttached(CPDA->EquippedSound, OwningPlayer->GetRootComponent());
		//UGameplayStatics::PlaySoundAtLocation(GetWorld(), EquippedSound, OwningPlayer->GetActorLocation()
	}

	return bReturn;
}



void AEquipment::SendToInventory(AActor* Actor, UNewItemObject* obj)
{
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	check(BChar);
	
	OwningEquipment = nullptr;
	//ItemObj->bIsDestoryed = true;

	//���� �߰��� InventoryComponent�� ������.
	/*if (bHasStorage && EquipInventoryComp)
	{
		ItemObj->SetItemInvComp(EquipInventoryComp);
	}*/

	UE_LOG(LogTemp,Warning,TEXT("Equipment::SendToInv / call Pickup func"));
	//�θ� class�� �ִ� Pickup�Լ� ȣ���ؼ� item�� ��´�.
	Pickup(BChar,obj);
}

//�� �Լ��� ���߿� CustomInventoryLibrary�� �����ҵ�.
//ItemObj�� ����ؾߵ�.


void AEquipment::Drop()
{
	Super::Drop();

	UE_LOG(LogTemp, Warning, TEXT("AEquipment::Drop"));
	SKMesh->SetHiddenInGame(true);
	OwningEquipment = false;


}

void AEquipment::Remove()
{
	/* �ƹ��͵� ����. .. ��..*/
	//Weapon�� Remove�� ����. �� ȣ���ؾߵ�.

	//if (ItemSetting.DataAsset->bHasStorage)// && EquipInventoryComp)
	{
		//ItemObj->SetItemInvComp(EquipInventoryComp);
	}

}