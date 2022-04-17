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
#include "OpenWorldRPG/NewInventory/NewItemObject.h"
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
			EquipGridWidget = CreateWidget<UNewInventoryGrid>(MainCon, WEquipGridWidget);
			//EquipGridWidget = CreateWidget<UNewInventoryGrid>(this, WEquipGridWidget);
			
			EquipGridWidget->GridInitialize(EquipInventoryComp, EquipInventoryComp->TileSize);
		}
	
	}
}

void AEquipment::ReInitialize(UNewItemObject* Obj)
{
	if(ItemObj)
	{
		ItemObj = Obj;
		if(bHasStorage)
		{
			AMainController* MainCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

			if (WEquipGridWidget && MainCon)
			{
				EquipGridWidget = CreateWidget<UNewInventoryGrid>(MainCon, WEquipGridWidget);
				EquipGridWidget->GridInitialize(EquipInventoryComp, EquipInventoryComp->TileSize);
			}
		}
	}
}

UNewItemObject* AEquipment::GetDefaultItemObj()
{
	UNewItemObject* Obj = Super::GetDefaultItemObj();

	if(Obj && bHasStorage && EquipInventoryComp)
	{
		Obj->InvComp = EquipInventoryComp;
	}
	return Obj;
}

void AEquipment::SetOwningPlayer(AActor* Actor)
{
	if (OwningPlayer != Actor)
	{
		AMainCharacter* Main = Cast<AMainCharacter>(Actor);
		if (Main)
		{
			OwningPlayer = Main;
			SetInstigator(Main); //Instigator ����.
		}
	}
}

void AEquipment::StepEquip(AActor* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("AEquipment::StepEquip func called"));
	AMainCharacter* Main = Cast<AMainCharacter>(Actor);

	if (Main)
	{
		/*�� ������ Ÿ�԰� ��ġ�ϴ� ���Ⱑ �̹� �ִٸ�
		 * 1. �� ���Ⱑ ���忡 ������ ���¶��, Inventory�� �߰��� �õ��Ѵ�.
		 * 2. �� ���Ⱑ Inventory�� �ִ� ���¶�� ���� ������ ���� �ٲ� ���� �Ѵ�.
		 * ��ġ�ϴ� ���Ⱑ ���ٸ� Equip�Լ��� ȣ���Ѵ�.
		 */
		if (Main->Equipment->IsSameTypeExist(this))
		{
			if (GetItemState() == EItemState::EIS_Spawn)
			{
				//�κ��丮�� �� item�� ������ �Լ��� �����Ѵ�.
				SendToInventory(Main);
				return;
			}
			else if (GetItemState() == EItemState::EIS_Pickup)
			{
				/* ���� �Ϸ��� ��� Pickup����(Inventory�� �ִ� ����)��
				 * ��� Swap�� �����Ѵ�.
				 */
				AEquipment* Beforeweapon = Main->Equipment->GetEquippedWeaponSameType(this);
				if (Beforeweapon)
				{
 					Main->Equipment->SwapEquipment(Beforeweapon, this);
					Beforeweapon->SendToInventory(Main);
					/*this�� SpawnActor�ؼ� Data�� �̰��ϴ� �۾��� �ϴ� �Լ��� ȣ���ؾ��Ѵ�.
					 * UEqupmentSlot::NativeDrop���� �ϴ� ��ƾ�� �Լ�ȭ �ؾ���.
					 *
					 */
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
	case EEquipmentType::EET_Pistol:
	case EEquipmentType::EET_Rifle:
	{
		AWeapon* Weapon = Cast<AWeapon>(this);
		check(Weapon);

		/* Weapon�� Primary, Sub�� �����Ǿ����� �������� ��/�ι���� �����Ѵ�. */

		if (Weapon->RifleAssign == ERifleAssign::ERA_MAX)
		{
			/* 1,2,3�� �������� Quick Swap�ϱ� ���� */
			if (EquipmentType == EEquipmentType::EET_Rifle) //�������̰�
			{
				if (Main->PrimaryWeapon) //�̹� �ֹ��Ⱑ ������
				{
					Weapon->RifleAssign = ERifleAssign::ERA_Sub;
					Main->SubWeapon = Weapon;// (AWeapon*)this; //�ι���� ����
				}
				else //�ֹ��Ⱑ ������
				{
					Weapon->RifleAssign = ERifleAssign::ERA_Primary;
					Main->PrimaryWeapon = Weapon;// (AWeapon*)this; //�ֹ����
				}
			}
			else //�ǽ���
			{
				Main->PistolWeapon = Weapon; // (AWeapon*)this;
			}
		}


		//��� �ִ� ���Ⱑ ���� ��� ���� Weapon�� �鵵�� �Ѵ�.
		if (Main->EquippedWeapon == nullptr)
		{
			if (EquipmentType == EEquipmentType::EET_Rifle)
			{
				if (Main->PrimaryWeapon)
				{
					Main->ChangeWeapon(1);
				}
				else if (Main->SubWeapon)
				{
					Main->ChangeWeapon(2);
				}

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

		//SKMesh->SetHiddenInGame(true); //�ӽ÷� �صа���.
	}
	break;
	case EEquipmentType::EET_Vest:
	{
		//����
		const USkeletalMeshSocket* Socket = Main->GetMesh()->GetSocketByName("VestSocket");
		Socket->AttachActor(this, Main->GetMesh());
	}
	break;
	case EEquipmentType::EET_Backpack:
	{
		//SKMesh->SetHiddenInGame(true); //�ӽ�
	}
	}

	//Main�� �ִ� Equipment�� Add���ش�.
	Main->Equipment->AddEquipment(this);
	SetOwningPlayer(Main);

	

	Mesh->SetSimulatePhysics(false);
	Mesh->SetEnableGravity(false);

	Mesh->SetHiddenInGame(true); //Static Mesh�� �Ⱥ��̰� �ϰ�, Collision�� ����.
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

	//�� ����� ���� Ÿ���� ���Ⱑ �̹� �����Ǿ��ְ� (Rifle�� ��� 2�� ��������)
	//if (Main->Equipment->IsWeaponExist(this))
	{
		//if (GetItemState() == EItemState::EIS_Spawn) //���忡 �ִ� �����
		{
			//OwningEquipment�� null�� �������ش�.
			OwningEquipment = nullptr;
			ItemObj->bIsDestoryed = true;
			//Inventory�� �̵��ؾ���.
			Pickup(Main);
	//		return true;
		}
		//else //Pickup���¸� (Inventory�� �ִ� ������) -> �������ش� //��� ���� ���� �Լ��� �����ؾ��ҵ��ϴ�.
		//{
		//	AEquipment* Beforeweapon = Main->Equipment->GetBeforeWeapon(this);
		//	if (Beforeweapon != nullptr)
		//	{
		//		Beforeweapon->OwningEquipment = nullptr;
		//		Beforeweapon->Pickup(Main); //���� ���⸦ Inventory�� ������. 
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

void AEquipment::Remove()
{
	/* �ƹ��͵� ����. .. ��..*/


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
