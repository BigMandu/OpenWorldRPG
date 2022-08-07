// Fill out your copyright notice in the Description page of Project Settings.



#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/CustomPDA.h"
#include "OpenWorldRPG/Item/Weapon.h"

#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/EquipmentComponent.h"

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
	if (ItemSetting.DataAsset && ItemSetting.DataAsset->bHasStorage)
	{
		SettingStorage();
		if (bHasSpawnItem)
		{
			SpawnItem();
		}
	}
}
void AEquipment::SettingStorage()
{
	StorageObj = NewObject<UItemStorageObject>();
	StorageObj->InitializeStorage(ItemSetting.DataAsset->StorageX, ItemSetting.DataAsset->StorageY, ItemSetting.DataAsset->TileSize);

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
	if (ItemSetting.DataAsset && ItemSetting.DataAsset->bHasStorage)
	{
		//Obj->bHasStorage = ItemSetting.DataAsset->bHasStorage;
		ItemSetting.DataAsset->bHasStorage;
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

bool AEquipment::Equip(AActor* Actor, ERifleSlot RifleSlot)
{
	bool bReturn = false;
	UE_LOG(LogTemp, Warning, TEXT("AEquipment::StepEquip func called"));
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	
	if (BChar)
	{
		/*�� ������ Ÿ�԰� ��ġ�ϴ� ���Ⱑ �̹� �ִٸ�
		 * 1. �� ���Ⱑ ���忡 ������ ���¶��, Inventory�� �߰��� �õ��Ѵ�.
		 * 2. �� ���Ⱑ Inventory�� �ִ� ���¶�� ���� ������ ���� �ٲ� ���� �Ѵ�.
		 * ��ġ�ϴ� ���Ⱑ ���ٸ� Equip�Լ��� ȣ���Ѵ�.
		 */
		if (BChar->Equipment->IsSameTypeExist(this,RifleSlot))
		{
			if (GetItemState() == EItemState::EIS_Spawn)
			{
				//�κ��丮�� �� item�� ������ �Լ��� �����Ѵ�.
				SendToInventory(BChar);
				return true;
			}
			else if (GetItemState() == EItemState::EIS_Pickup)
			{
				/* ���� �Ϸ��� ��� Pickup����(Inventory�� �ִ� ����)��
				 * ��� Swap�� �����Ѵ�.
				 */
				//Try To Swap Equipment
				UNewItemObject* EquippedObj = nullptr;
				//AEquipment* BeforeEquipped = BChar->Equipment->GetEquippedWeaponSameType(EEquipmentType::EET_MAX, ItemSetting, EquippedObj, RifleSlot);
				UNewItemObject* BeforeEquippedObj = BChar->Equipment->GetEquippedWeaponSameType(EEquipmentType::EET_MAX, ItemSetting, RifleSlot);
				if (BeforeEquippedObj)
				{
					BChar->Equipment->RemoveEquipment(EquippedObj);
					Cast<AEquipment>(BeforeEquippedObj->item)->SendToInventory(BChar);
				}

				//�ӽ÷� �ּ�ó����. GetEquippedWeaponSameType �Լ� ���� ������.
				//AEquipment* BeforeEquipment = BChar->Equipment->GetEquippedWeaponSameType(EEquipmentType::EET_MAX, this, RifleSlot);
				//if (BeforeEquipment)
				{
					//BChar->Equipment->RemoveEquipment(BeforeEquipment); //SwapEquipment(BeforeEquipment, this);
					//BeforeEquipment->SendToInventory(BChar);
					/*this�� SpawnActor�ؼ� Data�� �̰��ϴ� �۾��� �ϴ� �Լ��� ȣ���ؾ��Ѵ�.
					 * UEqupmentSlot::NativeDrop���� �ϴ� ��ƾ�� �Լ�ȭ �ؾ���
					 */
				}
			}
		}

		//���� ����.
		//���⼭ 2���б�� ������. 
		//if������ ������ ���� INV�� �ִ� Equipment�� Drag&Drop���� �������� �ʰ�. 
		//Equip��ư�� ���� �����ϴ°��
		

		if (ItemObj && ItemObj->bIsDestoryed)
		{
			//UCustomInventoryLibrary::SpawnEquipment(GetWorld(), ItemObj); //�Լ�ȭ ��.
			//�� SpawnEquipment���� BChar�� �Ѱ��ִ� �Ű������� �߰��ؾߵ�.
			//�ش� �Լ����� �ٷ� StepEquip�� ������ ���⼱ �׳� return�ϸ��.

			//���� ������ Equipment�� invComp�� ItemObj->InvComp���� �̰�����.
			UCustomInventoryLibrary::SpawnEquipment(GetWorld(), ItemObj, Actor);
			return true;
		}
		//�� ���� INV�� �ִ� Equipment�� Drag&Drop���� ���� �������. (EquipSlot::TrySlotEquip���� ȣ����)
		else
		{
			//Destoryed�� �ƴϸ� �׳� �� ���ϰ� �ٷ� step equip����
			bReturn = StepEquip(Actor, RifleSlot); 
		}
	}
	return bReturn;
}

bool AEquipment::StepEquip(AActor* Actor, ERifleSlot RifleSlot)
{
	bool bReturn = false;
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	const USkeletalMeshSocket* Socket = nullptr;
	switch (ItemSetting.DataAsset->EquipmentType)
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
	if (Socket != nullptr || (ItemSetting.DataAsset->EquipmentType == EEquipmentType::EET_Rifle || ItemSetting.DataAsset->EquipmentType == EEquipmentType::EET_Pistol))
	{
		if (Socket != nullptr)
		{
			Socket->AttachActor(this, BChar->GetMesh());
			SetActorRelativeTransform(ItemSetting.DataAsset->MeshAttachTransform);
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
		BChar->Equipment->AddEquipment(ItemObj);

		bReturn = true;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Equipment::StepEquip, Socket is null "));
	}

	/*if (EquippedSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), EquippedSound, Main->GetActorLocation());
	}*/

	return bReturn;
}



void AEquipment::SendToInventory(AActor* Actor)
{
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	check(BChar);

	OwningEquipment = nullptr;
	ItemObj->bIsDestoryed = true;

	//���� �߰��� InventoryComponent�� ������.
	/*if (bHasStorage && EquipInventoryComp)
	{
		ItemObj->SetItemInvComp(EquipInventoryComp);
	}*/

	//�θ� class�� �ִ� Pickup�Լ� ȣ���ؼ� item�� ��´�.
	Pickup(BChar);
}

//�� �Լ��� ���߿� CustomInventoryLibrary�� �����ҵ�.
//ItemObj�� ����ؾߵ�.


void AEquipment::Drop()
{
	Super::Drop();

	UE_LOG(LogTemp, Warning, TEXT("AWeapon::Drop"));
	SKMesh->SetHiddenInGame(true);
	OwningEquipment = false;


}

void AEquipment::Remove()
{
	/* �ƹ��͵� ����. .. ��..*/
	//Weapon�� Remove�� ����. �� ȣ���ؾߵ�.

	if (ItemSetting.DataAsset->bHasStorage)// && EquipInventoryComp)
	{
		//ItemObj->SetItemInvComp(EquipInventoryComp);
	}

}