// Fill out your copyright notice in the Description page of Project Settings.



#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/EquipmentComponent.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"
#include "OpenWorldRPG/EnemyAIController.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
#include "Sound/SoundCue.h"


AEquipment::AEquipment() : Super()
{
	SKMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	EquipInventoryComp = CreateDefaultSubobject<UNewInventoryComponent>(TEXT("EquipInventoryComp"));

	RootComponent = SKMesh;
	Mesh->SetupAttachment(GetRootComponent());
	//Mesh->CanCharacterStepUpOn = ECB_No;

	SKMesh->SetHiddenInGame(true);
	SKMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_EngineTraceChannel2, ECollisionResponse::ECR_Overlap);

}

void AEquipment::BeginPlay()
{
	Super::BeginPlay();

	/*if (bHasStorage)
	{
		SettingStorage();
	}*/

}

void AEquipment::ReInitialize(UNewItemObject* Obj)
{
	if(ItemObj && EquipGridWidget == nullptr)
	{
		ItemObj = Obj;
		if(bHasStorage)
		{
			/*if (MainCon == nullptr) 
			{
				MainCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
			}*/
			//AMainController* MainCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
			if (WEquipGridWidget)
			{
				if(MainCon)
				{
					EquipGridWidget = CreateWidget<UNewInventoryGrid>(MainCon, WEquipGridWidget);
					EquipGridWidget->GridInitialize(EquipInventoryComp, EquipInventoryComp->TileSize);


				}
				/*else if(AICon)
				{
					EquipGridWidget = CreateWidget<UNewInventoryGrid>(AICon, WEquipGridWidget);
				}*/
				
				
			}
		}
	}
}

UNewItemObject* AEquipment::GetDefaultItemObj()
{
	UNewItemObject* Obj = Super::GetDefaultItemObj();

	if(Obj && bHasStorage && EquipInventoryComp)
	{
		Obj->SetItemInvComp(EquipInventoryComp);
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
				 * 
				 * Weapon의 경우, RifleAssign에 
				 */
				AEquipment* BeforeEquipment = BChar->Equipment->GetEquippedWeaponSameType(EEquipmentType::EET_MAX, this, RifleSlot);
				if (BeforeEquipment)
				{
					BChar->Equipment->SwapEquipment(BeforeEquipment, this);
					BeforeEquipment->SendToInventory(BChar);
					/*this를 SpawnActor해서 Data를 이관하는 작업을 하는 함수를 호출해야한다.
					 * UEqupmentSlot::NativeDrop에서 하는 루틴을 함수화 해야함.
					 *
					 */
				}
			}
		}
		bReturn = StepEquip(Actor, RifleSlot);
	}
	return bReturn;
}

bool AEquipment::StepEquip(AActor* Actor, ERifleSlot RifleSlot)
{
	bool bReturn = false;
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	const USkeletalMeshSocket* Socket = nullptr;
	switch (EquipmentType)
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

	if (Socket != nullptr || (EquipmentType == EEquipmentType::EET_Rifle || EquipmentType == EEquipmentType::EET_Pistol))
	{
		if (Socket != nullptr)
		{
			Socket->AttachActor(this, BChar->GetMesh());
			SetActorRelativeTransform(MeshAttachTransform);
		}
		//if (Socket->AttachActor(this, BChar->GetMesh()))

		SKMesh->SetHiddenInGame(false);
		Mesh->SetHiddenInGame(true); //Static Mesh를 안보이게 하고, Collision을 끈다.
		Mesh->SetSimulatePhysics(false);
		Mesh->SetEnableGravity(false);
		Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		SetOwningPlayer(BChar);
		//Main에 있는 Equipment에 Add해준다.
		BChar->Equipment->AddEquipment(this);

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

	//이 무기와 같은 타입의 무기가 이미 장착되어있고 (Rifle인 경우 2개 장착가능)
	//if (Main->Equipment->IsWeaponExist(this))
	{
		//if (GetItemState() == EItemState::EIS_Spawn) //월드에 있는 무기면
		{
			//OwningEquipment를 null로 설정해준다.
			OwningEquipment = nullptr;
			ItemObj->bIsDestoryed = true;
			//Inventory로 이동해야함.
			Pickup(BChar);
	//		return true;
		}
		//else //Pickup상태면 (Inventory에 있는 무기임) -> 스왑해준다 //얘는 따로 빼서 함수를 구현해야할듯하다.
		//{
		//	AEquipment* Beforeweapon = Main->Equipment->GetBeforeWeapon(this);
		//	if (Beforeweapon != nullptr)
		//	{
		//		Beforeweapon->OwningEquipment = nullptr;
		//		Beforeweapon->Pickup(Main); //기존 무기를 Inventory로 보낸다. 
		//	}
		//	UE_LOG(LogTemp, Warning, TEXT("Weapon::CheckSendToInventory. something wrong."));
		//	return false;
		//}
	}
	//return false;
}

void AEquipment::SettingStorage()
{
	//EquipGridWidget이 없을때만 생성한다.
	if (EquipGridWidget == nullptr)
	{
		if (MainCon == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("SettingStorage::MainCon is nullptr try Get Controller"));
			if (OwningPlayer)
			{
				MainCon = Cast<AMainController>(OwningPlayer->GetController());
			}
		}
		//AMainController* MainCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

		if (WEquipGridWidget && MainCon)
		{
			EquipGridWidget = CreateWidget<UNewInventoryGrid>(MainCon, WEquipGridWidget);
			if (EquipGridWidget && EquipInventoryComp)
			{
				UE_LOG(LogTemp, Warning, TEXT("SettingStorage::Try Initialize EquipGridWidget "));
				EquipGridWidget->GridInitialize(EquipInventoryComp, EquipInventoryComp->TileSize);
			}
		}
	}
}

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

}

/*
AEquipment* AEquipment::SpawnEquip(UNewItemObject* Obj, AActor* Actor)
{
	if (Obj && Actor)
	{
		AEquipment* T_Equipment = Cast<AEquipment>(GetWorld()->SpawnActor<AActor>(Obj->GetItemClass()));
		//AMainCharacter* Main = Cast<AMainCharacter>(Actor);
		if (T_Equipment)
		{
			Obj->bIsDestoryed = false;
			T_Equipment = this;
			T_Equipment->EquipInventoryComp = this->EquipInventoryComp;
			T_Equipment->ReInitialize(Obj);

			Obj->item = T_Equipment;
			//if(Main)
			//{
			//	T_Equipment->StepEquip(Main);
			//}
		}

		return T_Equipment;
	}
	return nullptr;
}
*/
