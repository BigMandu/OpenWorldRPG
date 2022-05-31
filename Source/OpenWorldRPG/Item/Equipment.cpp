// Fill out your copyright notice in the Description page of Project Settings.


#include "Equipment.h"
#include "Equipment.h"
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

	SKMesh->SetHiddenInGame(true); //AI debug������ �ӽ÷� false��Ŵ

}

void AEquipment::BeginPlay()
{
	Super::BeginPlay();

	if (bHasStorage)
	{
		SettingStorage();
	}

}

void AEquipment::ReInitialize(UNewItemObject* Obj)
{
	if(ItemObj)
	{
		ItemObj = Obj;
		if(bHasStorage)
		{
			if (MainCon == nullptr) 
			{
				MainCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
			}
			//AMainController* MainCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
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

void AEquipment::StepEquip(AActor* Actor)
{
	UE_LOG(LogTemp, Warning, TEXT("AEquipment::StepEquip func called"));
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);

	if (BChar)
	{
		/*�� ������ Ÿ�԰� ��ġ�ϴ� ���Ⱑ �̹� �ִٸ�
		 * 1. �� ���Ⱑ ���忡 ������ ���¶��, Inventory�� �߰��� �õ��Ѵ�.
		 * 2. �� ���Ⱑ Inventory�� �ִ� ���¶�� ���� ������ ���� �ٲ� ���� �Ѵ�.
		 * ��ġ�ϴ� ���Ⱑ ���ٸ� Equip�Լ��� ȣ���Ѵ�.
		 */
		if (BChar->Equipment->IsSameTypeExist(this))
		{
			if (GetItemState() == EItemState::EIS_Spawn)
			{
				//�κ��丮�� �� item�� ������ �Լ��� �����Ѵ�.
				SendToInventory(BChar);
				return;
			}
			else if (GetItemState() == EItemState::EIS_Pickup)
			{
				/* ���� �Ϸ��� ��� Pickup����(Inventory�� �ִ� ����)��
				 * ��� Swap�� �����Ѵ�.
				 */
				AEquipment* Beforeweapon = BChar->Equipment->GetEquippedWeaponSameType(this);
				if (Beforeweapon)
				{
					BChar->Equipment->SwapEquipment(Beforeweapon, this);
					Beforeweapon->SendToInventory(BChar);
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
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	switch (EquipmentType)
	{
	//case EEquipmentType::EET_Pistol:
	//case EEquipmentType::EET_Rifle:
	/*
	 * {
	
		AWeapon* Weapon = Cast<AWeapon>(this);
		check(Weapon);

		// Weapon�� Primary, Sub�� �����Ǿ����� �������� ��/�ι���� �����Ѵ�.

		if (Weapon->RifleAssign == ERifleAssign::ERA_MAX)
		{
			// 1,2,3�� �������� Quick Swap�ϱ� ���� 
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
	*/
	
		//break;
	case EEquipmentType::EET_Helmet:
	{
		const USkeletalMeshSocket* Socket = BChar->GetMesh()->GetSocketByName("headsocket");
		if (Socket)
		{
			if (Socket->AttachActor(this, BChar->GetMesh()))
			{
				SetActorRelativeTransform(MeshAttachTransform);
			}
		}
	}
	break;
	case EEquipmentType::EET_Vest:
	{
		//����
		const USkeletalMeshSocket* Socket = BChar->GetMesh()->GetSocketByName("VestSocket");
		if(Socket)
		{
			if(Socket->AttachActor(this, BChar->GetMesh()))
			{
				SetActorRelativeTransform(MeshAttachTransform);
			}			
		}
	}
	break;
	case EEquipmentType::EET_Backpack:
	{
		const USkeletalMeshSocket* Socket = BChar->GetMesh()->GetSocketByName("BackpackSocket");
		if (Socket)
		{
			if (Socket->AttachActor(this, BChar->GetMesh()))
			{
				SetActorRelativeTransform(MeshAttachTransform);
			}
		}
	}
	}

	

	//Main�� �ִ� Equipment�� Add���ش�.
	BChar->Equipment->AddEquipment(this);
	SetOwningPlayer(BChar);

	
	SKMesh->SetHiddenInGame(false);

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
	ABaseCharacter* BChar = Cast<ABaseCharacter>(Actor);
	check(BChar);

	//�� ����� ���� Ÿ���� ���Ⱑ �̹� �����Ǿ��ְ� (Rifle�� ��� 2�� ��������)
	//if (Main->Equipment->IsWeaponExist(this))
	{
		//if (GetItemState() == EItemState::EIS_Spawn) //���忡 �ִ� �����
		{
			//OwningEquipment�� null�� �������ش�.
			OwningEquipment = nullptr;
			ItemObj->bIsDestoryed = true;
			//Inventory�� �̵��ؾ���.
			Pickup(BChar);
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

void AEquipment::SettingStorage()
{
	if (MainCon == nullptr)
	{
		MainCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));
	}
	//AMainController* MainCon = Cast<AMainController>(UGameplayStatics::GetPlayerController(GetWorld(), 0));

	if (WEquipGridWidget && MainCon)
	{
		EquipGridWidget = CreateWidget<UNewInventoryGrid>(MainCon, WEquipGridWidget);
		//EquipGridWidget = CreateWidget<UNewInventoryGrid>(this, WEquipGridWidget);

		EquipGridWidget->GridInitialize(EquipInventoryComp, EquipInventoryComp->TileSize);
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
