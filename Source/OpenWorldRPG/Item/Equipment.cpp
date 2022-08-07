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

bool AEquipment::Equip(AActor* Actor, ERifleSlot RifleSlot)
{
	bool bReturn = false;
	UE_LOG(LogTemp, Warning, TEXT("AEquipment::StepEquip func called"));
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	
	if (BChar)
	{
		/*이 무기의 타입과 일치하는 무기가 이미 있다면
		 * 1. 이 무기가 월드에 스폰된 상태라면, Inventory에 추가를 시도한다.
		 * 2. 이 무기가 Inventory에 있는 상태라면 원래 장착된 장비와 바꿔 장착 한다.
		 * 일치하는 무기가 없다면 Equip함수를 호출한다.
		 */
		if (BChar->Equipment->IsSameTypeExist(this,RifleSlot))
		{
			if (GetItemState() == EItemState::EIS_Spawn)
			{
				//인벤토리로 이 item을 보내고 함수를 종료한다.
				SendToInventory(BChar);
				return true;
			}
			else if (GetItemState() == EItemState::EIS_Pickup)
			{
				/* 장착 하려는 장비가 Pickup상태(Inventory에 있는 상태)면
				 * 장비 Swap을 진행한다.
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

				//임시로 주석처리함. GetEquippedWeaponSameType 함수 버전 변경중.
				//AEquipment* BeforeEquipment = BChar->Equipment->GetEquippedWeaponSameType(EEquipmentType::EET_MAX, this, RifleSlot);
				//if (BeforeEquipment)
				{
					//BChar->Equipment->RemoveEquipment(BeforeEquipment); //SwapEquipment(BeforeEquipment, this);
					//BeforeEquipment->SendToInventory(BChar);
					/*this를 SpawnActor해서 Data를 이관하는 작업을 하는 함수를 호출해야한다.
					 * UEqupmentSlot::NativeDrop에서 하는 루틴을 함수화 해야함
					 */
				}
			}
		}

		//장착 시작.
		//여기서 2개분기로 나뉜다. 
		//if문으로 빠지는 경우는 INV에 있는 Equipment를 Drag&Drop으로 장착하지 않고. 
		//Equip버튼을 눌러 장착하는경우
		

		if (ItemObj && ItemObj->bIsDestoryed)
		{
			//UCustomInventoryLibrary::SpawnEquipment(GetWorld(), ItemObj); //함수화 함.
			//위 SpawnEquipment에서 BChar를 넘겨주는 매개변수를 추가해야됨.
			//해당 함수에서 바로 StepEquip을 때리고 여기선 그냥 return하면됨.

			//새로 생성한 Equipment의 invComp는 ItemObj->InvComp에서 이관해줌.
			UCustomInventoryLibrary::SpawnEquipment(GetWorld(), ItemObj, Actor);
			return true;
		}
		//이 경우는 INV에 있는 Equipment를 Drag&Drop으로 장착 했을경우. (EquipSlot::TrySlotEquip에서 호출함)
		else
		{
			//Destoryed가 아니면 그냥 뭐 안하고 바로 step equip으로
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

	//Socket이 있거나, WeaponType이면 해당 (Weapon은 WeaponClass에서 진행하기 때문에 Socket이 없음)
	if (Socket != nullptr || (ItemSetting.DataAsset->EquipmentType == EEquipmentType::EET_Rifle || ItemSetting.DataAsset->EquipmentType == EEquipmentType::EET_Pistol))
	{
		if (Socket != nullptr)
		{
			Socket->AttachActor(this, BChar->GetMesh());
			SetActorRelativeTransform(ItemSetting.DataAsset->MeshAttachTransform);
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

	//새로 추가함 InventoryComponent를 갱신함.
	/*if (bHasStorage && EquipInventoryComp)
	{
		ItemObj->SetItemInvComp(EquipInventoryComp);
	}*/

	//부모 class에 있는 Pickup함수 호출해서 item을 담는다.
	Pickup(BChar);
}

//이 함수는 나중에 CustomInventoryLibrary에 빼야할듯.
//ItemObj도 사용해야됨.


void AEquipment::Drop()
{
	Super::Drop();

	UE_LOG(LogTemp, Warning, TEXT("AWeapon::Drop"));
	SKMesh->SetHiddenInGame(true);
	OwningEquipment = false;


}

void AEquipment::Remove()
{
	/* 아무것도 안함. .. 음..*/
	//Weapon의 Remove가 있음. 꼭 호출해야됨.

	if (ItemSetting.DataAsset->bHasStorage)// && EquipInventoryComp)
	{
		//ItemObj->SetItemInvComp(EquipInventoryComp);
	}

}