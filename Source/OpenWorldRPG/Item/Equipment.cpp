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
	//EquipGridWidget이 없을때만 생성한다.
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

				//EquipInventoryComp = ItemObj->GetItemInvComp(); //Item Swap시에 문제 발생 (ItemObj의 InvComp가 null만 가지고 있음)
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
		//if(bHasStorage) 여기는 그냥 바로 SettingStorage로 빼버린다.
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

//부모 클래스에서 Beginplay시 한번만 사용됨.
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
			SetInstigator(BChar); //Instigator 설정.
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

////Inventory와 Equipped간의 장비 Swap
//void AEquipment::SwapBetweenInvAndEquipped(ABaseCharacter* BChar, UNewItemObject* ToInventory)
//{
//	if (ToInventory && ToInventory->Equipment)
//	{
//		UE_LOG(LogTemp, Warning, TEXT("Equipment::SwapEquip / need to same below obj"));
//		UE_LOG(LogTemp,Warning, TEXT("Equipment::SwapEquip / call RemoveEquip func. Remove Equip : %s"), *ToInventory->GetFName().ToString());
//		BChar->Equipment->RemoveEquipment(ToInventory);
//		//Obj를 넘겨버리자. 
//		UE_LOG(LogTemp, Warning, TEXT("Equipment::SwapEquip / call SendToInv func. Send Equip : %s"),*ToInventory->GetFName().ToString());
//		ToInventory->Equipment->SendToInventory(BChar, ToInventory);
//	}
//}
//
////Equipped Weapon간의 Slot swap (Rifle type만 해당)
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
		/*이 무기의 타입과 일치하는 무기가 이미 있다면 아래 2가지 조건에 따라 분기한다.
		 * 1. 이 무기가 월드에 스폰된 상태라면, Inventory에 추가를 시도한다.
		 * 2. 이 무기가 Inventory에 있는 상태라면 원래 장착된 장비와 바꿔 장착 한다.
		 * 일치하는 무기가 없다면 Equip함수를 호출한다.
		 */
		if (BChar->Equipment->IsSameTypeExist(this,RifleSlot))
		{
			if (GetItemState() == EItemState::EIS_Spawn)
			{
				////이미 장착중인 Weapon을 장착된 슬롯에 Equip을 시도한다면 slot을 서로 swap한다.
				//AWeapon* Weapon = Cast<AWeapon>(this);
				//if (Weapon)
				//{
				//	BeforeEquippedObj = BChar->Equipment->GetEquippedWeaponSameType(EEquipmentType::EET_Rifle, nullptr, RifleSlot);
				//	SwapBetweenEquipped(BChar, BeforeEquippedObj);
				//}
				//else
				{
					//일반 장비면 인벤토리로 이 item을 보내고 함수를 종료한다.
					SendToInventory(BChar,nullptr);
					return true;
				}
			}

			//아래 코드들은 Swap기능 관련 코드들이다. Swap기능은 없애기로 했다.

			//else if (GetItemState() == EItemState::EIS_Pickup)
			//{
			////장비간 Swap 기능은 없앰.

			//	/* 장착 하려는 장비가 Pickup상태(Inventory에 있는 상태)면
			//	 * 장비 Swap을 진행한다.
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
			//Equip을 하고나서 기존에 장착했던걸 옮긴다.. 새로 장착할 이 weapon이 attach socket에 부착되는 단점이 있지만
			//Inventory 정리가 더 깔끔하기 때문에 이렇게 했다.
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

	//Socket이 있거나, WeaponType이면 해당 (Weapon은 WeaponClass에서 진행하기 때문에 Socket이 없음)
	//if (Socket != nullptr || (ItemSetting.DataAsset->EquipmentType == EEquipmentType::EET_Rifle || ItemSetting.DataAsset->EquipmentType == EEquipmentType::EET_Pistol))
	{
		if (Socket != nullptr)
		{
			Socket->AttachActor(this, BChar->GetMesh());
			SetActorRelativeTransform(CPDA->MeshAttachTransform);
		}

		// Mesh Setting
		SKMesh->SetHiddenInGame(false);
		Mesh->SetHiddenInGame(true); //Static Mesh를 안보이게 하고, Collision을 끈다.
		Mesh->SetSimulatePhysics(false);
		Mesh->SetEnableGravity(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SetOwningPlayer(BChar);
		
		//Main에 있는 Equipment에 Add해준다.
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

	//새로 추가함 InventoryComponent를 갱신함.
	/*if (bHasStorage && EquipInventoryComp)
	{
		ItemObj->SetItemInvComp(EquipInventoryComp);
	}*/

	UE_LOG(LogTemp,Warning,TEXT("Equipment::SendToInv / call Pickup func"));
	//부모 class에 있는 Pickup함수 호출해서 item을 담는다.
	Pickup(BChar,obj);
}

//이 함수는 나중에 CustomInventoryLibrary에 빼야할듯.
//ItemObj도 사용해야됨.


void AEquipment::Drop()
{
	Super::Drop();

	UE_LOG(LogTemp, Warning, TEXT("AEquipment::Drop"));
	SKMesh->SetHiddenInGame(true);
	OwningEquipment = false;


}

void AEquipment::Remove()
{
	/* 아무것도 안함. .. 음..*/
	//Weapon의 Remove가 있음. 꼭 호출해야됨.

	//if (ItemSetting.DataAsset->bHasStorage)// && EquipInventoryComp)
	{
		//ItemObj->SetItemInvComp(EquipInventoryComp);
	}

}