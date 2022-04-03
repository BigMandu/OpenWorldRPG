// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenWorldRPG/Item/Equipment.h"
#include "OpenWorldRPG/Item/EquipmentComponent.h"
#include "OpenWorldRPG/Item/Weapon.h"
#include "OpenWorldRPG/NewInventory/NewInventoryComponent.h"
#include "OpenWorldRPG/NewInventory/NewInventoryGrid.h"
#include "OpenWorldRPG/MainCharacter.h"
#include "OpenWorldRPG/MainController.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"


AEquipment::AEquipment() : Super()
{
	SKMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	EquipInventoryComp = CreateDefaultSubobject<UNewInventoryComponent>(TEXT("EquipInventoryComp"));

	RootComponent = SKMesh;
	Mesh->SetupAttachment(GetRootComponent());
	SKMesh->SetHiddenInGame(true);

}

void AEquipment::BeginPlay()
{
	Super::BeginPlay();

	if (bHasStorage)
	{
		AMainController* MainCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
		if (WEquipGridWidget && MainCon)
		{
			UNewInventoryGrid* Grid = CreateWidget<UNewInventoryGrid>(MainCon, WEquipGridWidget);
			//EquipGridWidget = CreateWidget<UNewInventoryGrid>(this, WEquipGridWidget);
			//EquipGridWidget->GridInitialize(EquipInventoryComp, EquipInventoryComp->TileSize);
		}
	}
}

void AEquipment::SetOwningPlayer(AActor* Actor)
{
	if (OwningPlayer != Actor)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(Actor);
		if (Main)
		{
			OwningPlayer = Main;
			SetInstigator(Main); //Instigator 설정.
		}
	}
}

void AEquipment::StepEquip(AActor* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("AEquipment::StepEquip func called"));
	AMainCharacter* Main = Cast<AMainCharacter>(Actor);

	if (Main)
	{
		/*이 무기의 타입과 일치하는 무기가 이미 있다면
		 * 1. 이 무기가 월드에 스폰된 상태라면, Inventory에 추가를 시도한다.
		 * 2. 이 무기가 Inventory에 있는 상태라면 원래 장착된 장비와 바꿔 장착 한다.
		 * 일치하는 무기가 없다면 Equip함수를 호출한다.
		 */
		if (Main->Equipment->IsSameTypeExist(this))
		{
			if (GetItemState() == EItemState::EIS_Spawn)
			{
				//인벤토리로 이 item을 보내고 함수를 종료한다.
				SendToInventory(Main);
				return;
			}
			else if (GetItemState() == EItemState::EIS_Pickup)
			{
				/* 장착 하려는 장비가 Pickup상태(Inventory에 있는 상태)면
				 * 장비 Swap을 진행한다.
				 */
				AEquipment* Beforeweapon = Main->Equipment->GetEquippedWeaponSameType(this);
				if (Beforeweapon)
				{
					Main->Equipment->SwapEquipment(Beforeweapon, this);
					Beforeweapon->SendToInventory(Main);
				}
			}
		}
		Equip(Actor);
	}
}

void AEquipment::Equip(AActor* Actor)
{
	AMainCharacter* Main = Cast<AMainCharacter>(Actor);
	switch (EquipmentType)
	{
	case EEquipmentType::EET_Rifle:
	case EEquipmentType::EET_Pistol:
	{
		AWeapon* Weapon = Cast<AWeapon>(this);
		/* 1,2,3을 눌렀을때 Quick Swap하기 위해 */
		if (EquipmentType == EEquipmentType::EET_Rifle && Weapon) //라이플이고
		{
			if (Main->PrimaryWeapon) //이미 주무기가 있으면
			{
				Main->SubWeapon = Weapon;// (AWeapon*)this; //부무기로 지정
			}
			else //주무기가 없으면
			{
				Main->PrimaryWeapon = Weapon;// (AWeapon*)this; //주무기로
			}
		}
		else //피스톨
		{
			Main->PistolWeapon = Weapon; // (AWeapon*)this;
		}

		//들고 있는 무기가 없을 경우 지금 Weapon을 들도록 한다.
		if (Main->EquippedWeapon == nullptr)
		{
			if (EquipmentType == EEquipmentType::EET_Rifle)
			{
				Main->ChangeWeapon(1);
			}
			else
			{
				Main->ChangeWeapon(3);
			}

			//GunAttachToMesh(Main);
		}
	}
	break;
	case EEquipmentType::EET_Helmet:
	{

		//SKMesh->SetHiddenInGame(true); //임시로 해둔것임.
	}
	break;
	case EEquipmentType::EET_Vest:
	{
		//장착
		const USkeletalMeshSocket* Socket = Main->GetMesh()->GetSocketByName("VestSocket");
		Socket->AttachActor(this, Main->GetMesh());
		SKMesh->SetHiddenInGame(false); //임시로 해둔것임.
	}
	break;
	case EEquipmentType::EET_Backpack:
	{
		//SKMesh->SetHiddenInGame(true); //임시
	}
	}

	//Main에 있는 Equipment에 Add해준다.
	Main->Equipment->AddEquipment(this);
	SetOwningPlayer(Main);

	Mesh->SetSimulatePhysics(false);
	Mesh->SetEnableGravity(false);

	Mesh->SetHiddenInGame(true); //Static Mesh를 안보이게 하고, Collision을 끈다.
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);


	/*if (EquippedSound)
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), EquippedSound, Main->GetActorLocation());
	}*/


}

void AEquipment::SendToInventory(AActor* Actor)
{
	AMainCharacter* Main = Cast<AMainCharacter>(Actor);
	check(Main);

	//이 무기와 같은 타입의 무기가 이미 장착되어있고 (Rifle인 경우 2개 장착가능)
	//if (Main->Equipment->IsWeaponExist(this))
	{
		//if (GetItemState() == EItemState::EIS_Spawn) //월드에 있는 무기면
		{
			//OwningEquipment를 null로 설정해준다.
			OwningEquipment = nullptr;

			//Inventory로 이동해야함.
			Pickup(Main);
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

void AEquipment::Drop()
{
	Super::Drop();

	UE_LOG(LogTemp, Warning, TEXT("AWeapon::Drop"));
	SKMesh->SetHiddenInGame(true);
	OwningEquipment = false;


}